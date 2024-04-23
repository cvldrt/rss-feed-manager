#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "requestHandler.h"
#include "dialogWindow.h"
#include <QListWidgetItem>
#include <QtGui>
#include <QDomDocument>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    MainWindow(const QString&, QWidget *parent = nullptr);

private:
    Ui::MainWindow *ui;
    DialogWindow *dw;
    RequestHandler requestHandler;

public slots:
    void refresh();
    void addFeed();
    void editFeed();
    void deleteFeed();
    void filterByLabel();
    void showFeed(QListWidgetItem*);
    void printFeedInfo(QTextCursor&, QDomDocument&);
};

#endif // MAINWINDOW_H
