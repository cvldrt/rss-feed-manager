#ifndef HTTPSERVER_RSSFEEDMANAGER_H
#define HTTPSERVER_RSSFEEDMANAGER_H

#include <vector>
#include "rssFeed.h"
#include <QFile>

class RssFeedManager : public QObject {
 Q_OBJECT

    QNetworkAccessManager *networkAccessManager;
    std::filesystem::path path;
    std::vector<RssFeed> feeds;

 public:
    RssFeedManager(const QString&);
    ~RssFeedManager();
    void loadFeeds();
    QJsonObject getFeedStates() const;
    QByteArray getFeed(const QUrl&);
    void addFeed(const QUrl&);
    void deleteFeed(const QUrl&);
    void updateFeed(const QString&, std::unordered_set<QString>,int);
    void pollFeed(RssFeed&);

 public slots:
    void pollFeeds();
    void saveReplyContent(QNetworkReply*);
    void saveFeedState(const RssFeed&);
};

#endif //HTTPSERVER_RSSFEEDMANAGER_H
