#include "configdialog.h"
#include "ui_configdialog.h"

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::on_pushButton_clicked()
{
    currentSettings = new Settings;

    currentSettings->baudrate = ui->comboBox->currentText();
    currentSettings->device = ui->lineEdit->text();
    currentSettings->ff = ui->comboBox_2->currentIndex();

    emit settings_confirmed(*currentSettings);

    QDialog::accept();
}

void ConfigDialog::on_pushButton_2_clicked()
{
    QDialog::accept();
}
