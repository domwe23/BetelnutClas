//
// Created by Adminstrator on 25-8-1.
//

#ifndef CALIBRATIONWINDOW_H
#define CALIBRATIONWINDOW_H

#include <QWidget>
#include <QTimer>
#include "boardcard.h"
#include "MCDLL_NET_SORTING.h"
#include <QDoubleValidator>
#include <QComboBox>


QT_BEGIN_NAMESPACE
namespace Ui { class CalibrationWindow; }
QT_END_NAMESPACE

class CalibrationWindow : public VpMainWindow {
Q_OBJECT



signals:
void parameterSaved(double newRatio);

public:
    explicit CalibrationWindow(QWidget *parent = nullptr);
    ~CalibrationWindow() override;

    void updaEncoders();
    void InitWindow();
    void disposition();
    void disposition2();
    void saveConfig();
    void saveConfigCamera();
    void LoadConfig();
    void LoadConfigCamera();
    void LoadLocationRowToTable();
    void writeLogToFile(const QString &logText);
    void writeLog(const QString &logText);


    BoardCard *myBoardCard;
    int calibration1=0;
    int calibration2=0;
    int Encoder=0;
    int channel=1;
    double k=24;
    double ratio=0.005;

    QString boards_counts="1";
    QString pathFile="C:/vpconfig/Config.ini";
    QVector<int> boards_state={0,0,0};
    QVector<QString>  boards_number= {"0", "0", "0"};
    QVector<QString>  boards_type= {"0", "0", "0"};

    QVector<QString>  material_detect= {"0", "0", "0"};
    QVector<QString>  material_Inlet= {"0", "0", "0"};
    QVector<QString>  material_level= {"0", "0", "0"};
    QVector<QString>  material_filtering= {"0", "0", "0"};
    QVector<QString>  material_photospot= {"0", "0", "0"};

    QVector<QString>  camera_name= {"相机1", "相机1", "相机1"};
    QVector<QString>  camera_direction= {"0", "0", "0"};
    QVector<QString>  camera_delay= {"0", "0", "0"};
    QVector<QString>  camera_offset= {"0", "0", "0"};
    QVector<QString>  camera_light_advance= {"0", "0", "0"};
    QVector<QString>  camera_location= {"0", "0", "0"};
    QVector<QString>  camera_out_tags= {"0", "0", "0"};
    QVector<QString>  camera_out_action= {"0", "0", "0"};

    QVector<QString>  image_proctime= {"0", "0", "0"};
    QVector<QString>  image_timeout= {"0", "0", "0"};
    QVector<QString>  image_number= {"0", "0", "0"};

    QVector<QStringList> valve_name  = {
        (QStringList() << "气阀1" << "气阀2" << "气阀3" << "气阀4" << "气阀5" << "气阀6" << "气阀7"<< "气阀8" << "气阀9" << "气阀10" << "气阀11" << "气阀12" << "气阀13"),
        (QStringList() << "气阀1" << "气阀2" << "气阀3" << "气阀4" << "气阀5" << "气阀6" << "气阀7"<< "气阀8" << "气阀9" << "气阀10" << "气阀11" << "气阀12" << "气阀13"),
        (QStringList() << "气阀1" << "气阀2" << "气阀3" << "气阀4" << "气阀5" << "气阀6" << "气阀7"<< "气阀8" << "气阀9" << "气阀10" << "气阀11" << "气阀12" << "气阀13")
    };
    QVector<QStringList> location = {
        (QStringList() << "0" << "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0" << "0" << "0" << "0"),
        (QStringList() << "0" << "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0" << "0" << "0" << "0"),
        (QStringList() << "0" << "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0" << "0" << "0" << "0")
    };
    QVector<QStringList> valve_out_tags = {
        (QStringList() << "0" << "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0" << "0" << "0" << "0"),
        (QStringList() << "0" << "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0" << "0" << "0" << "0"),
        (QStringList() << "0" << "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0" << "0" << "0" << "0")
    };
    QVector<QStringList> valve_out_action = {
        (QStringList() << "0" << "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0" << "0" << "0" << "0"),
        (QStringList() << "0" << "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0" << "0" << "0" << "0"),
        (QStringList() << "0" << "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0" << "0" << "0" << "0")
    };

private:
    Ui::CalibrationWindow *ui;
};


#endif //CALIBRATIONWINDOW_H
