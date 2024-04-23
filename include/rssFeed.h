#ifndef HW_RSS_RSSFEED_H
#define HW_RSS_RSSFEED_H

#define DEFAULT_POLL_INTERVAL 30

#include <QNetworkReply>
#include <QTime>
#include <QUrl>
#include <filesystem>
#include <unordered_set>

class RssFeed {
 public:
    QUrl url;
    int pollInterval = DEFAULT_POLL_INTERVAL;
    std::unordered_set<QString> labels;
    QDateTime lastSync;
    int replyCode;

    RssFeed(QUrl);
    RssFeed(std::unordered_set<QString>, int);
    RssFeed(QUrl, int, std::unordered_set<QString>, QDateTime, int);
    QJsonObject toJson() const;
};

#endif // HW_RSS_RSSFEED_H
