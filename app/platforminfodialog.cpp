#include "platforminfodialog.h"
#include "ui_platforminfodialog.h"

PlatformInfoDialog::PlatformInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlatformInfoDialog)
{
    ui->setupUi(this);

    connect(this, SIGNAL(setPlatformInfoText(QString)), this, SLOT(updatePlatformInfoTextbox(QString)));
}

PlatformInfoDialog::~PlatformInfoDialog()
{
    delete ui;
}

void PlatformInfoDialog::updatePlatformInfoTextbox(QString platformInfo)
{
    ui->platformInfoText->setText(platformInfo);
}


void PlatformInfoDialog::setPlatformInfo(QString platformInfo)
{
    emit setPlatformInfoText(platformInfo);
}
