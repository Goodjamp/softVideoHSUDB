#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

typedef uint8_t* t_P;

t_P as;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    qRegisterMetaType< QVector<uint8_t> >();

    return a.exec();
}
