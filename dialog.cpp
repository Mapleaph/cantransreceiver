#include "dialog.h"
#include "ui_dialog.h"

volatile bool openFlag = false;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    currentSettings = NULL;

    dataLe[0] = ui->lineEdit_1;
    dataLe[1] = ui->lineEdit_2;
    dataLe[2] = ui->lineEdit_3;
    dataLe[3] = ui->lineEdit_4;
    dataLe[4] = ui->lineEdit_5;
    dataLe[5] = ui->lineEdit_6;
    dataLe[6] = ui->lineEdit_7;
    dataLe[7] = ui->lineEdit_8;

    ui->treeWidget->header()->setSectionsMovable(false);
    ui->treeWidget->header()->setSectionResizeMode(2, QHeaderView::Stretch);  // set column "Data" to stretch mode

    on_checkBox_stateChanged(Qt::CheckState::Unchecked);
    ui->checkBox->setDisabled(true);
    ui->label->setDisabled(true);
    ui->spinBox->setDisabled(true);
    ui->pushButton_3->setDisabled(true);
    ui->comboBox->setDisabled(true);
    ui->lineEdit->setDisabled(true);
    ui->lineEdit->setText(tr("00"));

    QRegExpValidator *hexValidator = new QRegExpValidator(QRegExp("[0-9A-Fa-f]{1,2}"));

    for (qint8 i=0; i<MAX_DLC; i++) {
        dataLe[i]->setValidator(hexValidator); // set Data input ranges from 00 to 0xff
        dataLe[i]->setDisabled(true);
        dataLe[i]->setText(tr("00"));
    }

    ui->treeWidget->setDisabled(true);

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{

    cfgDlg = new ConfigDialog;

    connect(cfgDlg, SIGNAL(settings_confirmed(Settings)), this, SLOT(updateStatus(Settings)));

    cfgDlg->setModal(true);
    cfgDlg->exec();

}

void Dialog::updateStatus(Settings currentSettings)
{

#if DBG
    qDebug() << currentSettings.baudrate;
    qDebug() << currentSettings.device;
    qDebug() << currentSettings.ff;
#endif

    this->currentSettings = new Settings;

    this->currentSettings->baudrate = currentSettings.baudrate;
    this->currentSettings->device = currentSettings.device;
    this->currentSettings->ff = currentSettings.ff;

    ui->lineEdit_11->setText(currentSettings.device);
    ui->lineEdit_9->setText(currentSettings.baudrate);
    ui->lineEdit_10->setText(ff_str[currentSettings.ff]);

}

void Dialog::updateDataLineEditGrid(qint32 dlc)
{
    qint32 i;

    if (0 == dlc) {
        for (i=0; i<MAX_DLC; i++) {
            dataLe[i]->setDisabled(true);
        }
    } else {

        for (i=dlc; i<MAX_DLC; i++) {
            dataLe[i]->setDisabled(true);
        }

        for (i=0; i<dlc; i++) {
            dataLe[i]->setEnabled(true);
        }
    }
}

void Dialog::on_comboBox_currentTextChanged(const QString &arg1)
{
    qint32 dlc = arg1.toInt();

    updateDataLineEditGrid(dlc);
}

void Dialog::on_checkBox_stateChanged(int arg1)
{

    if (arg1 == Qt::CheckState::Checked) {
        ui->spinBox->setDisabled(true);
        ui->label->setDisabled(true);
        ui->pushButton_3->setDisabled(true);
        for (qint8 i=0; i<MAX_DLC; i++) {
            dataLe[i]->setDisabled(true);
        }
        ui->lineEdit->setDisabled(true);
        ui->comboBox->setDisabled(true);

    } else if (arg1 == Qt::CheckState::Unchecked) {
        ui->spinBox->setEnabled(true);
        ui->label->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        for (qint8 i=0; i<MAX_DLC; i++) {
            dataLe[i]->setEnabled(true);
        }
        ui->lineEdit->setEnabled(true);
        ui->comboBox->setEnabled(true);

    }

}

void Dialog::on_pushButton_2_clicked()
{
    if (!openFlag) {

        if (this->currentSettings != NULL && this->currentSettings->device != NULL) {

            QRegExpValidator *hexValidator;

            if (this->currentSettings->ff == 0) { // standard

                hexValidator = new QRegExpValidator(QRegExp("[0-7]{0,1}[0-9A-Fa-f]{1,2}")); // 000-7ff

            } else { // extended

                hexValidator = new QRegExpValidator(QRegExp("[0-1]{0,1}[0-9A-Fa-f]{1,7}")); // 000-1fffffff
            }

            ui->lineEdit->setValidator(hexValidator);

            transceiver = new Worker();
            workerThread = new QThread;

            transceiver->moveToThread(workerThread);

            connect(workerThread, SIGNAL(finished()), transceiver, SLOT(deleteLater()));
            connect(this, SIGNAL(sigOpen(Settings)), transceiver, SLOT(doOpen(Settings)));
            connect(this, SIGNAL(sigClose()), transceiver, SLOT(doClose()));
            connect(transceiver, SIGNAL(sigOpened()), this, SLOT(handleOpened()));
            connect(transceiver, SIGNAL(sigExitThread()), this, SLOT(exitThread()));
            //connect(transceiver, SIGNAL(sigClosed()), this, SLOT(handleClosed()));
            connect(transceiver, SIGNAL(sigOpenFailed()), this, SLOT(handleOpenFailed()));
            connect(this, SIGNAL(sigSend(CAN_MESSAGE)), transceiver, SLOT(doSend(CAN_MESSAGE)));
            connect(this, SIGNAL(sigLoopSend(CAN_MESSAGE, qint32)), transceiver, SLOT(doLoopSend(CAN_MESSAGE, qint32)));
            connect(transceiver, SIGNAL(sigRecved(CAN_MESSAGE, qint32)), this, SLOT(handleRecved(CAN_MESSAGE, qint32)));
            workerThread->start();

            emit sigOpen(*(this->currentSettings));

        } else {

            QMessageBox::warning(this, tr("CAN Transceiver"), tr("Please Config the Port First!"));

        }

    } else {

        transceiver->loopFlag = false;
        emit sigClose();
        openFlag = false;
        ui->pushButton_2->setText(tr("Open"));
        changeToClosedUi();
        //delete transceiver;
    }

}

