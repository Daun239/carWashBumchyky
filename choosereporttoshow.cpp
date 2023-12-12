#include "choosereporttoshow.h"
#include "ui_choosereporttoshow.h"
#include <QDate>
#include "taskdata.h"
#include <QDateTime>
#include <QFile>
#include <algorithm>
#include <QSet>
#include "workerData.h"
#define BODY 250
#define BODY_AND_SALON 350
#define DRY_CLEANING 1800
#define COLUMNCOUNT 7


//Global variables - - - - - - - - - - - - - -
bool isSortedAscending = true;
static QList<taskData*> allTasks;
static QSet<QString> workersList;



// - - - - - - - - - - - - - - - - - - - - - - - - - - -


void sortTasksByTime() {
    std::sort(allTasks.begin() , allTasks.end() , [](const taskData* first , const taskData* second) {
        if (isSortedAscending)
        return first->getDuration() > second->getDuration() ;
        else
        return first->getDuration() < second->getDuration();
    });
}

void sortTasksByNumber() {
    std::sort(allTasks.begin() , allTasks.end() , [](const taskData* first , const taskData* second) {
        if (isSortedAscending)
            return first->getNumber() > second->getNumber() ;
        else
            return first->getNumber() < second->getNumber();
    });
}

//-----------------------------------------------------------------------------

chooseReportToShow::chooseReportToShow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::chooseReportToShow)
{
    ui->setupUi(this);

    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);

    int dayOfMonth = now->tm_mday;
    int month = now->tm_mon + 1;
    int year = now->tm_year + 1900;
    QDate currentDate; currentDate.setDate(year , month , dayOfMonth);

    ui->dateEdit->setDate(currentDate);

    ui->sortByComboBox->addItem("Number");
    ui->sortByComboBox->addItem("Time");

    ui->workersComboBox->setCurrentIndex(0);
    connect(ui->sortByComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=] () {
        chooseReportToShow::sortTasks() ; chooseReportToShow::printTable();
    });

    connect(ui->workersComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &chooseReportToShow::printTable);

    ui->ascendingOrderButton->setArrowType(Qt::DownArrow);



}

chooseReportToShow::~chooseReportToShow()
{
    delete ui;
}

void chooseReportToShow::on_cancelSelection_clicked()
{

}


