#include "taskdatamenu.h"
#include "ui_taskdatamenu.h"
#include <QMessageBox>


extern std::vector<taskData> allTasks;

taskDataMenu::taskDataMenu(QTableWidget* table , int row , QWidget *parent) :
    QDialog(parent),
    ui(new Ui::taskDataMenu) ,
    table(table),
    row(row)
{
    ui->setupUi(this);

    //Time combo box
    ui->timeComboBox->addItem("Select time period");
    ui->timeComboBox->addItem("Set beginning time as now");
    ui->timeComboBox->clearFocus();
    ui->timeComboBox->setCurrentIndex(-1);

    //Workers combo box
    //Object with workers names
    WorkersList stuff;
    stuff.addName("Sanya");
    stuff.addName("Roma");
    stuff.addName("Sviat");
    stuff.removeName("Sviat");
    for(int i = 0; i < stuff.getSize();++i){
        ui->workersComboBox->addItem(QString::fromStdString(stuff.getNameByIndex(i)));
    }
    ui->workersComboBox->setCurrentIndex(-1);

    //Task combo box
    ui->taskTypeComboBox->addItem("Carwash");
    ui->taskTypeComboBox->setCurrentIndex(-1);
}

taskDataMenu::~taskDataMenu()
{
    delete ui;
}

void taskDataMenu::on_addTaskButton_clicked()
{

    if (currWorkers.empty()) {
        QMessageBox::warning(this, "Wrong input", "You have to choose at least one worker");
        return;
    }

    beginningTime.first = ui->beginHoursSpinBox->value();
    beginningTime.second = ui->beginMinutesSpinBox->value();

    endTime.first = ui->endHoursSpinBox->value();
    endTime.second = ui->endMinutesSpinBox->value();

    taskName curName;
    switch(ui->taskTypeComboBox->currentIndex()) {
    case 0 :
        curName = body;
        break;
    case 1 :
        curName = bodyAndSalon;
        break;
    case2:
        curName = dryCleaning;
        break;
    }

    task = new taskType(curName);

    taskData current(currWorkers , *task ,  beginningTime , endTime);

    allTasks.push_back(current);

    //-------------------------
    std::string workersList;
    for (auto str : currWorkers) {
        workersList += str + " ,";
    }
    workersList.pop_back();
    //---------------------------------
    std::string strTask;
    if (this->task->getPrice() == 250) {
        strTask = "Body";
    }
    else if (this->task->getPrice() == 350) {
        strTask = "Body and salon";
    }
    else {
        strTask = "Dry cleaning";
    }

    QString strBeginTime;
    if (beginningTime.first < 10) {
        strBeginTime+= "0";
    }
    strBeginTime += QString::number(beginningTime.first) + ":";

    if (beginningTime.second < 10) {
        strBeginTime+= "0";
    }
    strBeginTime += QString::number(beginningTime.second);


    QString strEndTime;
    if (endTime.first < 10) {
        strEndTime+= "0";
    }
    strEndTime += QString::number(endTime.first) + ":";

    if (endTime.second < 10) {
        strEndTime += "0";
    }
    strEndTime += QString::number(endTime.second);



    table->setRowCount(allTasks.size());
    table->setItem(allTasks.size() - 1 , 0 , new QTableWidgetItem(QString::number(allTasks.size())));
    table->setItem(allTasks.size() - 1 , 1 , new QTableWidgetItem(QString::fromStdString(workersList)));
    table->setItem(allTasks.size() - 1 , 2 , new QTableWidgetItem(QString::fromStdString(strTask)));
    table->setItem(allTasks.size() - 1 , 3 , new QTableWidgetItem(QString::number(this->task->getPrice())));
    table->setItem(allTasks.size() - 1 , 4 , new QTableWidgetItem(strBeginTime));
    table->setItem(allTasks.size() - 1 , 5 , new QTableWidgetItem(strEndTime));
}


void taskDataMenu::on_addWorkerButton_clicked()
{
    if (ui->timeComboBox->currentIndex() == -1) {
        QMessageBox::warning(this , "Option not chosen", "Choose one of the options");
        return;
    }

    std::string current = ui->workersComboBox->currentText().toStdString();
    if (uMap.find(current) != uMap.end()) {
        QMessageBox::warning(this, "Error" , "Worker is already added");
        return ;
    }
    uMap[current] = currWorkers.size();
    currWorkers.push_back(current);
    ui->workersLabel->setText(ui->workersLabel->text() + " " + QString::fromStdString(current));
}


void taskDataMenu::on_removeWorkerButton_clicked()
{
    if (ui->timeComboBox->currentIndex() == -1) {
        QMessageBox::warning(this , "Option not chosen", "Choose one of the options");
        return;
    }
    std::string current = ui->workersComboBox->currentText().toStdString();
    if (uMap.find(current) == uMap.end()) {
        QMessageBox::warning(this, "Error" , "There is no such worker added");
        return ;
    }
    auto it = currWorkers.begin();
    for (int i = 0 ; i < uMap[current]; ++i) {
        it++;
    }
    currWorkers.erase(it);
    uMap.erase(current);
}

