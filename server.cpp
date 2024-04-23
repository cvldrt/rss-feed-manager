#include <QCoreApplication>
#include "include/executor.h"

#define DEFAULT_FEED_FOLDER "./.feeds/"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Executor ex(argc > 1 ? argv[1] : DEFAULT_FEED_FOLDER);
    ex.serve();

    QObject::connect(&ex, &Executor::stopped, &a, &QCoreApplication::quit);

    return a.exec();
}