void chooseReportToShow::on_showReportButton_clicked()
{
    QDateTime currDateTime = ui->dateEdit->dateTime();
    int year = currDateTime.date().year();
    int month = currDateTime.date().month();
    int day = currDateTime.date().day();

    QString fileName = QString("data/ordinary/%1/%2/%3.%4.%5.txt").arg(year).arg(month).arg(year)
                   .arg(month < 10 ? "0" + QString::number(month) : QString::number(month))
                   .arg(day < 10 ? "0" + QString::number(day) : QString::number(day));

    qDebug() << fileName;

    // Read data from the file
    QFile fileIn(fileName);
    if (fileIn.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream input(&fileIn);
        allTasks.clear();
        while (!input.atEnd()) {
            QString line = input.readLine();
            if (line.isEmpty()) {
                qDebug() << "Empty string!";
                return;
            }

            taskData* currTask = new taskData;
            QList<QString> currWorkers;
            qsizetype index , i ;
            QString curr;
            QTime currTaskEndTime , currTaskBeginningTime;


            // Дізнаємося про номер
            index = line.indexOf('"');
            for (i = index+1 ; i < line.length() && line[i] != '"'; ++i) {
                curr+= line[i];
            }
            if (curr.isEmpty()) {
                qDebug() << "Couldn't get the number of the task";
                return;
            }

            bool isConversionOk;
            int number = curr.toInt(&isConversionOk);
            if (!isConversionOk) {
                qDebug() << "Couldn't convert the number of the task";
                return;
            }
            currTask->setNumber(number);


            curr.clear();
            index = line.indexOf('|');
            if (index == -1) {
                qDebug() << "Couldn't find the delimetr | to find workers";
                return;
            }

            bool isOut = false;
            //Дізнаємось про працівників
            for (i = index+1 ; i < line.length() && line[i] != '|' ; ) {
                while(i < line.length() && line[i] != '"') {
                    if (line[i++] == '|') {
                        isOut = true;
                        break;
                    }
                }
                if (isOut) {
                    break;
                }
                ++i;

                while(i < line.length() && line[i] != '"') {
                    curr += line[i++];
                }
                if (curr.isEmpty()) {
                    qDebug() << "Can't add an empty worker";
                    return;
                }
                ++i;
                workersList.insert(curr);
                currWorkers.push_back(curr);
                curr.clear();
            }
            currTask->setWorkers(currWorkers);

            // Дізнаємось про тип завдання.
            while(i < line.length() && line[i++] != '"');

            while(i < line.length() && line[i] != '"') {
                curr+= line[i++];
            }
            isConversionOk = false;
            int typeOfTask = curr.toInt(&isConversionOk);
            if (!isConversionOk) {
                qDebug() << "Couldn't convert the task type";
                return;
            }
            switch(typeOfTask) {
            case 1 :
                currTask->setTask(body);
                break;
            case 2:
                currTask->setTask(bodyAndSalon);
                break;
            case 3 :
                currTask->setTask(dryCleaning);
                break;
            }
            i++;

            // Дізнаємось про початок виконання завдання
            curr.clear();
            while(i < line.length() && line[i++] != '"');

            while(i < line.length() && line[i] != ':') {
                curr+= line[i++];
            }
            isConversionOk = false;
            int beginningHour = curr.toInt(&isConversionOk);
            if (!isConversionOk) {
                qDebug() << "Couldn't convert the hour of the beginning of the task";
                return;
            }
            i++;
            curr.clear();
            while(i < line.length() && line[i] != '"') {
                curr+= line[i++];
            }
            isConversionOk = false;
            int beginningMinute = curr.toInt(&isConversionOk);
            if (!isConversionOk) {
                qDebug() << "Couldn't convert the minute of the beggining of the task";
                return;
            }
            currTaskBeginningTime.setHMS(beginningHour , beginningMinute , 0 , 0);
            currTask->setBeginningTime(currTaskBeginningTime);


            // Дізнаємось про час кінця виконання замовлення
            curr.clear();
            while(i < line.length() && line[i++] != '|');

            while (i < line.length() && line[i++] != '"');

            while (i < line.length() && line[i] != ':') {
                curr+= line[i++];
            }
            isConversionOk = false;
            int endHour = curr.toInt(&isConversionOk);
            if (!isConversionOk) {
                qDebug() << "Couldn't convert end hour";
                return;
            }
            ++i;
            curr.clear();
            while (i < line.length() && line[i] != '"') {
                curr+= line[i++];
            }
            isConversionOk = false;
            int endMinute = curr.toInt(&isConversionOk);
            if (!isConversionOk) {
                qDebug() << "Couldn't convert end minute";
                return;
            }
            currTaskEndTime.setHMS(endHour , endMinute , 0 , 0);
            currTask->setEndTime(currTaskEndTime);
            allTasks.push_back(currTask);
        }
        insertWorkersIntoComboBox();
    }
   // printTable();
}

void chooseReportToShow::addItemToTable() {
     QList<taskData*> allTasks;

}

