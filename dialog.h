#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QThread>
#include <QMessageBox>
#include <QDebug>
#include "common.h"
#include "configdialog.h"
#include "worker.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    void updateDataLineEditGrid(qint32 dlc);
    CAN_MESSAGE* msgGen();

signals:
    void sigOpen(Settings currentSettings);
    void sigSend(CAN_MESSAGE);
    void sigLoopSend(CAN_MESSAGE, qint32);
    void sigClose();

private slots:
    void on_pushButton_clicked();
    void updateStatus(Settings currentSettings);
    void on_comboBox_currentTextChanged(const QString &arg1);
    void on_checkBox_stateChanged(int arg1);
    void on_pushButton_2_clicked();
    void exitThread();
    void changeToOpenedUi();

    void changeToClosedUi();
    void handleOpenFailed();
    void handleOpened();
    void handleClosed();

    void on_pushButton_3_clicked();

    void on_checkBox_clicked();
    void handleRecved(CAN_MESSAGE msg, qint32 elapsedTime);

private:
    Ui::Dialog *ui;
    ConfigDialog *cfgDlg;
    QLineEdit *dataLe[MAX_DLC];
    Worker *transceiver;
    QThread *workerThread;
    Settings *currentSettings;


};

#endif // DIALOG_H
