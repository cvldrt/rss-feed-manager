#include "../include/myserver.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QXmlStreamWriter>
#include <filesystem>
#include <utility>

MyServer::MyServer(qintptr portNumber, QString address, const QString& feedFolderPath, QObject *parent)
    :
    QObject(parent), address(std::move(address)), port(portNumber) {

    rssFeedManager = new RssFeedManager(feedFolderPath);

    tcpServer = new QTcpServer(this);
    httpServer = new QHttpServer(this);

    httpServer->route("/addFeed",
                      [this](const QHttpServerRequest &req) {
                        return addFeed(req);
                      });

    httpServer->route("/deleteFeed",
                      [this](const QHttpServerRequest &req) {
                        return deleteFeed(req);
                      });

    httpServer->route("/updateFeed",
                      [this](const QHttpServerRequest &req) {
                        return updateFeed(req);
                      });

    httpServer->route("/getFeedStates",
                      [this](const QHttpServerRequest &req,
                             QHttpServerResponder &&resp) {
                        getFeedStates(req,
                                      resp);
                      });

    httpServer->route("/getFeed",
                      [this](const QHttpServerRequest &req,
                             QHttpServerResponder &&resp) {
                        getFeed(req,
                                resp);
                      });

    httpServer->bind(tcpServer);
}

MyServer::~MyServer() {
    delete httpServer;
    delete tcpServer;
}


QHttpServerResponse buildErrorMessage(const QString &message,
                                      QHttpServerResponse::StatusCode status) {
    qDebug() << message;
    QJsonObject jsonObject = {{"error", message}, {"errorCode", (int) status}};
    QJsonDocument jsonDocument;
    jsonDocument.setObject(jsonObject);
    return QHttpServerResponse(jsonObject, status);
}

QHttpServerResponse MyServer::validate(const QHttpServerRequest &req,
                                       const QHttpServerRequest::Method &method,
                                       const std::vector<QString> &vals) {
    if (req.method() != method) {
        return buildErrorMessage("Invalid method",
                                 QHttpServerResponse::StatusCode::MethodNotAllowed);
    }

    if (vals.empty()) {
        return {QHttpServerResponse::StatusCode::Ok};
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(req.body(), &err);

    if (err.error != QJsonParseError::NoError) {
        return buildErrorMessage("Invalid input " + err.errorString(),
                                 QHttpServerResponse::StatusCode::BadRequest);
    }

    for (const auto &val : vals) {
        if (!doc.isObject() || !doc.object().contains(val)) {
            return buildErrorMessage("Invalid json document",
                                     QHttpServerResponse::StatusCode::BadRequest);
        }
    }

    return {QHttpServerResponse::StatusCode::Ok};
}

QHttpServerResponse MyServer::addFeed(const QHttpServerRequest &req) {
    auto response = validate(req, QHttpServerRequest::Method::Post, {"url"});
    if (response.statusCode() != QHttpServerResponse::StatusCode::Ok) return response;

    QJsonDocument doc = QJsonDocument::fromJson(req.body());
    rssFeedManager->addFeed(QUrl(doc["url"].toString()));
    return {"Feed with url: " + doc["url"].toString() + " added\n"};
}

QHttpServerResponse MyServer::deleteFeed(const QHttpServerRequest &req) {
    auto response = validate(req, QHttpServerRequest::Method::Post, {"url"});
    if (response.statusCode() != QHttpServerResponse::StatusCode::Ok) return response;

    QJsonDocument doc = QJsonDocument::fromJson(req.body());
    rssFeedManager->deleteFeed(QUrl(doc["url"].toString()));
    return {"Feed with url: " + doc["url"].toString() + " deleted\n"};
}

QHttpServerResponse MyServer::updateFeed(const QHttpServerRequest &req) {
    auto response = validate(req, QHttpServerRequest::Method::Post, {"url", "interval", "labels"});
    if (response.statusCode() != QHttpServerResponse::StatusCode::Ok) return response;

    QJsonDocument doc = QJsonDocument::fromJson(req.body());

    std::unordered_set<QString> labels;
    for (const auto obj: doc["labels"].toArray()) {
        labels.insert(obj.toString());
    }

    QString url = doc["url"].toString();
    int pollInterval = doc["interval"].toInt();

    rssFeedManager->updateFeed(url, labels, pollInterval);
    return {"Feed with url: " + doc["url"].toString() + " updated\n"};
}

QHttpServerResponse MyServer::getFeedStates(const QHttpServerRequest &req,
                             QHttpServerResponder &resp) {
    auto response = validate(req, QHttpServerRequest::Method::Get, {});
    if (response.statusCode() != QHttpServerResponse::StatusCode::Ok) return response;

    resp.write(QJsonDocument(rssFeedManager->getFeedStates()));

    return {QHttpServerResponse::StatusCode::Ok};
}

QHttpServerResponse MyServer::getFeed(const QHttpServerRequest &req,
                       QHttpServerResponder &resp) {

    auto response = validate(req, QHttpServerRequest::Method::Post, {"url"});
    if (response.statusCode() != QHttpServerResponse::StatusCode::Ok) return response;

    QJsonDocument doc = QJsonDocument::fromJson(req.body());
    QUrl url = QUrl(doc["url"].toString());

    resp.write(rssFeedManager->getFeed(url), {{"content-type", "application/xml"}});

    return {QHttpServerResponse::StatusCode::Ok};
}

void MyServer::startListening() {
    qDebug() << "Listening: " << tcpServer->listen(QHostAddress(address), port);
    qDebug() << "Error string: " << tcpServer->errorString();
}
