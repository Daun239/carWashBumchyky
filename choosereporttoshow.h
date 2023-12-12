#ifndef CHOOSEREPORTTOSHOW_H
#define CHOOSEREPORTTOSHOW_H

#include <QMainWindow>

namespace Ui {
class chooseReportToShow;
}

class chooseReportToShow : public QMainWindow
{
    Q_OBJECT

public:
    explicit chooseReportToShow(QWidget *parent = nullptr);
    ~chooseReportToShow();

private slots:
    void on_cancelSelection_clicked();

    void on_showReportButton_clicked();

    void addItemToTable();
    void printTable();

    void on_ascendingOrderButton_clicked();
    void reverseSwapOrder();
    void sortTasks();
    void insertWorkersIntoComboBox();
    void showIndividualWorkersInfo();

private:
    Ui::chooseReportToShow *ui;
};

#endif // CHOOSEREPORTTOSHOW_H
