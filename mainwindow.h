#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addTaskButton_clicked();
    //Контекстне меню
    void editTaskTriggered();
    void finishTaskTriggered();
    void showContextMenu(const QPoint &pos);

private:
    Ui::MainWindow *ui;
    int row;
};
#endif // MAINWINDOW_H
