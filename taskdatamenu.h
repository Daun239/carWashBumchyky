/*
 * #ifndef TASKDATAMENU_H
#define TASKDATAMENU_H

#include <QDialog>
#include "taskdata.h"
#include <QTableWidget>
#include <unordered_map>

namespace Ui {
class taskDataMenu;
}

class taskDataMenu : public QDialog
{
    Q_OBJECT

public:
    explicit taskDataMenu(QTableWidget* table , int row , QWidget *parent = nullptr);
    ~taskDataMenu();

private slots:
     void on_addTaskButton_clicked();

    void on_addWorkerButton_clicked();

    void on_removeWorkerButton_clicked();

private:
    Ui::taskDataMenu *ui;
    QTableWidget* table;
    int row;
    std::unordered_map<std::string , int> uMap;
    QList<QString> currWorkers;
    std::pair<int,int> beginningTime;
    std::pair<int,int> endTime;
    taskType* task;
};

#endif // TASKDATAMENU_H
*/
