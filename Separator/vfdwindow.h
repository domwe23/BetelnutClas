//
// Created by Adminstrator on 25-7-27.
//

#ifndef VFDWINDOW_H
#define VFDWINDOW_H

#include <QWidget>
#include "VpMainWindow.h"
#include <QSettings>
#include <QDir>
#include <QCoreApplication>
#include <QMessageBox>
#include "ProfinetManaged.h"
#include <QDate>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui { class VFDWindow; }
QT_END_NAMESPACE

class VFDWindow : public VpMainWindow {
Q_OBJECT



public:
    explicit VFDWindow(QWidget *parent = nullptr);
    ~VFDWindow() override;

    int Handle =-1;
    int vfdConnect=0;
    QString serial="COM5";
    QString baud_rate="9600";
    QString data_bits="8";
    QString stop_bits="1";
    QString parity="2";
    int serialConnect=0;
    QString pathFile="C:/vpconfig/Config.ini";

    const char* speedAddr = "100";
    const char* directionAddr = "99";
    const char* startAddr = "99";
    QMap<int,int> serial_Map;


    QString sorting_Station ="0";
    QString sorting_velocity ="0";
    QString sorting_direction ="0";

    QString loading_Station ="0";
    QString loading_velocity ="0";
    QString loading_direction ="0";

    QString lift_Station ="0";
    QString lift_velocity ="0";
    QString lift_direction ="0";

    QVector<QStringList> serialParame = {
        QStringList() << "1" << "1" << "1",
        QStringList() << "2" << "1" << "1",
        QStringList() << "3" << "1" << "1",
        QStringList() << "4" << "1" << "1",
        QStringList() << "5" << "1" << "1",
        QStringList() << "6" << "1" << "1",
        QStringList() << "7" << "1" << "1",
        QStringList() << "8" << "1" << "1"
    };

    QStringList vfdstart=QStringList()<< "0" << "0" << "0"<< "0" << "0" << "0"<<"0"<<"0";





    void on_pushButton_clicked();
    void loadFromIni();
    void loadSerialIni(int index=1);
    void initConnect();
    int initSerial();
    void initSerialParameter();
    void closeSerial();
    void on_pushButton2_start();
    void on_pushButton2_stop();
    void on_pushButton3_start();
    void on_pushButton3_stop();
    int createSerial(int station);
    int initSerial2();
    int initSerial3();
    void saveConfig();
    void loadSerialParame();
    void initConect();
    void serialCon();
    void serialWrite(int station);
    void serialWrite10(int station=10);
    void serialStop(int station);
    int initSerialStop();
    int serialWriteSpeed(int handle,int station,uint16_t speed);
    int serialWriteDirection(int handle,int station,uint16_t direction);
    void connectMotor(int index);
    void clearMaterials();
    int comState();
    void writeLog(const QString &logText);
private:
    Ui::VFDWindow *ui;
};


#endif //VFDWINDOW_H
