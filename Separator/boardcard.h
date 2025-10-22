//
// Created by Adminstrator on 25-7-23.
//

#ifndef BOARDCARD_H
#define BOARDCARD_H

#include <QWidget>
#include <QStandardItemModel>
#include <QSettings>
#include <QMessageBox>
#include <QTime>
#include <QFileInfo>
#include <QDir>
#include <QGroupBox>
#include <QLabel>
#include "VpMainWindow.h"
#include "MCDLL_NET_SORTING.h"
#include "VpLog.h"
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDateTime>
#include <QQueue>
#include <QThread>
#include <QMutexLocker>

QT_BEGIN_NAMESPACE
namespace Ui { class BoardCard; }
QT_END_NAMESPACE

class BoardCard : public VpMainWindow {
Q_OBJECT

public:
    explicit BoardCard(QWidget *parent = nullptr);
    ~BoardCard() override;
    void InitWindow();
    void InitConnect();
    void savepParameter();
    void LoadConfig();
    void LoadConfigToPage();
    void writeLog(const QString &logText);

    int openBoard();
    int stopBoard();
    int openSift();
    int stopSift();
    int closeBoard();
    int clearcount();
    int setCameraBlowParameter();
    int setCameraParameter();
    int setCameraDelay();
    int setCameraOffset();
    short MCF_Screen_Get_Distance_Encoder(unsigned short StationNumber = 0);

    int setValveParameter();

    int InitBoardConfig();

    void Get_Rotary_Encoder(short StationNumber);
    void Get_Rotary_Encoders();
    void Get_channel_Speed();
    void Get_Camera_Count(short camera_name ,short StationNumber);
    void Get_Camera_Counts();
    void updateEncoderValue();
    void updateUiValue();
    void updateChannelValue();
    void Get_material_Count(short StationNumber);
    void Get_material_Counts();
    void Get_DI0_Status();
    void listeningStacking();
    void listeningStacking1();
    void listeningStacking2();
    void listeningStacking3();
    void listeningStartMotor();
    void listeningStopMotor();
    void Get_DO0_Status();
    void GEt_Board_state();
    double calculateAverageSpeed(QQueue<double>& history, double newSpeed, int maxSize);
    void loadAppConfig();

    QStringList Get_Trig_Blow();
    QString extractCode(const QString& text);
    QString extractRawTimeValue(const QString& text);
    QGroupBox* createStatusGroup(const QString& title, int lightCount,int index);
    void updateLight();
    void writeLogToFile(const QString &logText);

    void testOutput1(unsigned short outionumber,unsigned short outlogic,unsigned outstation);
    void testOutput2(
        unsigned short BlowChannal,
        unsigned short Motion_Dir,
        unsigned short Action_Mode,
        unsigned short Action_IO,
        unsigned short StationNumber = 0);


    void testOutput(unsigned short outionumber,unsigned short outlogic,unsigned outstation);


    // 启动/停止监控线程
    void startMonitoring();
    void stopMonitoring();

    std::atomic<bool> camThreadRunning=false;
    std::atomic<bool> materialThreadRunning=false;
    QMutex camMutex;
    QMutex materialMutex;

    QThread *camThread=nullptr;
    QThread *materialThread=nullptr;

    void monitorCamcons();
    void monitorMaterials();




 signals:
    void camconChanged(int index, int value);
    void materialChanged(int index, int value);
    void plcStartMotor(int index);
    void plcStopMotor(int index);


public:
    QString pathFile="C:/vpconfig/Config.ini";
    QVector<int> lastMaterialCount = {0,0,0};
    QVector<QTime> lastTime;
    QQueue<double> channelSpeedHistory[3];
    int historySize=12;

    int passage=1;//通道
    int channel=1;
    int Encoder=0;
    int BoarState=-1; //板卡状态
    QVector<int> Encoders={0,0,0}; //编码器脉冲
    QVector<int> camcons={0,0,0};   //相机计数
    QVector<int> materials={0,0,0}; //入料数
    QVector<int> DI0Status={1,1,1}; //DIO
    QVector<int> DI03Status={1,1,1,1,1,1,1,1,1}; //DIO~3
    QVector<QString> Stacking={"500","500","500"};//堆料脉冲
    QVector<int> lastCamcons=camcons;
    QVector<int> lastMaterials=materials;

    int camcon=0;
    int material=0;
    int Trigger1=0;
    int Trigger2=0;
    int Distance=0;
    QThread *mater;


    QLabel *light1;
    QLabel *light2;
    QLabel *light3;
    QLabel *light4;
    QLabel *light5;
    QLabel *light6;
    QLabel *light7;
    QLabel *light8;
    QLabel *light9;
    QVBoxLayout *rightLayout;
    QStandardItemModel *model;


    // 物件数量
    uint32_t Trigger_Piece_Number[2];
    // 尺寸合格物件数
    uint32_t Trigger_Piece_Find4[2];
    uint32_t Trigger_Piece_Find1[2];
    uint32_t Trigger_Piece_Find2[2];
    uint32_t Trigger_Piece_Find3[2];
    //缓存物件数
    uint32_t Rate_Piece_Find_Dly1[2];
    uint32_t Rate_Piece_Find_Dly2[2];
    uint32_t Rate_Piece_Find_Dly3[2];
    // 尺寸合格的物件百分比
    uint32_t Trigger_Piece_Size_Ok_Percentage[2];
    // 尺寸不合格物件数
    uint32_t Rate_Piece_Pass[2];
    uint32_t Rate_Piece_Pass2[2];

    //时间
    QTime Rate_Tick[3] = { QTime(0,0,0), QTime(0,0,0), QTime(0,0,0) };
    QVector<int> channelSpeed={0,0,0};

    QString boards_counts="1";
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
    QVector<QStringList> valve_location = {
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
    Ui::BoardCard *ui;


};


#endif //BOARDCARD_H
