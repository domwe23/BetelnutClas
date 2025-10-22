//
// Created by adminstrator on 25-9-12.
//

#ifndef BLSIZEWINDOW_H
#define BLSIZEWINDOW_H

#include <QWidget>
#include "VpMainWindow.h"
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include "VpLog.h"


QT_BEGIN_NAMESPACE
namespace Ui { class BlSizeWindow; }
QT_END_NAMESPACE

class BlSizeWindow : public VpMainWindow {
Q_OBJECT

public:
    explicit BlSizeWindow(QWidget *parent = nullptr);
    ~BlSizeWindow() override;

    void initConnect();
    void saveConfig();
    void loadConfig();
    void updateUiFromVars();

signals:
    void camRatioCheck(int index,double ratio);


public:
    QStringList A_W = {"0","0"};
    QStringList A_H = {"0","0"};
    QStringList B_W = {"0","0"};
    QStringList B_H = {"0","0"};
    QStringList C_W = {"0","0"};
    QStringList C_H = {"0","0"};
    QStringList D_W = {"0","0"};
    QStringList D_H = {"0","0"};
    QStringList E1_W = {"0","0"};
    QStringList E1_H = {"0","0"};
    QStringList E2_W = {"0","0"};
    QStringList E2_H = {"0","0"};
    QStringList E3_W = {"0","0"};
    QStringList E3_H = {"0","0"};
    QStringList E4_W = {"0","0"};
    QStringList E4_H = {"0","0"};
    QStringList E5_W = {"0","0"};
    QStringList E5_H = {"0","0"};
    QStringList camRatio = {"0","0","0"};
    QString camRatio1="0";
    QString camRatio2="0";
    QString camRatio3="0";

private:
    Ui::BlSizeWindow *ui;
    QString pathFile="C:/vpconfig/Config.ini";

};


#endif //BLSIZEWINDOW_H
