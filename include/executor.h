#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QObject>
#include "myserver.h"

class Executor : public QObject
{
    Q_OBJECT

    MyServer *server;
    QThread *worker;
public:
    explicit Executor(const QString& ,QObject *parent = nullptr);

    ~Executor();

    void serve();
signals:
    void startListening();
    void stopped();
};

#endif // EXECUTOR_H
