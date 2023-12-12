#include "mainwindow.h"

#include <QApplication>
#include <QToolButton>
#include <QMenu>
#include <QToolBar>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
