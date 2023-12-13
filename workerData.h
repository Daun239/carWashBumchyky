#ifndef WORKERDATA_H
#define WORKERDATA_H
#include "taskdata.h"

class workerData {
public :
    workerData() : tasksExecuted (0), moneyGained(0) , name ("") , timeToExecuteTask(0 , 0) { } ;
    int tasksExecuted;
    int moneyGained;
    QString name;
    QTime timeToExecuteTask;

private :

};

#endif // WORKERDATA_H
