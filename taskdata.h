#ifndef TASKDATA_H
#define TASKDATA_H
#include "taskType.h"
#include <list>
#include <chrono>
#include <vector>
#include <algorithm>
#include <QString>
#include <QTime>

class taskData
{
public:
    taskData(int& number , QList<QString>& workers , taskName type , QTime& beginningTime , QTime& endTime) :
        number(number) , workers(workers) , type(type) , beginningTime(beginningTime) , endTime(endTime) {} ;
    taskData() { }
    // Setters
    void setTask(taskName type ) { this->type = type ; }
    void setWorkers(QList<QString>& workers ) { this->workers = workers ;}
    void setBeginningTime(const QTime& time ) { beginningTime = time ; }
    void setEndTime(const QTime& time) { endTime = time ; }
    void setNumber(const int& num) { number = num; }

    //Getters
    QList<QString> getWorkers() const noexcept { return workers ; }
    const QTime getBeginningTime() const noexcept { return beginningTime; }
    const QTime getEndTime() const noexcept { return endTime ; }
    taskName getTask() const noexcept { return type ; }

private :
    int number;
    QList<QString> workers;
    taskName type;
    QTime beginningTime;
    QTime endTime;
};

#endif // TASKDATA_H
