#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "taskdata.h"
#include "taskdatamenu.h"
#include <ctime>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QTimer>
#include <QStandardItemModel>
#include <QListView>
#include <QStringListModel>
#include <QLineEdit>
#include <QTimeEdit>
#include <QStackedLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QComboBox>
#include "choosereporttoshow.h"
#define BODY 250
#define BODY_AND_SALON 350
#define DRY_CLEANING 1800

//Global variables
bool isAdmin = false;
int passwordCount = 0;
int dayOfMonth, month, year;
QMenu* workersMenu;
const QString realPassword = "123";
QList<taskData*> allTasks;
QList<QAction*> workersList;
QList<QTimeEdit*> beginningTimeEditList;
QList<QTimeEdit*> endTimeEditList;
QList<QLabel*> workersLabels;
QList<QLabel*> priceLabels;
QList<QMenu*> workersMenus;


//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Carwash");

    ui->changeWorkers->setVisible(false);

    ui->stackedWidget->setCurrentWidget(ui->MainPage);
    //connect(this , &QWidget::keyPressEvent , )

    if (!createFolders()) {
        QMessageBox::information(this , "Critical error" , "Couldn't open or create folders for data");
        return;
    }
    //Створення або відкриття файлу з даними про замовлення
    readFileData();

    installEventFilter(this);

    // Set the menu for the tool button
    ui->workersToolButton->setMenu(workersMenu);

    ui->workersToolButton->setPopupMode(QToolButton::MenuButtonPopup); // QToolButton::InstantPopup);

}

//-----------------------------------------------------------------------------

MainWindow::~MainWindow()
{
    delete ui;
}

//-----------------------------------------------------------------------------

void MainWindow::on_addTaskButton_clicked()
{
    taskData* ptr;
    addTask(true , *ptr);
}


//-----------------------------------------------------------------------------

void MainWindow::readFileData() {
    // Create a menu for workers
    workersMenu = new QMenu(this);

    // Set the file path for workers' data
    QString workersFilePath = QCoreApplication::applicationDirPath() + "/data/workersData.txt";

    // Open the file for reading
    QFile file(workersFilePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Create a QTextStream for convenient reading
        QTextStream in(&file);

        // Read workers' data
        while (!in.atEnd()) {
            QString line = in.readLine();
            for (int i = 0; i < line.length(); ++i) {
                QString curr;
                // Extract worker names enclosed in double quotes
                while (i < line.length() && line[i++] != '\"');
                while (i < line.length() && line[i] != '\"') {
                    curr += line[i++];
                }
                if (curr.length() != 0) {
                    // Create a QAction for each worker
                    QAction* newAction = new QAction(curr);
                    newAction->setCheckable(true);
                    newAction->setChecked(false);
                    workersList.push_back(newAction);
                    workersMenu->addAction(newAction);
                }
            }
        }

        // Close the file after reading
        file.close();
    } else {
        // Handle the error if the file couldn't be opened
        qDebug() << "Failed to open file for reading";
    }

    // Get the current date
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);

    dayOfMonth = now->tm_mday;
    month = now->tm_mon + 1;
    year = now->tm_year + 1900;

    // Set the file name for data of this day
    fileName = QString("data/ordinary/%1/%2/%3.%4.%5.txt").arg(year).arg(month).arg(year)
                   .arg(month < 10 ? "0" + QString::number(month) : QString::number(month))
                   .arg(dayOfMonth < 10 ? "0" + QString::number(dayOfMonth) : QString::number(dayOfMonth));

    qDebug() << fileName;

    // Read data from the file
    QFile fileIn(fileName);
    if (fileIn.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream input(&fileIn);
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
                currWorkers.push_back(curr);
                curr.clear();
            }

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

            addTask(false , *currTask);
        }
    } else {
        qDebug() << "Failed to open file for reading.";
    }

    fileIn.close();
}

//-----------------------------------------------------------------------------

