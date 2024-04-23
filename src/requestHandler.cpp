#include "../include/requestHandler.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <qeventloop.h>
#include <QJsonArray>
#include <unordered_set>
#include <QFile>
#include <QDir>


RequestHandler::RequestHandler(const QString& path): imageFolder(path) {
    networkAccessManager = new QNetworkAccessManager(this);

    if (!QDir(path).exists()){
        QDir().mkdir(path);
        qDebug() << "Created folder: " + path;
    }
}

RequestHandler::~RequestHandler() {
    delete networkAccessManager;
    QDir(imageFolder).removeRecursively();
}

const std::vector<QJsonObject> &RequestHandler::getFeedStates() const {
    return feedStates;
}

/**
 * Formats an array of QJsonObjects representing feeds to strings to be displayed in listWidget
 * @param arr vector of JsonObjects
 * @return vector of QStrings
 */
std::vector<QString> RequestHandler::formatJsons(std::vector<QJsonObject>& arr){
    std::vector<QString> res;

    for (const auto& fs: arr){

        QString url = fs.value("url").toString();
        QString pollInterval = QString::number(fs.value("interval").toInt());

        QJsonArray labelsArray = fs.value("labels").toArray();
        QString labels;
        for (auto && i : labelsArray)
            labels += i.toString() + ", ";

        QString lastSync = fs.value("lastSync").toString();
        QString replyCode = QString::number(fs.value("code").toInt());

        res.push_back("Url: " + url + '\n' +
                        "Labels: " + labels + '\n' +
                        "Polled: " + lastSync + " (" + replyCode + ")\n" +
                        "Poll interval: " + pollInterval);
    }

    return res;
}

std::vector<QString> RequestHandler::getFormattedFeedStates() {
    refreshFeedStates();
    return formatJsons(feedStates);
}

/**
 * Refreshes feed states saved in feedStates
 */
void RequestHandler::refreshFeedStates() {
    QNetworkRequest r(QUrl("http://localhost:8085/getFeedStates"));
    auto reply = networkAccessManager->get(r);

    QEventLoop loop;
    loop.connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), SLOT(quit()));
    loop.exec();

    auto doc = QJsonDocument::fromJson(reply->readAll());
    QJsonArray array = doc.object().value("states").toArray();

    feedStates.clear();
    for (const auto& obj: array) {
        feedStates.push_back(obj.toObject());
    }
}

/**
 * Sends request to the server to add feed with the given url
 * @param url
 */
void RequestHandler::addFeed(const QString& url){
    QJsonObject obj = {{"url", url}};
    QJsonDocument doc;
    doc.setObject(obj);

    QNetworkRequest r(QUrl("http://localhost:8085/addFeed"));
    r.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");

    auto reply = networkAccessManager->post(r, {doc.toJson()});
    connect(reply, &QNetworkReply::finished, this, &RequestHandler::finished);
}

/**
 * Sends a request to the server for a RSS feed .xml file
 * @param i index in feedStates
 * @return content of httpReply
 */
QByteArray RequestHandler::getFeed(int i) {
    QString url = feedStates[i].value("url").toString();

    QNetworkRequest r(QUrl("http://localhost:8085/getFeed"));
    r.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");

    auto obj =  QJsonObject();
    obj.insert("url", url);

    auto reply = networkAccessManager->post(r, QJsonDocument(obj).toJson());

    QEventLoop loop;
    loop.connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), SLOT(quit()));
    loop.exec();

    return reply->readAll();
}

/**
 * Sends request to the server to delete feed
 * @param i index in feedStates
 */
void RequestHandler::deleteFeed(int i) {
    QString url = feedStates[i].value("url").toString();

    QJsonObject obj = {{"url", url}};
    QJsonDocument doc;
    doc.setObject(obj);

    QNetworkRequest r(QUrl("http://localhost:8085/deleteFeed"));
    r.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");

    auto reply = networkAccessManager->post(r, {doc.toJson()});
    connect(reply, &QNetworkReply::finished, this, &RequestHandler::finished);
}

/**
 * Sends request to the server to change labels and poll interval of feed with the given url
 * @param tuple <url, labels, pollInterval>
 */
void RequestHandler::updateFeed(std::tuple<QString, QString, int> tuple) {
    auto [url, ls, pi] = tuple;


    QJsonArray labels;
    for (const auto& label: ls.split(",").toList()){
        labels.push_back(label);
    }

    QJsonObject obj = {{"url", url}, {"interval", pi}, {"labels", labels}};
    QJsonDocument doc;
    doc.setObject(obj);

    QNetworkRequest r(QUrl("http://localhost:8085/updateFeed"));
    r.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");

    auto reply = networkAccessManager->post(r, {doc.toJson()});
    connect(reply, &QNetworkReply::finished, this, &RequestHandler::finished);
}

/**
 * Downloads an image
 * @param url url from which to download file
 * @return path to the downloaded file
 */
QString RequestHandler::downloadImg(const QUrl &url) {
    if (url.isEmpty() || !url.isValid()) return {};

    std::filesystem::path saveFilePath = (imageFolder + url.fileName()).toStdString();
    if (exists(saveFilePath)){
        return QString(saveFilePath.string().c_str());
    }

    auto reply = networkAccessManager->get(QNetworkRequest(url));

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    auto img = QFile();
    img.setFileName(saveFilePath);
    img.open(QIODevice::WriteOnly);
    img.write(reply->readAll());
    img.close();

    qDebug() << "Created file: " << saveFilePath.filename();

    return QString(saveFilePath.string().c_str());
}

