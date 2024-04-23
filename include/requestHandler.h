#ifndef CLIENTREPL_H
#define CLIENTREPL_H

#include <QObject>
#include <QNetworkAccessManager>

class RequestHandler : public QObject {
    Q_OBJECT
    QNetworkAccessManager *networkAccessManager;
    QString imageFolder;
    std::vector<QJsonObject> feedStates;

 public:
    RequestHandler(const QString&);
    ~RequestHandler();
    void refreshFeedStates();
    std::vector<QString> getFormattedFeedStates();
    void addFeed(const QString&);
    void deleteFeed(int);
    void updateFeed(std::tuple<QString, QString, int> data);
    QString downloadImg(const QUrl&);
    QByteArray getFeed(int);
    std::vector<QString> formatJsons(std::vector<QJsonObject>& arr);
    const std::vector<QJsonObject> &getFeedStates() const;

 signals:
    void finished();
};
    QByteArray getFeed(int i);

#endif // CLIENTREPL_H
