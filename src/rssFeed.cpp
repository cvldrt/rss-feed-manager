#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <utility>
#include "../include/rssFeed.h"

RssFeed::RssFeed(QUrl url) : url(std::move(url)) {}

RssFeed::RssFeed(std::unordered_set<QString> labels, int pollInterval) :
    pollInterval(pollInterval),
    labels(labels)
{}

RssFeed::RssFeed(QUrl url,
                 int pollInterval,
                 std::unordered_set<QString> labels,
                 QDateTime lastSync,
                 int replyCode)
    : url(url),
      pollInterval(pollInterval),
      labels(labels),
      lastSync(lastSync),
      replyCode(replyCode) {}

QJsonObject RssFeed::toJson() const {
    QJsonArray feedJsonArray;

    for (const auto &label : labels) {
        feedJsonArray.push_back(label);
    }

    QJsonObject feedState;

    feedState.insert("url", url.url());
    feedState.insert("interval", pollInterval);
    feedState.insert("labels", feedJsonArray);
    feedState.insert("lastSync", lastSync.toString());
    feedState.insert("code", replyCode);

    return feedState;
}
