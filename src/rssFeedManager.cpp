#include <QFile>
#include <QDir>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <qeventloop.h>
#include <QJsonArray>
#include <QTimer>
#include <algorithm>
#include "../include/rssFeedManager.h"

#define POLL_INTERVAL 30

RssFeedManager::RssFeedManager(const QString& path) : path(path.toStdString()) {
    // I would like to initialize networkAccessManager here, but it doesn't work
    // networkAccessManager = new QNetworkAccessManager();

    if (!QDir(path).exists()){
        QDir().mkdir(path);
        qDebug() << "Created folder: " + path;
    } else {
        loadFeeds();
    }

    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &RssFeedManager::pollFeeds);
    timer->start(POLL_INTERVAL * 60 * 1000);
}

RssFeedManager::~RssFeedManager() {
    delete networkAccessManager;
}

QJsonDocument getJsonFromFile(const std::filesystem::path &path) {
    QFile qfile(path);
    qfile.open(QIODevice::ReadOnly | QIODevice::Text);
    auto contents = qfile.readAll();
    qfile.close();

    return QJsonDocument::fromJson(contents);
}

RssFeed getFeedFromJson(const QJsonDocument &document) {
    QString url = document.object().value("url").toString();
    int pollInterval = document.object().value("interval").toInt();

    QJsonArray labelsArray = document.object().value("labels").toArray();
    std::unordered_set<QString> labels;
    for (auto i = labelsArray.begin(); i != labelsArray.end(); i++)
        labels.insert(i->toString());

    QDateTime lastSync =
        QDateTime::fromString(document.object().value("lastSync").toString());
    int replyCode = document.object().value("code").toInt();

    return {url, pollInterval, labels,
            lastSync, replyCode};
}

void RssFeedManager::loadFeeds() {
    for (auto const &file : std::filesystem::directory_iterator{path}) {
        if (file.path().extension() == ".json") {
            auto doc = getJsonFromFile(file.path());
            feeds.push_back(getFeedFromJson(doc));
            qDebug() << "Loaded: " << file.path().c_str();
        }
    }
}

void RssFeedManager::addFeed(const QUrl &url) {
    RssFeed newFeed(url);
    pollFeed(newFeed);
    feeds.push_back(newFeed);
}

void RssFeedManager::deleteFeed(const QUrl &url) {
    auto deleted =
        std::erase_if(feeds, [&url](RssFeed &feed) { return feed.url == url; });

    if (deleted > 0) {
        if (QFile::remove(path.c_str() + url.host() + ".json"))
            qDebug() << "Deleted file: " << path.c_str() + url.host() + ".json";

        if (QFile::remove(path.c_str() + url.host() + ".xml"))
            qDebug() << "Deleted file: " << path.c_str() + url.host() + ".xml";
    }
}

void RssFeedManager::pollFeeds() {
    qDebug() << "Checking feeds";
    for (auto& feed: feeds){
        if (feed.lastSync.secsTo(QDateTime::currentDateTime()) > feed.pollInterval * 60){
            pollFeed(feed);
        }
    }
}

void RssFeedManager::pollFeed(RssFeed &feed) {
    networkAccessManager = new QNetworkAccessManager();

    connect(networkAccessManager, &QNetworkAccessManager::finished, this,
            &RssFeedManager::saveReplyContent);

    auto reply = networkAccessManager->get(QNetworkRequest(QUrl(feed.url)));

    // Wait for the download to finish ??
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    feed.replyCode =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    feed.lastSync = QDateTime::currentDateTime();

    saveFeedState(feed);
}

void RssFeedManager::saveFeedState(const RssFeed &feed) {
    QJsonDocument feedJson(feed.toJson());
    QFile jsonFile(QString(path.string().c_str()) + feed.url.host() + ".json");

    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(feedJson.toJson());

    qDebug() << "Created file: " + jsonFile.fileName();
}

void RssFeedManager::updateFeed(const QString& url, std::unordered_set<QString> ls,int pi) {
    RssFeed& feed = *std::find_if(feeds.begin(),
                              feeds.end(),
                              [&url](RssFeed &f) {
                                return f.url == url;
                              });

    feed.pollInterval = pi;
    feed.labels = ls;
    saveFeedState(feed);
}

void RssFeedManager::saveReplyContent(QNetworkReply *reply) {
    QFile rssFeedFile(QString(path.c_str()) + reply->url().host() + ".xml");

    if (!rssFeedFile.open(QIODevice::WriteOnly)) return;

    const QByteArray data = reply->readAll();
    rssFeedFile.write(data);
    rssFeedFile.close();

    qDebug() << "Created file: " + rssFeedFile.fileName();
}

QJsonObject RssFeedManager::getFeedStates() const {
    QJsonObject res;
    QJsonArray arr;

    for (auto& feed: feeds){
        arr.push_back(feed.toJson());
    }

    res.insert("states", arr);

    return res;
}
QByteArray RssFeedManager::getFeed(const QUrl &url) {
    QFile rssFeedFile(QString(path.c_str()) + url.host() + ".xml");

    if (!rssFeedFile.open(QIODevice::ReadOnly)) return {};

    return rssFeedFile.readAll();
}
