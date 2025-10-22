//
// Created by Adminstrator on 25-7-22.
//

#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QWidget>
#include "VpMainWindow.h"
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QSettings>



QT_BEGIN_NAMESPACE
namespace Ui { class settingWindow; }
QT_END_NAMESPACE

class settingWindow : public VpMainWindow {
Q_OBJECT

public:
    explicit settingWindow(QWidget *parent = nullptr);
    // settingWindow(int index);

    ~settingWindow() override;

     void initWindow();

    void onPushButton4Clicked();

    void loadSettingPage();

    void loadSettingValve();

    QComboBox *dirBox3;
    QComboBox *outputBox3;
    QLineEdit *Qfiltering;

    QLineEdit *Qcamera_name;
    QLineEdit *Qcamera_location;
    QComboBox *directionBox ;
    QComboBox *outputPortBox;
    QComboBox *actionBox;
    QLineEdit *Qcamera_delay;
    QSpinBox *offsetSpin;
    QLineEdit *Qlight_advance;


    QComboBox *ValveBox;
    QLineEdit *Qvalve_location;
    QComboBox *dirBox;
    QComboBox *outputBox;
    QComboBox *actionBox2;
    QSpinBox *offsetBox;




    int channel=1;

    QString is_enable="开启";
    QString level="低电平";
    QString filtering="0";

   QString station_number="0";
   QString card_type="2";

   QString camera_name="相机";
   QString camera_location="0";
   QString camera_direction="正方向";
   QString camera_out_tags="D01";
   QString camera_out_action="0";
   QString camera_delay="0";
   QString camera_offset="0";
   QString camera_light_advance="0";
    QStringList valve_name = QStringList() << "气阀1" << "气阀2" << "气阀3" << "气阀4" << "气阀5" << "气阀6" << "气阀7" << "气阀8" << "气阀9" << "气阀10" << "气阀11" << "气阀12" << "气阀13";
   QStringList valve_location = {"0","0","0","0","0","0","0","0","0","0","0","0","0"};
   QStringList valve_direction= {"0","0","0","0","0","0","0","0","0","0","0","0","0"};
   QStringList valve_out_tags= {"0","0","0","0","0","0","0","0","0","0","0","0","0"};
   QStringList valve_out_action= {"0","0","0","0","0","0","0","0","0","0","0","0","0"};
   QStringList valve_offset= {"0","0","0","0","0","0","0","0","0","0","0","0","0"};
private:
    Ui::settingWindow *ui;
};


#endif //SETTINGWINDOW_H