void Dialog::exitThread()
{
    workerThread->exit();
    workerThread->wait();
}

void Dialog::changeToOpenedUi()
{
    openFlag = true;
    ui->pushButton_2->setText(tr("Close"));
    ui->checkBox->setEnabled(true);
    ui->spinBox->setEnabled(true);
    ui->label->setEnabled(true);
    ui->treeWidget->setEnabled(true);
    ui->comboBox->setEnabled(true);

    updateDataLineEditGrid(ui->comboBox->currentText().toInt());

    ui->pushButton_3->setEnabled(true);

}

void Dialog::handleOpened()
{
    openFlag = true;

    ui->pushButton_2->setText(tr("Close"));
    ui->checkBox->setEnabled(true);
    ui->spinBox->setEnabled(true);
    ui->label->setEnabled(true);
    ui->treeWidget->setEnabled(true);
    ui->comboBox->setEnabled(true);
    ui->lineEdit->setEnabled(true);

    updateDataLineEditGrid(ui->comboBox->currentText().toInt());

    ui->pushButton_3->setEnabled(true);

}


void Dialog::changeToClosedUi()
{

    on_checkBox_stateChanged(Qt::CheckState::Unchecked);
    ui->checkBox->setDisabled(true);
    ui->label->setDisabled(true);
    ui->spinBox->setDisabled(true);
    ui->pushButton_3->setDisabled(true);
    ui->comboBox->setDisabled(true);
    ui->lineEdit->setDisabled(true);

    for (qint8 i=0; i<MAX_DLC; i++) {
        dataLe[i]->setDisabled(true);
    }

    ui->treeWidget->setDisabled(true);

}

void Dialog::on_pushButton_3_clicked()
{

    //qint8 dlc = ui->comboBox->currentText().toInt();
    //qint8 id = ui->lineEdit->text().toInt(0, 16);
    //qint8 ff = this->currentSettings->ff;

    CAN_MESSAGE *msg = new CAN_MESSAGE;

    msg = msgGen();

#if DBG
    qDebug() << msg->ID;
    qDebug() << msg->FrameFormat;
    qDebug() << msg->DataLenCode;
    qDebug() << msg->RemoteTransReq;
#endif

    emit sigSend(*msg);

}


CAN_MESSAGE* Dialog::msgGen()
{

    CAN_MESSAGE *msg = new CAN_MESSAGE;

    qint8 dlc = ui->comboBox->currentText().toInt();
    qint32 id = ui->lineEdit->text().toInt(0, 16);
    qint8 ff = this->currentSettings->ff;

    if (dlc > 0) {

        for (qint8 i=0; i<dlc; i++) {
            msg->Data[i] = dataLe[i]->text().toInt(0, 16);
        }

        msg->ID = id;
        msg->FrameFormat = ff;
        msg->DataLenCode = dlc;
        msg->RemoteTransReq = 0;

    }

    return msg;
}


void Dialog::on_checkBox_clicked()
{
    CAN_MESSAGE *msg = new CAN_MESSAGE;

    msg = msgGen();

    qint32 interval = ui->spinBox->text().toInt();

    if (ui->checkBox->isChecked()) {
        transceiver->loopFlag = true;
        emit sigLoopSend(*msg, interval);
    } else {
        transceiver->loopFlag = false;
    }
}

void Dialog::handleRecved(CAN_MESSAGE msg, qint32 elapsedTime)
{

#if DBG
    qDebug() << "handleRecved";
    qDebug() << msg.ID;
    qDebug() << msg.DataLenCode;
#endif

    QStringList dataList;
    for (qint8 i=0; i<msg.DataLenCode; i++) {
        dataList << QString::number(msg.Data[i], 16);
    }

    QTreeWidgetItem *item = NULL;

    QList<QTreeWidgetItem*> items = ui->treeWidget->findItems(QString::number(msg.ID), Qt::MatchExactly, 0);

    if (items.size() == 0) {

        item = new QTreeWidgetItem;

        item->setText(0, QString::number(msg.ID));
        item->setText(1, QString::number(msg.DataLenCode));
        item->setText(2, dataList.join(' '));
        item->setText(3, QString::number(elapsedTime));
        item->setText(4, "1");

        ui->treeWidget->insertTopLevelItem(0, item);

    } else {

        item = items.first();
        item->setText(1, QString::number(msg.DataLenCode));
        item->setText(2, dataList.join(' '));
        item->setText(3, QString::number(elapsedTime));
        item->setText(4, QString::number(items.first()->text(4).toInt()+1));
    }


}

void Dialog::handleOpenFailed()
{
    QMessageBox::warning(this, tr("CAN Transceiver"), tr("Open Port Failed!"));
    exitThread();
}

void Dialog::handleClosed()
{
    exitThread();

}
