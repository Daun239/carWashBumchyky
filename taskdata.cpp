#include "taskdata.h"
//#include <vector>

//taskData::taskData()

taskData::taskData(std::list<std::string>& workers, taskType& type, std::pair<int, int> beginningTime , std::pair<int,int> endTime)
    : workers(workers), beginningTime(beginningTime), endTime(endTime), type(type) {
    // Constructor body if needed
}