bool MainWindow::createFolders() {
    QString exeFilePath = QCoreApplication::applicationDirPath();
    QString dataFilePath = exeFilePath + "/data";

    // Create QDir object to work with the path
    QDir dataDir(dataFilePath);
    if (!dataDir.exists()) {
        if (dataDir.mkpath(".")) {
            qDebug() << "Data folder created successfully.";
        } else {
            qDebug() << "Failed to create data folder.";
            return false;
        }
    }

    QString backupDataFilePath = dataFilePath + "/backup";
    QDir backupDataDir(backupDataFilePath);
    if (!backupDataDir.exists()) {
        if (backupDataDir.mkpath(".")) {
            qDebug() << "Backup folder created successfully.";
        } else {
            qDebug() << "Failed to create backup folder.";
            return false;
        }
    }

    QString ordinaryDataFilePath = dataFilePath + "/ordinary";
    QDir ordinaryDataDir(ordinaryDataFilePath);
    if (!ordinaryDataDir.exists()) {
        if (ordinaryDataDir.mkpath(".")) {
            qDebug() << "Ordinary folder created successfully.";
        } else {
            qDebug() << "Failed to create ordinary folder.";
            return false;
        }
    }

    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);

    // Отримати день місяця
    int dayOfMonth = now->tm_mday;
    int month = now->tm_mon + 1;
    int year = now->tm_year + 1900;

    QString ordinaryYearsDataFilePath = ordinaryDataFilePath + "/" + QString::number(year);
    QDir ordinaryYearsDataDir(ordinaryYearsDataFilePath);
    if (!ordinaryYearsDataDir.exists()) {
        if (ordinaryYearsDataDir.mkpath(".")) {
            qDebug() << "Ordinary folder created successfully.";
        } else {
            qDebug() << "Failed to create ordinary folder.";
            return false;
        }
    }

    QString ordinaryMonthDataFilePath = ordinaryYearsDataFilePath;

    ordinaryMonthDataFilePath += "/" + QString::number(month);

    QDir ordinaryMonthsDataDir(ordinaryMonthDataFilePath);
    if (!ordinaryMonthsDataDir.exists()) {
        if (ordinaryMonthsDataDir.mkpath(".")) {
            qDebug() << "Ordinary folder created successfully.";
        } else {
            qDebug() << "Failed to create ordinary folder.";
            return false;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------

void MainWindow::setupUi() {

      ui->currentTable->setColumnCount(7);

    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);

    // Отримати день місяця
    dayOfMonth = now->tm_mday;
    month = now->tm_mon + 1;
    year = now->tm_year + 1900;
}


//-----------------------------------------------------------------------------
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this)
    {
        if (!isAdmin && event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);
            QChar keyChar;
            // Handle the key press event
            if (!keyEvent->text().isEmpty()) {
               keyChar = keyEvent->text().at(0);
            }
            else {
                return QMainWindow::eventFilter(obj, event);
            }

            if (realPassword[passwordCount] == keyChar) {
                passwordCount++;
            }
            else {
                passwordCount = 0;
            }
            if (passwordCount == realPassword.length()) {
                isAdmin = true;
                QMessageBox::information(this ,"Administrator mode" , "Congratulations, you've successfully entered the administrator mode!");
                ui->changeWorkers->setVisible(true);
                passwordCount = 0;
            }

            return true; // Event handled
        }
    }

    // Call the base class implementation for other events
    return QMainWindow::eventFilter(obj, event);
}
//-----------------------------------------------------------------------------

void MainWindow::on_changeWorkers_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->ChangeWorkersPage);
}

void MainWindow::on_goBackToolButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->MainPage);
}

void MainWindow::on_deleteWorkerButton_clicked()
{
    for (int i = 0 ; i < workersList.size(); ++i) {
        if (workersList[i]->isChecked()) {
            workersMenu->removeAction(workersList[i]);
            workersList.removeAt(i);
        }
    }
}

//-----------------------------------------------------------------------------

void MainWindow::on_addWorkerButton_clicked()
{
    QString curr = ui->lineEdit->text();

    // Check if an action with the same name already exists
    bool actionExists = false;
    for (QAction* existingAction : workersList) {
        if (existingAction->text() == curr) {
            actionExists = true;
            break;
        }
    }

    if (!actionExists) {
        QAction* newAction = new QAction(curr);
        newAction->setCheckable(true);
        newAction->setChecked(false);
        workersList.append(newAction);
        workersMenu->addAction(newAction);
    } else {
        qDebug() << "Already in the list!";
    }
}

