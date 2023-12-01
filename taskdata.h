#ifndef TASKDATA_H
#define TASKDATA_H
#include "taskType.h"
#include <list>
#include <chrono>

typedef std::chrono::time_point<std::chrono::system_clock> TIME;

class taskData
{
public:
    taskData(std::list<std::string>& workers , taskType& type , std::pair<int, int> beginningTime , std::pair<int,int> endTime);

    std::list<std::string> getWorkers() const noexcept { return workers ; }

    const std::pair<int,int> getBeginningTime() const noexcept { return beginningTime; }

    const std::pair<int,int> getEndTime() const noexcept { return endTime ; }

    taskType getTask() const noexcept { return type ; }

private :
    std::list<std::string> workers;
    taskType type;
    std::pair<int,int> beginningTime;
    std::pair<int,int> endTime;
};

#endif // TASKDATA_H
