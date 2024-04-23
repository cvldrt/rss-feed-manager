#include "../include/dialogWindow.h"
#include "./ui_dialogwindow.h"
#include <QDialog>

DialogWindow::DialogWindow(QWidget *parent, RequestHandler *rh, int i)
    : QDialog(parent), ui(new Ui::DialogWindow), requestHandler(rh) {
    auto feed = requestHandler->getFeedStates()[i];

    ui->setupUi(this);
    ui->lineEdit->setText(feed["url"].toString());
    ui->lineEdit_2->setText(jsonArrayToString(feed["labels"].toArray()));
    ui->spinBox->setValue(feed["interval"].toInt());

    connect(ui->cancelButton,
            &QAbstractButton::clicked,
            this,
            &DialogWindow::cancel);
    connect(ui->saveButton,
            &QAbstractButton::clicked,
            this,
            &DialogWindow::save);
}

QString DialogWindow::jsonArrayToString(const QJsonArray& array){
    QString str;
    for (const auto &l :array){
        str += l.toString() + ',';
    }
    str.remove(-1, 1);
    return str;
}

void DialogWindow::cancel() {
    this->close();
}

void DialogWindow::save() {
    QString url = ui->lineEdit->text();
    QString labels = ui->lineEdit_2->text();
    int pollInterval = ui->spinBox->value();

    requestHandler->updateFeed({url, labels, pollInterval});

    this->close();
}