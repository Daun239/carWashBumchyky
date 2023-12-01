#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "taskdata.h"
#include "taskdatamenu.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::vector<taskData> allTasks;


void MainWindow::on_addTaskButton_clicked()
{
    taskDataMenu* menu = new taskDataMenu(ui->currentTable, allTasks.size(), this);
    menu->show();
}

