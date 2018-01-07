#include "EasyGrblSetup.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication  a(argc, argv);
    EasyGrblSetup w;
    w.show();

    return a.exec();
}
