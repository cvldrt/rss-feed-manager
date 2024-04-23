#include "../include/executor.h"

Executor::Executor(const QString& path, QObject *parent)
    : QObject{parent}
{
    worker = new QThread;
    server = new MyServer(8085, "127.0.0.1", path);
    server->moveToThread(worker);
    connect(this,  &Executor::startListening, server, &MyServer::startListening);
    worker->start();
}

Executor::~Executor()
{
    delete server;
    delete worker;
}

void Executor::serve()
{
    emit startListening();
}