//-----------------------------------------------------------------------------

void MainWindow::saveWorkersToFile() {

    QString filePath = QCoreApplication::applicationDirPath() + "/data";

    QFile file(QCoreApplication::applicationDirPath() + "/data/workersData.txt");

    // Відкрити файл для читання
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate | QIODevice::Append)) {
        // Створити об'єкт QTextStream для зручного читання
        QTextStream out(&file);
        for (QAction * action : workersList) {
            out << '"' + action->text() + '"' + "\t";
        }
        file.close();
    }
    else {
        qDebug() << "Couldn't open file!\n";
    }
}

void MainWindow::on_actionsave_triggered()
{
    if (!ui->stackedWidget->currentIndex()) {
        saveTasksToFile();
    }
    else {
        saveWorkersToFile();
    }
}

//-----------------------------------------------------------------------------

void MainWindow::saveTasksToFile() {

    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
    dayOfMonth = now->tm_mday;
    month = now->tm_mon + 1;
    year = now->tm_year + 1900;

    QFile file(QCoreApplication::applicationDirPath() + "/data/ordinary/" +
               QString::number(year) + "/" + QString::number(month) + "/" + QString::number(year) + "." +
               QString::number(month) + "." + QString::number(dayOfMonth) + ".txt");

    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate | QIODeviceBase::Append)) {
        // Створити об'єкт QTextStream для зручного читання
        QTextStream out(&file);
        int i = 1 ;
        for (auto it : allTasks) {
            out << "Number: \"" + QString::number(i++) + "\" | Workers: ";
            for (auto workers : it->getWorkers()) {
                out << '"' << workers << "\" ";
            }
            out << "| Task type : \"" << it->getTask() << "\" | ";
            out << "Beginning time : \"" << it->getBeginningTime().hour() << ":" << it->getBeginningTime().minute() << "\" | ";
            out << "Ending time : \"" << it->getEndTime().hour() << ":" << it->getEndTime().minute() << "\"\n";
        }
        file.close();
    }
    else {
        qDebug() << "Couldn't open file!\n";
    }

}

//-----------------------------------------------------------------------------

void MainWindow::removeTask(unsigned int row) {
    if (row >= allTasks.size()) {
        QMessageBox::critical(this , "Error" , "Can't remove non existing row");
        return;
    }
    for (int i = row + 1 ; i < allTasks.size() ; ++i) {
        ui->currentTable->setItem(i , 0 , new QTableWidgetItem(QString::number(i)));
    }
    ui->currentTable->removeRow(row);
    allTasks.remove(row);
    beginningTimeEditList.remove(row);
    endTimeEditList.remove(row);
    workersLabels.remove(row);
    priceLabels.remove(row);
   // workersMenus.remove(row);
}

