#include <QApplication>
#include <QNetworkReply>
#include <QObject>
#include "include/mainWindow.h"

#define DEFAULT_IMG_FOLDER "./.imgs/"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w(argc > 1 ? argv[0] : DEFAULT_IMG_FOLDER);
    w.show();

    return app.exec();
}
