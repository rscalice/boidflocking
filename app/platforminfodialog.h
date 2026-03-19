#ifndef PLATFORMINFODIALOG_H
#define PLATFORMINFODIALOG_H

#include <QDialog>

namespace Ui {
class PlatformInfoDialog;
}

class PlatformInfoDialog : public QDialog
{
    Q_OBJECT

public slots:
    void updatePlatformInfoTextbox(QString platformInfo);

public:
    explicit PlatformInfoDialog(QWidget *parent = 0);
    ~PlatformInfoDialog();

    void setPlatformInfo(QString platformInfo);

signals:
    void setPlatformInfoText(QString platformInfo);

private:
    Ui::PlatformInfoDialog *ui;
};

#endif // PLATFORMINFODIALOG_H