void MainWindow::addTask(bool createNew , taskData& existingTaskData) {
    ui->currentTable->setRowCount(allTasks.size() + 1);

    ui->currentTable->setRowHeight(allTasks.size(), 40);

    // Create a QPushButton and QTimeEdit using dynamic allocation
    QPushButton* beginningTimeButton = new QPushButton(this);
    beginningTimeButton->setText("Current");
    QTimeEdit* beginningTimeEdit = new QTimeEdit(this);
    beginningTimeEditList.push_back(beginningTimeEdit);
    beginningTimeEdit->setDisplayFormat("hh:mm");  // Set the display format
    if (!createNew) {
        beginningTimeEdit->setTime(existingTaskData.getBeginningTime());
    }

    // Create a QHBoxLayout and set the QTimeEdit and QPushButton as its widgets
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(beginningTimeEdit);
    layout->addWidget(beginningTimeButton);

    // Create a container widget to hold the layout
    QWidget* beginningTimeContainer = new QWidget(this);
    beginningTimeContainer->setLayout(layout);

    // Create QTimeEdit for endTime
    QTimeEdit* endTimeEdit = new QTimeEdit(this);
    endTimeEditList.push_back(endTimeEdit);
    endTimeEdit->setDisplayFormat("hh:mm");  // Set the display format
    if (!createNew) {
        endTimeEdit->setTime(existingTaskData.getEndTime());
    }

    // Create a QHBoxLayout for endTime and set the QTimeEdit as its widget
    QHBoxLayout* endTimeLayout = new QHBoxLayout;
    QPushButton* endTimeButton = new QPushButton;
    endTimeButton->setText("Current");
    endTimeLayout->addWidget(endTimeEdit);
    endTimeLayout->addWidget(endTimeButton);

    // Create a container widget for endTime
    QWidget* endTimeContainer = new QWidget(this);
    endTimeContainer->setLayout(endTimeLayout);

    QLabel* priceLabel = new QLabel;

    priceLabels.push_back(priceLabel);

    QComboBox* tasksComboBox = new QComboBox;
    tasksComboBox->addItem("...");
    tasksComboBox->addItem("Body");
    tasksComboBox->addItem("Body and salon");
    tasksComboBox->addItem("Dry cleaning");
    if (!createNew) {
        switch(existingTaskData.getTask()) {
        case body :
            tasksComboBox->setCurrentIndex(1);
            priceLabel->setText(QString::number(BODY));
            ui->currentTable->setCellWidget(allTasks.size(), 3, priceLabel);
            break;
        case bodyAndSalon :
        tasksComboBox->setCurrentIndex(2);
            priceLabel->setText(QString::number(BODY_AND_SALON));
            ui->currentTable->setCellWidget(allTasks.size(), 3, priceLabel);
        break;
        case dryCleaning :
        tasksComboBox->setCurrentIndex(3);
        priceLabel->setText(QString::number(DRY_CLEANING));
        ui->currentTable->setCellWidget(allTasks.size(), 3, priceLabel);
        break;
        default:
        tasksComboBox->setCurrentIndex(0);
    }
    }

    QStackedLayout* tasksWidget = new QStackedLayout;
    tasksWidget->addWidget(tasksComboBox);
    QWidget* tasksContainer = new QWidget;
    tasksContainer->setLayout(tasksWidget);


    QMenu* individualMenu = new QMenu;

    for (auto it : workersList) {
        QAction* newAction = new QAction(it->text());
        newAction->setCheckable(true);
        newAction->setChecked(false);
        individualMenu->addAction(newAction);

        connect(newAction, &QAction::triggered, this, [=]() {
            int rowIndex = ui->currentTable->indexAt(beginningTimeButton->mapTo(ui->currentTable, QPoint(0, 0))).row();
            QList<QString> currentWorkers;
            if (newAction->isChecked() == false) {
                workersLabels[rowIndex]->clear();
                for (auto it2 : individualMenu->actions()) {
                    if (it2->isChecked()) {
                        workersLabels[rowIndex]->setText(workersLabels[rowIndex]->text() + " , " + it2->text());
                        currentWorkers.push_back(it2->text());
                    }
                }
            }
            else {
                if (workersLabels[rowIndex]->text().isEmpty()) {
                    workersLabels[rowIndex]->setText(workersLabels[rowIndex]->text() + newAction->text());
                }
                else {
                    workersLabels[rowIndex]->setText(workersLabels[rowIndex]->text() + " , "  + newAction->text());
                }
                currentWorkers = allTasks[rowIndex]->getWorkers();
                currentWorkers.push_back(newAction->text());
            }
            allTasks[rowIndex]->setWorkers(currentWorkers);
        });
    }

    QLabel * workersLabel = new QLabel;
    workersLabels.push_back(workersLabel);
    QToolButton * workersList = new QToolButton;
    workersList->setMenu(individualMenu);
    QHBoxLayout * workersListLayout = new QHBoxLayout;
    workersListLayout->addWidget(workersLabel);
    workersListLayout->addWidget(workersList);
    QWidget* workersListContainer = new QWidget;
    workersListContainer->setLayout(workersListLayout);
    workersList->setPopupMode(QToolButton::MenuButtonPopup); // QToolButton::InstantPopup);
    if (!createNew) {
        for (auto it : existingTaskData.getWorkers()) {
            workersLabel->setText(workersLabel->text() + it);
        }
    }

    QPushButton* removeButton = new QPushButton;
    QStackedLayout* removeButtonLayout = new QStackedLayout;
    removeButtonLayout->addWidget(removeButton);
    QWidget * removeButtonContainer = new QWidget;
    removeButtonContainer->setLayout(removeButtonLayout);

    connect(removeButton, &QPushButton::pressed, this, [=]() {
        int rowIndex = ui->currentTable->indexAt(beginningTimeButton->mapTo(ui->currentTable, QPoint(0, 0))).row();
        removeTask(rowIndex);
    });


    taskData* newTask;

    if (!createNew) {
        newTask = new taskData(existingTaskData);
    }
    else {
        newTask = new taskData;
    }

    allTasks.push_back(newTask);

    connect(tasksComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [=](int index) {
                int rowIndex = ui->currentTable->indexAt(tasksComboBox->mapTo(ui->currentTable, QPoint(0 , 0))).row();
                qDebug() << index << "\t" << rowIndex;
                switch(index) {
                case 1 :
                    allTasks[rowIndex]->setTask(body);
                    priceLabels[rowIndex]->setText(QString::number(BODY));
                    break;
                case 2 :
                    priceLabels[rowIndex]->setText(QString::number(BODY_AND_SALON));
                    allTasks[rowIndex]->setTask(bodyAndSalon);
                    break;
                case 3 :
                    allTasks[rowIndex]->setTask(dryCleaning);
                    priceLabels[rowIndex]->setText(QString::number(DRY_CLEANING));
                    break;
                default :
                    qDebug() << "Default";
                }
            });

    connect(beginningTimeButton, &QPushButton::clicked, this, [=]() {
        int rowIndex = ui->currentTable->indexAt(beginningTimeButton->mapTo(ui->currentTable, QPoint(0, 0))).row();

        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);
        QTime currTime;
        currTime.setHMS(now->tm_hour, now->tm_min , 0 , 0);
        beginningTimeEditList[rowIndex]->setTime(currTime);
    });

    // Connect signals for the QPushButton in the end time column
    connect(endTimeButton, &QPushButton::clicked, this, [=]() {
        int rowIndex = ui->currentTable->indexAt(beginningTimeButton->mapTo(ui->currentTable, QPoint(0, 0))).row();

        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);
        QTime currTime;
        currTime.setHMS(now->tm_hour, now->tm_min , 0 , 0);
        endTimeEditList[rowIndex]->setTime(currTime);
    });

    connect(beginningTimeEdit, &QTimeEdit::timeChanged, this, [=](const QTime& newTime) {

        int rowIndex = ui->currentTable->indexAt(tasksComboBox->mapTo(ui->currentTable, QPoint(0 , 0))).row();
        allTasks[rowIndex]->setBeginningTime(beginningTimeEdit->time());
        qDebug() << "Time changed to: " << newTime.toString();
    });

    connect(endTimeEdit, &QTimeEdit::timeChanged, this, [=](const QTime& newTime) {

        int rowIndex = ui->currentTable->indexAt(tasksComboBox->mapTo(ui->currentTable, QPoint(0 , 0))).row();
        allTasks[rowIndex]->setEndTime(endTimeEdit->time());
        qDebug() << "Time changed to: " << newTime.toString();
    });

    connect(individualMenu, &QMenu::aboutToHide, [=]() {
        // Your code here (optional)
        // To prevent the menu from closing, call ignore() on it
    });

    // Insert the container widgets into the QTableWidget
    ui->currentTable->setItem(allTasks.size() - 1, 0, new QTableWidgetItem(QString::number(allTasks.size())));
    ui->currentTable->setCellWidget(allTasks.size() - 1 , 1 , workersListContainer);
    ui->currentTable->setCellWidget(allTasks.size() - 1 , 2 , tasksContainer);
    ui->currentTable->setCellWidget(allTasks.size() - 1 , 3 , priceLabel);
    ui->currentTable->setCellWidget(allTasks.size() - 1, 4, beginningTimeContainer);
    ui->currentTable->setCellWidget(allTasks.size() - 1, 5, endTimeContainer);
    ui->currentTable->setCellWidget(allTasks.size() - 1 , 6 , removeButtonContainer);
    ui->currentTable->setColumnWidth(4 , 150);
    ui->currentTable->setColumnWidth(5 , 150);
}

void MainWindow::on_seeTheReportButton_clicked()
{
    chooseReportToShow * ptr = new chooseReportToShow;
    ptr->show();
}

