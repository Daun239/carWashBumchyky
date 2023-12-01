#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "taskdata.h"
#include "taskdatamenu.h"

std::vector<taskData> allTasks;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->currentTable->setContextMenuPolicy(Qt::CustomContextMenu);//Додаємо контекстне меню
    connect(ui->currentTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);//Теж для контекстного меню
}

void MainWindow::editTaskTriggered() {
    if (row >= 0) {
        taskDataMenu* menu = new taskDataMenu(ui->currentTable, row, this);
        menu->show();
    }
}

void MainWindow::finishTaskTriggered() {
    if (row >= 0) {
        auto now = std::chrono::system_clock::now();
        std::chrono::hours offset(0);
        auto adjustedTime = now + offset;
        std::time_t adjustedTimeT = std::chrono::system_clock::to_time_t(adjustedTime);
        std::tm *adjustedTM = std::localtime(&adjustedTimeT);
        QString strEndTime;
        if (adjustedTM->tm_hour < 10) {
            strEndTime+= "0";
        }
        strEndTime += QString::number(adjustedTM->tm_hour) + ":";
        if (adjustedTM->tm_min < 10) {
            strEndTime += "0";
        }
        strEndTime += QString::number(adjustedTM->tm_min);
        ui->currentTable->setItem(row , 5 , new QTableWidgetItem(strEndTime));
    }
}
//Контекстне меню для додавання і завершення замовлення
void MainWindow::showContextMenu(const QPoint &pos) {
    QTableWidget *tableWidget = qobject_cast<QTableWidget*>(sender());
    if (!tableWidget)
        return;
    row = tableWidget->rowAt(pos.y()); // Запам'ятовуємо індекс рядка
    if (row >= 0) {
        QMenu contextMenu;
        QAction editTask("Edit", this);
        QAction finishTask("Finish", this);
        connect(&editTask, &QAction::triggered, this, &MainWindow::editTaskTriggered);
        connect(&finishTask, &QAction::triggered, this, &MainWindow::finishTaskTriggered);
        contextMenu.addAction(&editTask);
        contextMenu.addAction(&finishTask);
        contextMenu.exec(tableWidget->viewport()->mapToGlobal(pos));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_addTaskButton_clicked()
{
    taskDataMenu* menu = new taskDataMenu(ui->currentTable, allTasks.size(), this);
    menu->show();
}

