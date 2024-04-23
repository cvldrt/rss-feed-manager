#include "../include/mainWindow.h"
#include "./ui_mainwindow.h"
#include <QDomDocument>

MainWindow::MainWindow(const QString &path, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      requestHandler(RequestHandler(path)) {
    ui->setupUi(this);

    connect(&requestHandler,
            &RequestHandler::finished,
            this,
            &MainWindow::refresh);

    ui->listWidget->setStyleSheet(
        "QListWidget::item { border-bottom: 1px solid gray; }");
    ui->textBrowser->setReadOnly(true);
    ui->textBrowser->setOpenExternalLinks(true);

    connect(ui->refreshButton,
            &QAbstractButton::clicked,
            this,
            &MainWindow::refresh);
    connect(ui->addButton,
            &QAbstractButton::clicked,
            this,
            &MainWindow::addFeed);
    connect(ui->editButton,
            &QAbstractButton::clicked,
            this,
            &MainWindow::editFeed);
    connect(ui->deleteButton,
            &QAbstractButton::clicked,
            this,
            &MainWindow::deleteFeed);
    connect(ui->searchButton,
            &QAbstractButton::clicked,
            this,
            &MainWindow::filterByLabel);
    connect(ui->listWidget,
            &QListWidget::itemClicked,
            this,
            &MainWindow::showFeed);

    refresh();
}

void printFeedItems(QTextCursor & cursor, QDomDocument & doc) {
    QDomNodeList items = doc.elementsByTagName("item");

    for (int i = 0; i < items.size(); i++) {
        QString title =
            items.at(i).toElement().elementsByTagName("title").at(0).toElement().text();
        QString link =
            items.at(i).toElement().elementsByTagName("link").at(0).toElement().text();
        QString description =
            items.at(i).toElement().elementsByTagName("description").at(0).toElement().text();
        //QString enclosure = doc.elementsByTagName("enclosure").at(i).toElement().text();
        QString author =
            items.at(i).toElement().elementsByTagName("author").at(0).toElement().text();
        QString pubDate =
            items.at(i).toElement().elementsByTagName("pubDate").at(0).toElement().text();

        cursor.insertHtml("\
        <!DOCTYPE html>\
        <html>\
        <body>\
            <br><br>\
            <h3>" + title + "</h3>\
            <a href=\"" + link + "\">[link]</a>\
            <h5>" + pubDate + "</h5>\
            <h6>" + author + "</h6>\
            <p>" + description + "<br></p>\
        </body>\
        </html>\
        ");
    }
}

void MainWindow::printFeedInfo(QTextCursor &cursor, QDomDocument &doc) {
    QString title = doc.elementsByTagName("title").at(0).toElement().text();
    QString link = doc.elementsByTagName("link").at(0).toElement().text();
    QString description =
        doc.elementsByTagName("description").at(0).toElement().text();
    QString pubDate = doc.elementsByTagName("pubDate").at(0).toElement().text();
    QString lastBuildDate =
        doc.elementsByTagName("lastBuildDate").at(0).toElement().text();
    QString imgUrl =
        doc.elementsByTagName("image").at(0).toElement().elementsByTagName("url").at(
            0).toElement().text();
    QString imgPath = requestHandler.downloadImg(imgUrl);

    cursor.insertHtml("\
        <!DOCTYPE html>\
        <html>\
        <body>\
            <img src=\"" + imgPath + "\">\
            <h1>" + title + "</h1>\
            <a href=\"" + link + "\">" + link + "</a>\
            <h4>" + pubDate + "</h4>\
            <h4>" + lastBuildDate + "</h4>\
            <p>" + description + "</p>\
        </body>\
        </html>\
    ");
}

void MainWindow::showFeed(QListWidgetItem *i) {
    auto *document = ui->textBrowser->document();
    auto cursor = QTextCursor(document);

    int index = ui->listWidget->row(i);
    auto feed = requestHandler.getFeed(index);

    ui->textBrowser->clear();

    QDomDocument doc;
    doc.setContent(feed);

    printFeedInfo(cursor, doc);
    printFeedItems(cursor, doc);
}

void MainWindow::refresh() {
    qDebug() << "Refresh";

    ui->listWidget->clear();
    for (const auto &state : requestHandler.getFormattedFeedStates()) {
        ui->listWidget->addItem(state);
    }
}

void MainWindow::addFeed() {
    qDebug() << "Add feed";

    if (ui->lineEdit->text().isEmpty()) return;

    QString url = ui->lineEdit->text();
    ui->lineEdit->clear();

    requestHandler.addFeed(url);
}

void MainWindow::editFeed() {
    qDebug() << "Edit feed";

    auto selected = ui->listWidget->selectedItems();
    if (selected.size() != 1) return;

    dw = new DialogWindow(this,
                          &requestHandler,
                          ui->listWidget->row(selected[0]));
    dw->show();
}

void MainWindow::deleteFeed() {
    qDebug() << "Delete feed";

    auto selected = ui->listWidget->selectedItems();
    if (selected.empty()) return;

    for (int i; const auto &f : selected) {
        i = ui->listWidget->row(f);
        requestHandler.deleteFeed(i);
    }
}

void MainWindow::filterByLabel() {
    if (ui->lineEdit_2->text().isEmpty()) {
        refresh();
        return;
    }

    QString target = ui->lineEdit_2->text();

    std::vector<QJsonObject> filtered;
    for (const auto &obj : requestHandler.getFeedStates()) {
        for (const auto &l : obj["labels"].toArray()) {
            if (l == target) {
                filtered.push_back(obj);
            }
        }
    }

    ui->listWidget->clear();
    for (const auto &feed : requestHandler.formatJsons(filtered)) {
        ui->listWidget->addItem(feed);
    }
}