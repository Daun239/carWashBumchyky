#include "taskdata.h"
//#include <vector>

//taskData::taskData()

taskData::taskData(std::list<std::string>& workers, taskType& type, std::pair<int, int> beginningTime , std::pair<int,int> endTime)
    : workers(workers), beginningTime(beginningTime), endTime(endTime), type(type) {
    // Constructor body if needed
}
void WorkersList::addName(const std::string& name) {
    workerNames.push_back(name);
}

void WorkersList::removeName(const std::string& name) {
    auto it = std::find(workerNames.begin(), workerNames.end(), name);
    if (it != workerNames.end()) {
        workerNames.erase(it);
    }
}
int WorkersList::getSize() {
    return workerNames.size();
}
std::string WorkersList::getNameByIndex(int index) {
    if (index < workerNames.size()) {
        return workerNames[index];
    }

    static std::string emptyString;
    return emptyString;
}
