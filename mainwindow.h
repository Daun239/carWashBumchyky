#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QEvent>
#include <QKeyEvent>
#include "taskdata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected :
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void on_addTaskButton_clicked();
    void readFileData();
    bool createFolders();
    void setupUi();
    void on_changeWorkers_clicked();
    void on_goBackToolButton_clicked();
    void on_deleteWorkerButton_clicked();
    void on_addWorkerButton_clicked();
    void saveWorkersToFile();
    void saveTasksToFile();
    void on_actionsave_triggered();
    void removeTask(unsigned int row);

    void addTask(bool createNew , taskData& existingTaskData);

private:
    Ui::MainWindow *ui;
    int row;
    QString fileName;
    QMenu* menu;
    QAction* loh;

};
#endif // MAINWINDOW_H
