#ifndef TASKTYPE_H
#define TASKTYPE_H
#include <string>


enum taskName {
    body = 1,
    bodyAndSalon,
    dryCleaning
};

struct taskType {
public :
    taskType(taskName newTask) {
        changeTask(newTask);
    }
    void changeTask(taskName newTask) {
            switch(newTask) {
            case body:
                price = 250;
                break;
            case bodyAndSalon :
                price = 350;
                break;
            default :
                price = 1800;
        }
    }
    const int getPrice () const noexcept { return price ; }
    int price;
};



#endif // TASKTYPE_H
