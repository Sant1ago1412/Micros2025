#include "microscom.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MicrosCom w;
    w.show();
    return a.exec();
}
