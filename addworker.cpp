#include "addworker.h"
#include "ui_addworker.h"

addWorker::addWorker(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addWorker)
{
    ui->setupUi(this);
}

addWorker::~addWorker()
{
    delete ui;
}
