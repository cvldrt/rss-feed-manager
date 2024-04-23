#ifndef MYSERVER_H
#define MYSERVER_H

#include <QObject>
#include "rssFeed.h"
#include "rssFeedManager.h"
#include <QTcpServer>
#include <QHttpServer>
#include <filesystem>

class MyServer : public QObject {
 Q_OBJECT

    QTcpServer *tcpServer;
    QHttpServer *httpServer;
    QString address;
    qintptr port;
    RssFeedManager *rssFeedManager;

 public:
    MyServer(qintptr portNumber,
             QString address,
             const QString& feedFolderPath,
             QObject *parent = nullptr);
    ~MyServer();
    static  QHttpServerResponse  validate(const QHttpServerRequest &,
               const QHttpServerRequest::Method &,
               const std::vector<QString> &);
    QHttpServerResponse addFeed(const QHttpServerRequest &);
    QHttpServerResponse deleteFeed(const QHttpServerRequest &);
    QHttpServerResponse updateFeed(const QHttpServerRequest &);
    QHttpServerResponse getFeedStates(const QHttpServerRequest &, QHttpServerResponder &);
    QHttpServerResponse getFeed(const QHttpServerRequest &, QHttpServerResponder &);

 public slots:
    void startListening();
};

#endif // MYSERVER_H
