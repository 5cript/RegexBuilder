#include "regexeditor.h"

#include <QApplication>
#include <QFile>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    RegexEditor mainWindow;
    mainWindow.show();

    return app.exec();
}
