#include "taskdatamenu.h"
#include "ui_taskdatamenu.h"
#include <QMessageBox>
#include <chrono>
#include <ctime>


/*
extern QList<QAction*> menuActions;
extern WorkersList workersList;
extern std::vector<taskData> allTasks;

 taskDataMenu::taskDataMenu(QTableWidget* table , int row , QWidget *parent) :
    QDialog(parent),
    ui(new Ui::taskDataMenu) ,
    table(table),
    row(row)
{
    ui->setupUi(this);
    ui->timeComboBox->addItem("Select time period");
    ui->timeComboBox->addItem("Set beginning time as now");
    ui->timeComboBox->clearFocus();


    //Workers combo box
    for (int i = 0 ; i < workersList.getSize(); ++i) {
        ui->workersComboBox->addItem(QString::fromStdString(workersList.getNameByIndex(i)));
    }

    //Task combo box
    ui->taskTypeComboBox->addItem("Body");
    ui->taskTypeComboBox->addItem("Body and salon");
    ui->taskTypeComboBox->addItem("Dry cleaning");
    ui->taskTypeComboBox->setCurrentIndex(0);
    if(row!=allTasks.size()){
        currWorkers=allTasks[row].getWorkers();
        int i=1;
        for(const auto& element:currWorkers){
            uMap[element]=i++;
        }
        ui->addTaskButton->setText("Apply changes");
        ui->addTaskButton->setText(currWorkers.front().c_str());
        std::string Workers=table->item(row,1)->text().toStdString();
        ui->workersLabel->setText(Workers.c_str());
         std::string FinishTime=table->item(row,5)->text().toStdString();
       /* if(FinishTime=="*"){
            ui->timeComboBox->setCurrentIndex(1);
            ui->beginHoursSpinBox->setValue(allTasks[row].getBeginningTime().first);
            ui->beginMinutesSpinBox->setValue(allTasks[row].getBeginningTime().second);
        }else{
            ui->timeComboBox->setCurrentIndex(0);
            ui->beginHoursSpinBox->setValue(allTasks[row].getBeginningTime().first);
            ui->beginMinutesSpinBox->setValue(allTasks[row].getBeginningTime().second);
            ui->endHoursSpinBox->setValue(allTasks[row].getEndTime().first);
            ui->endMinutesSpinBox->setValue(allTasks[row].getEndTime().second);
        }
        ui->workersComboBox->setCurrentIndex(-1);
    }
    //Time combo box
}

taskDataMenu::~taskDataMenu()
{
    delete ui;
}
/*

void taskDataMenu::on_addTaskButton_clicked()
{
    if(row==allTasks.size()){
        if (currWorkers.empty()) {
            QMessageBox::warning(this, "Wrong input", "You have to choose at least one worker");
            return;
        }

        if(ui->timeComboBox->currentIndex()==0){
            beginningTime.first = ui->beginHoursSpinBox->value();
            beginningTime.second = ui->beginMinutesSpinBox->value();
            endTime.first = ui->endHoursSpinBox->value();
            endTime.second = ui->endMinutesSpinBox->value();
        }else{
            auto now = std::chrono::system_clock::now();
            std::chrono::hours offset(0);
            auto adjustedTime = now + offset;
            std::time_t adjustedTimeT = std::chrono::system_clock::to_time_t(adjustedTime);
            std::tm *adjustedTM = std::localtime(&adjustedTimeT);
            beginningTime.first=static_cast<int>(adjustedTM->tm_hour);
            beginningTime.second=static_cast<int>(adjustedTM->tm_min);
            endTime.first = 0;
            endTime.second = 0;
        }

        taskName curName;
        switch(ui->taskTypeComboBox->currentIndex()) {
        case 0:
            curName = body;
            break;
        case 1:
            curName = bodyAndSalon;
            break;
        case 2:
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
        if(ui->timeComboBox->currentIndex()==1)table->setItem(allTasks.size()-1 , 5 , new QTableWidgetItem("*"));
        else table->setItem(allTasks.size()-1, 5 , new QTableWidgetItem(strEndTime));
    }else{
        if(ui->timeComboBox->currentIndex()==0){
            beginningTime.first = ui->beginHoursSpinBox->value();
            beginningTime.second = ui->beginMinutesSpinBox->value();
            endTime.first = ui->endHoursSpinBox->value();
            endTime.second = ui->endMinutesSpinBox->value();
        }else{
            auto now = std::chrono::system_clock::now();
            std::chrono::hours offset(0);
            auto adjustedTime = now + offset;
            std::time_t adjustedTimeT = std::chrono::system_clock::to_time_t(adjustedTime);
            std::tm *adjustedTM = std::localtime(&adjustedTimeT);
            beginningTime.first=static_cast<int>(adjustedTM->tm_hour);
            beginningTime.second=static_cast<int>(adjustedTM->tm_min);
            endTime.first = 0;
            endTime.second = 0;
        }
        taskName curName;
        switch(ui->taskTypeComboBox->currentIndex()) {
        case 0:
            curName = body;
            break;
        case 1:
            curName = bodyAndSalon;
            break;
        case 2:
            curName = dryCleaning;
            break;
        }

        task = new taskType(curName);

        taskData current(currWorkers , *task ,  beginningTime , endTime);

        allTasks[row]=current;

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
        table->setItem(row , 0 , new QTableWidgetItem(QString::number(row+1)));
        table->setItem(row , 1 , new QTableWidgetItem(QString::fromStdString(workersList)));
        table->setItem(row , 2 , new QTableWidgetItem(QString::fromStdString(strTask)));
        table->setItem(row , 3 , new QTableWidgetItem(QString::number(this->task->getPrice())));
        table->setItem(row , 4 , new QTableWidgetItem(strBeginTime));
        if(ui->timeComboBox->currentIndex()==1)table->setItem(row , 5 , new QTableWidgetItem("*"));
        else table->setItem(row, 5 , new QTableWidgetItem(strEndTime));
    }
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

*/
