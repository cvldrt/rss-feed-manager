#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtGui>
#include <QJsonArray>
#include "requestHandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DialogWindow; }
QT_END_NAMESPACE

class DialogWindow : public QDialog {
    Q_OBJECT
public:
    DialogWindow(QWidget *parent = nullptr, RequestHandler* = nullptr, int = 0);
    void cancel();
    void save();

private:
    Ui::DialogWindow *ui;
    RequestHandler* requestHandler;
    QString jsonArrayToString(const QJsonArray &array);
};

#endif // DIALOG_H
