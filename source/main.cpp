#include "EasyGrblSetup.h"
#include <QApplication>
#include <QLibraryInfo>
#include <QDir>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString locale = QLocale::system().name().section('_', 0, 0);
    // Translation for 'Qt'
    QTranslator qtTranslator;
    if (qtTranslator.load("qt_" + locale,
                          QLibraryInfo::location(QLibraryInfo::TranslationsPath)) )
        app.installTranslator(&qtTranslator);

    // Translation for 'EasyGrblSetup'
    QTranslator myTranslator;
    if (myTranslator.load("EasyGrblSetup_" + locale,
                          QDir::currentPath() + "/locale"))
        app.installTranslator(&myTranslator);

    EasyGrblSetup w;
    w.show();

    return app.exec();
}
