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



//Global variables
bool isAdmin = false;
int passwordCount = 0;
int dayOfMonth, month, year;
int tasksCount;
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
    addTask(false , *ptr);
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
    fileName = QString("data/ordinary/%1.%2.%3.txt").arg(year)
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

            //Дізнаємось про працівників
            for (i = index ; i < line.length() && line[i] != '|' ; ) {

                while(i < line.length() && line[i++] != '"');

                while(i < line.length() && line[i] != '"') {
                    curr += line[i++];
                }
                if (curr.isEmpty()) {
                    qDebug() << "Can't add an empty worker";
                    return;
                }
                currWorkers.push_back(curr);
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

            // Дізнаємось про початок виконання завдання
            curr.clear();
            while(i < line.length() && line[i++] != '"');

            while(i < line.length() && line[i++] != ':') {
                curr+= line[i];
            }
            isConversionOk = false;
            int beginningHour = curr.toInt(&isConversionOk);
            if (!isConversionOk) {
                qDebug() << "Couldn't convert the hour of the beginning of the task";
                return;
            }
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
            curr.clear();
            while (i < line.length() && line[i] != '"') {
                curr+= line[i++];
            }
            isConversionOk = false;
            int endMinute = curr.toInt();
            if (!isConversionOk) {
                qDebug() << "Couldn't convert end minute";
                return;
            }
            currTaskEndTime.setHMS(endHour , endMinute , 0 , 0);

            currTask->setEndTime(currTaskEndTime);


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

      ui->currentTable->setColumnCount(6);

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
    if (ui->stackedWidget->currentIndex()) {
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
            out << "Number: " + QString::number(i) + " | Workers: ";
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
    if (row >= tasksCount) {
        QMessageBox::critical(this , "Error" , "Can't remove non existing row");
        return;
    }
    ui->currentTable->removeRow(row);
    allTasks.remove(row);
}

void MainWindow::addTask(bool createNew , taskData& existingTaskData) {
    ui->currentTable->setRowCount(++tasksCount);

    ui->currentTable->setRowHeight(tasksCount - 1, 40);

    // Create a QPushButton and QTimeEdit using dynamic allocation
    QPushButton* beginningTimeButton = new QPushButton(this);
    beginningTimeButton->setText("Current");
    QTimeEdit* beginningTimeEdit = new QTimeEdit(this);
    beginningTimeEditList.push_back(beginningTimeEdit);
    beginningTimeEdit->setDisplayFormat("hh:mm");  // Set the display format

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

    // Create a QHBoxLayout for endTime and set the QTimeEdit as its widget
    QHBoxLayout* endTimeLayout = new QHBoxLayout;
    QPushButton* endTimeButton = new QPushButton;
    endTimeButton->setText("Current");
    endTimeLayout->addWidget(endTimeEdit);
    endTimeLayout->addWidget(endTimeButton);

    // Create a container widget for endTime
    QWidget* endTimeContainer = new QWidget(this);
    endTimeContainer->setLayout(endTimeLayout);

    QComboBox* tasksComboBox = new QComboBox;
    tasksComboBox->addItem("...");
    tasksComboBox->addItem("Body");
    tasksComboBox->addItem("Body and salon");
    tasksComboBox->addItem("Dry cleaning");

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

    // Insert the container widgets into the QTableWidget
    ui->currentTable->setItem(tasksCount - 1, 0, new QTableWidgetItem(QString::number(tasksCount)));
    ui->currentTable->setCellWidget(tasksCount - 1 , 1 , workersListContainer);
    ui->currentTable->setCellWidget(tasksCount - 1 , 2 , tasksContainer);
    ui->currentTable->setCellWidget(tasksCount - 1, 4, beginningTimeContainer);
    ui->currentTable->setCellWidget(tasksCount - 1, 5, endTimeContainer);
    ui->currentTable->setColumnWidth(4 , 150);
    ui->currentTable->setColumnWidth(5 , 150);

    taskData* newTask = new taskData;

    allTasks.push_back(newTask);

    connect(tasksComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [=](int index) {
                int rowIndex = ui->currentTable->indexAt(tasksComboBox->mapTo(ui->currentTable, QPoint(0 , 0))).row();
                qDebug() << index << "\t" << rowIndex;
                switch(index) {
                case 1 :
                    allTasks[rowIndex]->setTask(body);
                    ui->currentTable->setItem(rowIndex , 3 , new QTableWidgetItem(QString::number(250)));
                    break;
                case 2 :
                    allTasks[rowIndex]->setTask(bodyAndSalon);
                    ui->currentTable->setItem(rowIndex , 3 , new QTableWidgetItem(QString::number(350)));
                    break;
                case 3 :
                    allTasks[rowIndex]->setTask(dryCleaning);
                    ui->currentTable->setItem(rowIndex , 3 , new QTableWidgetItem(QString::number(1800)));
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
}