void chooseReportToShow::printTable() {
     qDebug() << "PRINT TABLE";
     ui->reportTable->setRowCount(100);
     int tasksCompletedByWorker = 0;
     if (!ui->workersComboBox->currentIndex()) {
        ui->reportTable->setRowCount(allTasks.size());
     }


     for (int i = 0 ; i < allTasks.size() ; ++i) {
        if (ui->workersComboBox->currentIndex() && !allTasks[i]->getWorkers().contains(ui->workersComboBox->currentText())) {
            continue;
        }
        tasksCompletedByWorker++;
        ui->reportTable->setCellWidget(i , 0 , new QLabel(QString::number(allTasks[i]->getNumber())));
        QString workers;
        for (int j = 0 ; j < allTasks[i]->getWorkers().size() ; j++) {
            workers+= allTasks[i]->getWorkers()[j];
            if (j != allTasks[i]->getWorkers().size() - 1) {
                workers += " , ";
            }
        }
        QString taskType, taskPrice;
        switch(allTasks[i]->getTask()) {
        case 1 :
            taskType = "Body";
            taskPrice = QString::number(BODY);
            break;
        case 2 :
            taskType = "Body and salon";
            taskPrice = QString::number(BODY_AND_SALON);
            break;
        case 3 :
            taskType = "Dry cleaning";
            taskPrice = QString::number(DRY_CLEANING);
                break;
        default :
                taskType = "*";
                taskPrice = "*";
        }

        QString beginningTime = allTasks[i]->getBeginningTime().toString("hh:mm");
        QString endTime = allTasks[i]->getEndTime().toString("hh:mm");
        QString duration = QString(allTasks[i]->getDuration().toString("hh:mm"));

        QVector<QLabel*> labels(COLUMNCOUNT);
        for (int j = 0 ; j < COLUMNCOUNT; ++j) {
                labels[j] = new QLabel;
        }
       // labels.reserve(COLUMNCOUNT);
        labels[0]->setText(QString::number(allTasks[i]->getNumber()));
        labels[1]->setText(workers);
        labels[2]->setText(taskType);
        labels[3]->setText(taskPrice);
        labels[4]->setText(beginningTime);
        labels[5]->setText(endTime);
        labels[6]->setText(duration);
        for (int j = 0 ; j < COLUMNCOUNT; ++j) {
                labels[j]->setAlignment(Qt::AlignCenter);
                if (!ui->workersComboBox->currentIndex())
                ui->reportTable->setCellWidget(i , j , labels[j]);
                else {
                ui->reportTable->setCellWidget(tasksCompletedByWorker - 1 , j , labels[j]);
                }
        }

     }
     if (ui->workersComboBox->currentIndex())
        ui->reportTable->setRowCount(tasksCompletedByWorker);
}


void chooseReportToShow::on_ascendingOrderButton_clicked()
{
    reverseSwapOrder();
    sortTasks();
    printTable();
}

void chooseReportToShow::reverseSwapOrder() {
    if (!isSortedAscending) {
    ui->ascendingOrderButton->setArrowType(Qt::ArrowType::UpArrow);
    }
    else
    ui->ascendingOrderButton->setArrowType(Qt::ArrowType::DownArrow);
    isSortedAscending = !isSortedAscending;
}

void chooseReportToShow::sortTasks() {
    switch(ui->sortByComboBox->currentIndex()) {
    case 0 :
    sortTasksByNumber();
    break;
    case 1 :
    sortTasksByTime();
    break;
    }
}

void chooseReportToShow::insertWorkersIntoComboBox() {
    qDebug() << "INSERTCOMBOBOX";
    ui->workersComboBox->clear();
    ui->workersComboBox->addItem("All workers");
    for (auto it : workersList) {
    ui->workersComboBox->addItem(it);
    }
}

void chooseReportToShow::showIndividualWorkersInfo() {
    ui->individualWorkersTable->setRowCount(workersList.size());
    QVector<workerData> workersData(workersList.size());
    int i = 0 ;
    for (auto it : workersList) {
    workersData[i++].name = it;
    }
    for (int i = 0 ; i < allTasks.size() ; ++i) {
    if (allTasks[i]->getWorkers().contains(workersData[i].name)) {
                workersData[i].moneyGained += allTasks[i]->getPrice();
                workersData[i].tasksExecuted++;
                //workersData[i].timeToExecuteTask += allTasks[i].get
        }
    }
    for (int i = 0 ; i < workersList.size(); ++i) {
        workersData[i].moneyGained /= workersData[i].tasksExecuted;
        ui->individualWorkersTable->setCellWidget(i , 0 , new QLabel(workersData[i].name));
        ui->individualWorkersTable->setCellWidget(i , 2 , new QLabel(QString::number(workersData[i].tasksExecuted)));
        ui->individualWorkersTable->setCellWidget(i , 3 , new QLabel(QString::number(workersData[i].moneyGained)));
    }

}

