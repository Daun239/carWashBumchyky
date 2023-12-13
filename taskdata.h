#ifndef TASKDATA_H
#define TASKDATA_H
#include "taskType.h"
#include <list>
#include <chrono>
#include <vector>
#include <algorithm>
#include <QString>
#include <QTime>

#define BODY 250
#define BODY_AND_SALON 350
#define DRY_CLEANING 1800

class taskData
{
public:
    taskData(int& number , QList<QString>& workers , taskName type , QTime& beginningTime , QTime& endTime) :
        number(number) , workers(workers) , type(type) , beginningTime(beginningTime) , endTime(endTime) {} ;
    taskData() { }
    taskData(taskData& other) { number = other.number ; workers = other.workers ; type = other.type ; beginningTime = other.beginningTime; endTime = other.endTime ; }
    std::vector<int> indeces;
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
    int getNumber() const noexcept { return number ; }
    const QTime getDuration() const noexcept {
        QDateTime startDateTime(QDate(2000, 1, 1), beginningTime);
        QDateTime endDateTime(QDate(2000, 1, 1), endTime);
        qint64 seconds = startDateTime.secsTo(endDateTime);
        return QTime(0, 0).addSecs(seconds);
    }
    int getPrice() const noexcept  {
        switch(type) {
        case body :
            return BODY;
            break;
        case bodyAndSalon :
            return BODY_AND_SALON;
            break;
        case dryCleaning:
            return DRY_CLEANING;
            break;
        default :
            return -1;
        }
        return -1;
    }

private :
    int number;
    QList<QString> workers;
    taskName type;
    QTime beginningTime;
    QTime endTime;
    QTime taskDuration;
};



#endif // TASKDATA_H
