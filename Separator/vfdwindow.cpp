//
// Created by Adminstrator on 25-7-27.
//

// You may need to build the project (run Qt uic code generator) to get "ui_VFDWindow.h" resolved

#include "vfdwindow.h"

#include <VpLog.h>

#include "MCDLL_NET_SORTING.h"
#include "ui_VFDWindow.h"


VFDWindow::VFDWindow(QWidget *parent) :
    VpMainWindow(parent), ui(new Ui::VFDWindow) {
    ui->setupUi(this);
    initConect();
    ui->verticalLayout->setStretch(0,1);
    ui->verticalLayout->setStretch(1,3);
    ui->verticalLayout_2->setStretch(0,1);
    ui->verticalLayout_2->setStretch(1,3);
    ui->verticalLayout_3->setStretch(0,1);
    ui->verticalLayout_3->setStretch(1,3);

    ui->checkBox->setChecked(true);
    //ui->checkBox_2->setChecked(true);
    this->setWindowTitle("变频器设置");

    /*ui->verticalLayout_4->setStretch(0,5);
    ui->verticalLayout_4->setStretch(1,1);*/

   // connect(ui->pushButton, &QPushButton::clicked, this, &VFDWindow::on_pushButton_clicked);




    connect(ui->pushButton_9, &QPushButton::clicked, this, &VFDWindow::connectMotor);
    connect(ui->pushButton_10, &QPushButton::clicked, this, &VFDWindow::saveConfig);


    ui->pushButton_2->setCheckable(true);
    connect(ui->pushButton_2, &QPushButton::clicked, this, [=](bool checked) {
        if (checked) {
            serialWrite(0);
            ui->pushButton_2->setText("停止");
        } else {
            serialStop(0);
            ui->pushButton_2->setText("启动");
        }
    });

    ui->pushButton_3->setCheckable(true);
    connect(ui->pushButton_3, &QPushButton::clicked, this, [=](bool checked) {
        if (checked) {
            serialWrite(1);
            ui->pushButton_3->setText("停止");
        } else {
            serialStop(1);
            ui->pushButton_3->setText("启动");
        }
    });

    ui->pushButton_6->setCheckable(true);
    connect(ui->pushButton_6, &QPushButton::clicked, this, [=](bool checked) {
        if (checked) {
            serialWrite(2);
            ui->pushButton_6->setText("停止");
        } else {
            serialStop(2);
            ui->pushButton_6->setText("启动");
        }
    });

    ui->pushButton_7->setCheckable(true);
    connect(ui->pushButton_7, &QPushButton::clicked, this, [=](bool checked) {
        if (checked) {
            serialWrite(3);
            ui->pushButton_7->setText("停止");
        } else {
            serialStop(3);
            ui->pushButton_7->setText("启动");
        }
    });
    ui->pushButton_8->setCheckable(true);
    connect(ui->pushButton_8, &QPushButton::clicked, this, [=](bool checked) {
        if (checked) {
            serialWrite(4);
            ui->pushButton_8->setText("停止");
        } else {
            serialStop(4);
            ui->pushButton_8->setText("启动");
        }
    });
    ui->pushButton_4->setCheckable(true);
    connect(ui->pushButton_4, &QPushButton::clicked, this, [=](bool checked) {
        if (checked) {
            serialWrite(5);
            ui->pushButton_4->setText("停止");
        } else {
            serialStop(5);
            ui->pushButton_4->setText("启动");
        }
    });
    ui->pushButton_5->setCheckable(true);
    connect(ui->pushButton_5, &QPushButton::clicked, this, [=](bool checked) {
        if (checked) {
            serialWrite(6);
            ui->pushButton_5->setText("停止");
        } else {
            serialStop(6);
            ui->pushButton_5->setText("启动");
        }
    });
    ui->pushButton_11->setCheckable(true);
    connect(ui->pushButton_11, &QPushButton::clicked, this, [=](bool checked) {
        if (checked) {
            serialWrite(7);
            ui->pushButton_11->setText("停止");
        } else {
            serialStop(7);
            ui->pushButton_11->setText("启动");
        }
    });
    //connect(ui->pushButton_2, &QPushButton::clicked, this, [=]() { serialWrite(0); });
    //connect(ui->pushButton_3, &QPushButton::clicked, this, [=]() { serialWrite(1); });
    //connect(ui->pushButton_6, &QPushButton::clicked, this, [=]() { serialWrite(2); });
    //connect(ui->pushButton_7, &QPushButton::clicked, this, [=]() { serialWrite(3); });
    //connect(ui->pushButton_8, &QPushButton::clicked, this, [=]() { serialWrite(4); });
    //connect(ui->pushButton_4, &QPushButton::clicked, this, [=]() { serialWrite(5); });
    //connect(ui->pushButton_5, &QPushButton::clicked, this, [=]() { serialWrite(6); });

    loadFromIni();
    loadSerialIni();
    initConnect();
    // if (serialConnect==0)
    // initSerial();
    //this->setFixedSize(830, 520);
    loadSerialParame();
    initSerial2(); //连接串口

    /*LOG_INFO("开始写入")
    for (int i=0;i<7;i++)
    {
        serialWriteSpeed(i);
    }
LOG_INFO("结束写入")*/
    /*if (ui->checkBox->isChecked())
    {
        ui->pushButton_2->setCheckable(false);
    }
    if (ui->checkBox_2->isChecked())
    {
        ui->pushButton_3->setCheckable(false);
    }*/


}

VFDWindow::~VFDWindow() {
    delete ui;
}



void VFDWindow::on_pushButton_clicked()
{
    QFileInfo fileInfo(pathFile);
    QDir dir = fileInfo.dir();


    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            return;
        }
    }

    QFile iniFile(pathFile);
    QSettings settings(pathFile, QSettings::IniFormat);
    settings.beginGroup("vfd");
    settings.setValue("sorting_Station", sorting_Station);
    settings.setValue("sorting_velocity", sorting_velocity);
    settings.setValue("sorting_direction", sorting_direction);

    settings.setValue("loading_Station", loading_Station);
    settings.setValue("loading_velocity", loading_velocity);
    settings.setValue("loading_direction", loading_direction);

    settings.setValue("lift_Station", lift_Station);
    settings.setValue("lift_velocity", lift_velocity);
    settings.setValue("lift_direction", lift_direction);
    settings.endGroup();


    QMessageBox::information(this, "提示", "保存到成功");


    if (serialConnect==0)
        initSerial();

    initSerialParameter();

}




void VFDWindow::loadFromIni()
{
    /*// QDir dir(QCoreApplication::applicationDirPath());
    // dir.cdUp();
    // QString iniFilePath = dir.absoluteFilePath("Config.ini");

    QFile iniFile(pathFile);
    if (!iniFile.exists()) {
        QMessageBox::warning(this, "错误", "配置文件 Config.ini 不存在！");
        return;
    }

    QSettings settings(pathFile, QSettings::IniFormat);
    settings.beginGroup("vfd");

    sorting_Station   = settings.value("sorting_Station", "0").toString();
    sorting_velocity  = settings.value("sorting_velocity", "0").toString();
    sorting_direction = settings.value("sorting_direction", "0").toString();

    loading_Station   = settings.value("loading_Station", "0").toString();
    loading_velocity  = settings.value("loading_velocity", "0").toString();
    loading_direction = settings.value("loading_direction", "0").toString();

    lift_Station      = settings.value("lift_Station", "0").toString();
    lift_velocity     = settings.value("lift_velocity", "0").toString();
    lift_direction    = settings.value("lift_direction", "0").toString();


    settings.endGroup();


    ui->lineEdit->setText(sorting_Station);
    ui->lineEdit_2->setText(sorting_velocity);
    ui->lineEdit_3->setText(sorting_direction);

    ui->lineEdit_4->setText(loading_Station);
    ui->lineEdit_5->setText(loading_velocity);
    ui->lineEdit_6->setText(loading_direction);

    ui->lineEdit_7->setText(lift_Station);
    ui->lineEdit_8->setText(lift_velocity);
    ui->lineEdit_9->setText(lift_direction);*/
}


void VFDWindow::loadSerialIni(int index) {
    QFile iniFile(pathFile);
    if (!iniFile.exists()) {
        QMessageBox::warning(this, "错误", "配置文件 Config.ini 不存在！");
        return;
    }

    QSettings settings(pathFile, QSettings::IniFormat);
    if (index==1) {
        settings.beginGroup("com1");
    }
    else if (index==2) {
        settings.beginGroup("com2");
    }else {
        settings.beginGroup("com3");
    }

    serial   = settings.value("serial", "").toString();
    baud_rate  = settings.value("baud_rate", "").toString();
    data_bits = settings.value("data_bits", "").toString();
    stop_bits   = settings.value("stop_bits", "").toString();
    parity  = settings.value("parity", "").toString();

    settings.endGroup();
}




void VFDWindow::initConnect()
{
    /*connect(ui->lineEdit, &QLineEdit::textChanged,
            this, [=](const QString &text){
        sorting_Station = text;
    });
    connect(ui->lineEdit_2, &QLineEdit::textChanged,
            this, [=](const QString &text){
        sorting_velocity = text;
    });
    connect(ui->lineEdit_3, &QLineEdit::textChanged,
            this, [=](const QString &text){
        sorting_direction = text;
    });



    connect(ui->lineEdit_4, &QLineEdit::textChanged,
            this, [=](const QString &text){
        loading_Station = text;
    });
    connect(ui->lineEdit_5, &QLineEdit::textChanged,
            this, [=](const QString &text){
        loading_velocity = text;
    });
    connect(ui->lineEdit_6, &QLineEdit::textChanged,
            this, [=](const QString &text){
        loading_direction = text;
    });



    connect(ui->lineEdit_7, &QLineEdit::textChanged,
            this, [=](const QString &text){
        lift_Station = text;
    });
    connect(ui->lineEdit_8, &QLineEdit::textChanged,
            this, [=](const QString &text){
        lift_velocity = text;
    });
    connect(ui->lineEdit_9, &QLineEdit::textChanged,
            this, [=](const QString &text){
        lift_direction = text;
    });*/
}


int VFDWindow::initSerial() {
    int station1=sorting_Station.toInt(), station2=loading_Station.toInt(), station3=lift_Station.toInt();
    //loadSerialIni(1);
    for (int i=0;i<12;i++) {
        int handle=-1;
        auto ret=ProfinetManaged::modbus_rtu_create(serial.toStdString().c_str(), baud_rate.toInt(), station1, data_bits.toInt(),(ProfinetManaged::StopBits)stop_bits.toInt(),  (ProfinetManaged::Parity)parity.toInt(), &handle);
        if (ret.isSuccess==0){
            serial_Map.insert(station1,handle);
            station1++;
        }else {
            QMessageBox::warning(nullptr,"连接失败",QString("串口站号 %1 连接失败，%2").arg(station1).arg(QString::fromStdString(ret.message)));
            return -1;
        }
    }
    //loadSerialIni(3);
        for (int i=0;i<2;i++) {
            int handle=-1;
            auto ret=ProfinetManaged::modbus_rtu_create(serial.toStdString().c_str(), baud_rate.toInt(), station2, data_bits.toInt(),(ProfinetManaged::StopBits)stop_bits.toInt(),  (ProfinetManaged::Parity)parity.toInt(), &handle);
            if (ret.isSuccess==0){
                serial_Map.insert(station2,handle);
                station2++;
            }else {
                QMessageBox::warning(nullptr,"连接失败",QString("串口站号 %1 连接失败，%2").arg(station2).arg(QString::fromStdString(ret.message)));
                return -1;
            }
        }
    //loadSerialIni(2);
        for (int i=0;i<12;i++) {
            int handle=-1;
            auto ret=ProfinetManaged::modbus_rtu_create(serial.toStdString().c_str(), baud_rate.toInt(), station3, data_bits.toInt(),(ProfinetManaged::StopBits)stop_bits.toInt(),  (ProfinetManaged::Parity)parity.toInt(), &handle);
            if (ret.isSuccess==0){
                serial_Map.insert(station3,handle);
                station3++;
            }else {
                QMessageBox::warning(nullptr,"连接失败",QString("串口站号 %1 连接失败，%2").arg(station3).arg(QString::fromStdString(ret.message)));
                return -1;
            }
        }
    serialConnect=1;
    return 0;

}



void VFDWindow::initSerialParameter() {
    int station1=sorting_Station.toInt(), station2=loading_Station.toInt(), station3=lift_Station.toInt();
    for (int i=0;i<12;i++) {
        int handle=-1;
        uint16_t speed=sorting_velocity.toUInt();
        uint16_t direction=sorting_direction.toUInt();
        if (serial_Map.contains(station1)) {
            handle = serial_Map.value(station1);
        } else {
            QMessageBox::warning(nullptr, "错误", QString("串口未找到站号 %1 对应的句柄").arg(station1));
            return;
        }

        auto retDir = ProfinetManaged::profinet_write_uint16(handle, directionAddr, &direction, 1);
        if (retDir.isSuccess!=0) {
            QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 方向写入%2失败，%3").arg(station1).arg(directionAddr).arg(QString::fromStdString(retDir.message)));
            return;
        }

        auto retSpeed = ProfinetManaged::profinet_write_uint16(handle, speedAddr, &speed, 1);
        if (retSpeed.isSuccess!=0) {
            QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 速度写入%2失败，%3").arg(station1).arg(speedAddr).arg(QString::fromStdString(retSpeed.message)));
            return;
        }

        station1++;
    }



    for (int i=0;i<2;i++) {
        int handle=-1;
        uint16_t speed=loading_velocity.toUInt();
        uint16_t direction=loading_direction.toUInt();
        if (serial_Map.contains(station2)) {
            handle = serial_Map.value(station2);
        } else {
            QMessageBox::warning(nullptr, "错误", QString("串口未找到站号 %1 对应的句柄").arg(station2));
            return;
        }

        auto retDir = ProfinetManaged::profinet_write_uint16(handle, directionAddr, &direction, 1);
        if (retDir.isSuccess!=0) {
            QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 方向写入%2失败，%3").arg(station2).arg(directionAddr).arg(QString::fromStdString(retDir.message)));
            return;
        }

        auto retSpeed = ProfinetManaged::profinet_write_uint16(handle, speedAddr, &speed, 1);
        if (retSpeed.isSuccess!=0) {
            QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 速度写入%2失败，%3").arg(station2).arg(speedAddr).arg(QString::fromStdString(retSpeed.message)));
            return;
        }

        station2++;
    }



    for (int i=0;i<12;i++) {
        int handle=-1;
        uint16_t speed=lift_velocity.toUInt();
        uint16_t direction=lift_direction.toUInt();
        if (serial_Map.contains(station3)) {
            handle = serial_Map.value(station3);
        } else {
            QMessageBox::warning(nullptr, "错误", QString("串口未找到站号 %1 对应的句柄").arg(station3));
            return;
        }

        auto retDir = ProfinetManaged::profinet_write_uint16(handle, directionAddr, &direction, 1);
        if (retDir.isSuccess!=0) {
            QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 方向写入%2失败，%3").arg(station3).arg(directionAddr).arg(QString::fromStdString(retDir.message)));
            return;
        }

        auto retSpeed = ProfinetManaged::profinet_write_uint16(handle, speedAddr, &speed, 1);
        if (retSpeed.isSuccess!=0) {
            QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 速度写入%2失败，%3").arg(station3).arg(speedAddr).arg(QString::fromStdString(retSpeed.message)));
            return;
        }

        station3++;
    }

}


void VFDWindow::closeSerial() {

int station1=sorting_Station.toInt(), station2=loading_Station.toInt(), station3=lift_Station.toInt();
    for (int i=0;i<12;i++) {
        int handle=-1;
        uint16_t direction=0;
        if (serial_Map.contains(station1)) {
            handle = serial_Map.value(station1);
        } else {
            QMessageBox::warning(nullptr, "错误", QString("串口未找到站号 %1 对应的句柄").arg(station1));
            return;
        }


        station1++;
    }



    for (int i=0;i<2;i++) {
        int handle=-1;
        uint16_t direction=0;
        if (serial_Map.contains(station2)) {
            handle = serial_Map.value(station2);
        } else {
            QMessageBox::warning(nullptr, "错误", QString("串口未找到站号 %1 对应的句柄").arg(station2));
            return;
        }

        auto retDir = ProfinetManaged::profinet_write_uint16(handle, directionAddr, &direction, 1);
        if (retDir.isSuccess!=0) {
            QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 关闭失败%2").arg(station2).arg(directionAddr).arg(QString::fromStdString(retDir.message)));
            return;
        }

        station2++;
    }



    for (int i=0;i<12;i++) {
        int handle=-1;
        uint16_t direction=0;
        if (serial_Map.contains(station3)) {
            handle = serial_Map.value(station3);
        } else {
            QMessageBox::warning(nullptr, "错误", QString("串口未找到站号 %1 对应的句柄").arg(station3));
            return;
        }

        auto retDir = ProfinetManaged::profinet_write_uint16(handle, directionAddr, &direction, 1);
        if (retDir.isSuccess!=0) {
            QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 关闭失败%2").arg(station3).arg(directionAddr).arg(QString::fromStdString(retDir.message)));
            return;
        }

        station3++;
    }
}


void VFDWindow::on_pushButton2_start() {
    /*int handle=-1;
    uint16_t speed=ui->lineEdit_2->text().toUShort();
    uint16_t direction=ui->lineEdit_3->text().toUShort();
    int station=ui->lineEdit->text().toInt();
    uint16_t command=1;
    if (serial_Map.contains(station)) {
        handle = serial_Map.value(station);
    } else {
        QMessageBox::warning(nullptr, "错误", QString("串口未找到站号 %1 对应的句柄").arg(station));
        return;
    }

    auto retDir = ProfinetManaged::profinet_write_uint16(handle, directionAddr, &direction, 1);
    if (retDir.isSuccess!=0) {
        QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 方向写入%2失败，%3").arg(station).arg(directionAddr).arg(QString::fromStdString(retDir.message)));
        return;
    }

    auto retSpeed = ProfinetManaged::profinet_write_uint16(handle, speedAddr, &speed, 1);
    if (retSpeed.isSuccess!=0) {
        QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 速度写入%2失败，%3").arg(station).arg(speedAddr).arg(QString::fromStdString(retSpeed.message)));
        return;
    }

    auto retStart = ProfinetManaged::profinet_write_uint16(handle, startAddr, &command, 1);
    if (retSpeed.isSuccess!=0) {
        QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 启动失败，%2").arg(station).arg(QString::fromStdString(retSpeed.message)));
        return;
    }*/

}


void VFDWindow::on_pushButton2_stop() {
    /*int handle=-1;
    uint16_t speed=ui->lineEdit_2->text().toUShort();
    uint16_t direction=ui->lineEdit_3->text().toUShort();
    int station=ui->lineEdit->text().toInt();
    uint16_t command=0;
    if (serial_Map.contains(station)) {
        handle = serial_Map.value(station);
    } else {
        QMessageBox::warning(nullptr, "错误", QString("串口未找到站号 %1 对应的句柄").arg(station));
        return;
    }

    auto retDir = ProfinetManaged::profinet_write_uint16(handle, directionAddr, &direction, 1);
    if (retDir.isSuccess!=0) {
        QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 方向写入%2失败，%3").arg(station).arg(directionAddr).arg(QString::fromStdString(retDir.message)));
        return;
    }

    auto retSpeed = ProfinetManaged::profinet_write_uint16(handle, speedAddr, &speed, 1);
    if (retSpeed.isSuccess!=0) {
        QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 速度写入%2失败，%3").arg(station).arg(speedAddr).arg(QString::fromStdString(retSpeed.message)));
        return;
    }

    auto retStart = ProfinetManaged::profinet_write_uint16(handle, startAddr, &command, 1);
    if (retSpeed.isSuccess!=0) {
        QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 启动失败，%2").arg(station).arg(QString::fromStdString(retSpeed.message)));
        return;
    }
    */

}





void VFDWindow::on_pushButton3_start() {
    /*int handle=-1;
    uint16_t speed=ui->lineEdit_11->text().toUShort();
    uint16_t direction=ui->lineEdit_12->text().toUShort();
    int station=ui->lineEdit_10->text().toInt();
    uint16_t command=1;
    if (serial_Map.contains(station)) {
        handle = serial_Map.value(station);
    } else {
        QMessageBox::warning(nullptr, "错误", QString("串口未找到站号 %1 对应的句柄").arg(station));
        return;
    }

    auto retDir = ProfinetManaged::profinet_write_uint16(handle, directionAddr, &direction, 1);
    if (retDir.isSuccess!=0) {
        QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 方向写入%2失败，%3").arg(station).arg(directionAddr).arg(QString::fromStdString(retDir.message)));
        return;
    }

    auto retSpeed = ProfinetManaged::profinet_write_uint16(handle, speedAddr, &speed, 1);
    if (retSpeed.isSuccess!=0) {
        QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 速度写入%2失败，%3").arg(station).arg(speedAddr).arg(QString::fromStdString(retSpeed.message)));
        return;
    }

    auto retStart = ProfinetManaged::profinet_write_uint16(handle, startAddr, &command, 1);
    if (retSpeed.isSuccess!=0) {
        QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 启动失败，%2").arg(station).arg(QString::fromStdString(retSpeed.message)));
        return;
    }*/

}


void VFDWindow::on_pushButton3_stop() {
    /*int handle=-1;
    uint16_t speed=ui->lineEdit_11->text().toUShort();
    uint16_t direction=ui->lineEdit_12->text().toUShort();
    int station=ui->lineEdit_10->text().toInt();
    uint16_t command=0;
    if (serial_Map.contains(station)) {
        handle = serial_Map.value(station);
    } else {
        QMessageBox::warning(nullptr, "错误", QString("串口未找到站号 %1 对应的句柄").arg(station));
        return;
    }

    auto retDir = ProfinetManaged::profinet_write_uint16(handle, directionAddr, &direction, 1);
    if (retDir.isSuccess!=0) {
        QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 方向写入%2失败，%3").arg(station).arg(directionAddr).arg(QString::fromStdString(retDir.message)));
        return;
    }

    auto retSpeed = ProfinetManaged::profinet_write_uint16(handle, speedAddr, &speed, 1);
    if (retSpeed.isSuccess!=0) {
        QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 速度写入%2失败，%3").arg(station).arg(speedAddr).arg(QString::fromStdString(retSpeed.message)));
        return;
    }

    auto retStart = ProfinetManaged::profinet_write_uint16(handle, startAddr, &command, 1);
    if (retSpeed.isSuccess!=0) {
        QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 启动失败，%2").arg(station).arg(QString::fromStdString(retSpeed.message)));
        return;
    }*/

}

int VFDWindow::createSerial(int station) {
    int handle=-1;
    auto ret=ProfinetManaged::modbus_rtu_create(serial.toStdString().c_str(), baud_rate.toInt(), station, data_bits.toInt(),(ProfinetManaged::StopBits)stop_bits.toInt(),  (ProfinetManaged::Parity)parity.toInt(), &handle);
    if (ret.isSuccess==0){
        serial_Map.insert(station,handle);
        Handle=handle;
    }else {
        QMessageBox::warning(nullptr,"连接失败",QString("站号 %1 连接失败，%2").arg(station).arg(QString::fromStdString(ret.message)));
        return -1;
    }
    return 0;
}



int VFDWindow::initSerial2() {

        int station=serialParame[1][0].toInt();
        int handle=-1;
        auto ret=ProfinetManaged::modbus_rtu_create(serial.toStdString().c_str(), baud_rate.toInt(), station, data_bits.toInt(),(ProfinetManaged::StopBits)stop_bits.toInt(),  (ProfinetManaged::Parity)parity.toInt(), &handle);
        if (ret.isSuccess==0){
            serial_Map.insert(station,handle);
            Handle=handle;
            vfdConnect=1;
        }else {
            QMessageBox::warning(nullptr,"连接失败",QString("串口站号 %1 连接失败，%2").arg(station).arg(QString::fromStdString(ret.message)));
            return -1;
        }
        return 0;
    }


int VFDWindow::initSerialStop() {
    if (ui->pushButton_2->text()=="停止")
    {
        ui->pushButton_2->click();
       //serialWrite(0);
    }
    if (ui->pushButton_3->text()=="停止")
    {
        ui->pushButton_3->click();
        //serialWrite(1);
    }
    if (ui->pushButton_6->text()=="停止")
    {
        ui->pushButton_6->click();
        //serialWrite(2);
    }
    if (ui->pushButton_7->text()=="停止")
    {
        ui->pushButton_7->click();
        //serialWrite(3);
    }
    if (ui->pushButton_8->text()=="停止")
    {
        ui->pushButton_8->click();
        //serialWrite(4);
    }
    if (ui->pushButton_4->text()=="停止")
    {
        ui->pushButton_4->click();
        //serialWrite(5);
    }
    if (ui->pushButton_5->text()=="停止")
    {
        ui->pushButton_5->click();
        //serialWrite(6);
    }
    if (ui->pushButton_11->text()=="停止")
    {
        ui->pushButton_11->click();
        //serialWrite(6);
    }

return 0;

}


int VFDWindow::initSerial3() {
    if (ui->checkBox->isChecked() && ui->pushButton_2->text()=="启动")
    {
        /*ui->pushButton_2->setText("启动");
        serialWrite(0);*/
        ui->pushButton_2->click();
        //serialStop(0);
       Sleep(100);
    }

    if (ui->checkBox_2->isChecked() && ui->pushButton_3->text()=="启动")
    {
        /*ui->pushButton_3->setText("启动");
        serialWrite(1);*/
        ui->pushButton_3->click();
        //serialStop(1);
        Sleep(100);
    }

    if (ui->checkBox_3->isChecked() && ui->pushButton_6->text()=="启动")
    {
        /*ui->pushButton_6->setText("启动");
        serialWrite(2);*/
        ui->pushButton_6->click();
        //serialStop(2);
       Sleep(100);
    }

    if (ui->checkBox_4->isChecked() && ui->pushButton_7->text()=="启动")
    {
        /*ui->pushButton_7->setText("启动");
        serialWrite(3);*/
        ui->pushButton_7->click();
        //serialStop(3);
       Sleep(100);
    }

    if (ui->checkBox_5->isChecked() && ui->pushButton_8->text()=="启动")
    {
        /*ui->pushButton_8->setText("启动");
        serialWrite(4);*/
        ui->pushButton_8->click();
        //serialStop(4);
        Sleep(100);
    }

    if (ui->checkBox_6->isChecked() && ui->pushButton_4->text()=="启动")
    {
        /*ui->pushButton_4->setText("启动");
        serialWrite(5);*/
        ui->pushButton_4->click();
        //serialStop(5);
        Sleep(100);
    }

    if (ui->checkBox_7->isChecked() && ui->pushButton_5->text()=="启动")
    {
        /*ui->pushButton_5->setText("启动");
        serialWrite(6);*/
        ui->pushButton_5->click();
        //serialStop(6);
       Sleep(100);
    }

    if (ui->checkBox_8->isChecked() && ui->pushButton_11->text()=="启动")
    {
        /*ui->pushButton_11->setText("启动");
        serialWrite(7);*/
        ui->pushButton_11->click();
        //serialStop(7);
        Sleep(100);
    }

    return 0;

}

//*******************************************************************************************************
void VFDWindow::saveConfig() {
    QFileInfo fileInfo(pathFile);
    QDir dir = fileInfo.dir();

    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            return;
        }
    }

    QSettings settings(pathFile, QSettings::IniFormat);
    settings.beginGroup("serialParame");

    for (int i = 0; i < serialParame.size(); ++i) {
        QString key = QString("station%1").arg(i + 1);
        QString value = serialParame[i].join(",");
        settings.setValue(key, value);
    }

    settings.setValue("vfdstart",vfdstart.join(","));
    settings.endGroup();


}



void VFDWindow::loadSerialParame() {
    QSettings settings(pathFile, QSettings::IniFormat);
    settings.beginGroup("serialParame");

    for (int i = 1; i <= 8; ++i) {
        QString key = QString("station%1").arg(i);
        QString value = settings.value(key).toString();

        QStringList params;
        if (!value.isEmpty()) {
            QStringList tempList = value.split(",");
            for (const QString &str : tempList) {
                if (!str.isEmpty()) {
                    params << str;
                }
            }
        }

        // 确保params长度为3，不够补空字符串
        while (params.size() < 3) {
            params << "";
        }

        serialParame[i - 1] = params;  // 直接赋值，不用append
    }

    QString vfdstartStr = settings.value("vfdstart", "0,0,0,0,0,0,0,0").toString();
     vfdstart = vfdstartStr.split(",");


    settings.endGroup();

    ui->lineEdit->setText(serialParame[0][0]);
    ui->comboBox->setCurrentIndex(serialParame[0][1].toInt());
    ui->comboBox_8->setCurrentIndex(serialParame[0][2].toInt());

    ui->lineEdit_10->setText(serialParame[1][0]);
    ui->comboBox_2->setCurrentIndex(serialParame[1][1].toInt());
    ui->comboBox_9->setCurrentIndex(serialParame[1][2].toInt());

    ui->lineEdit_4->setText(serialParame[2][0]);
    ui->comboBox_3->setCurrentIndex(serialParame[2][1].toInt());
    ui->comboBox_10->setCurrentIndex(serialParame[2][2].toInt());

    ui->lineEdit_7->setText(serialParame[3][0]);
    ui->comboBox_4->setCurrentIndex(serialParame[3][1].toInt());
    ui->comboBox_11->setCurrentIndex(serialParame[3][2].toInt());

    ui->lineEdit_21->setText(serialParame[4][0]);
    ui->comboBox_5->setCurrentIndex(serialParame[4][1].toInt());
    ui->comboBox_12->setCurrentIndex(serialParame[4][2].toInt());

    ui->lineEdit_13->setText(serialParame[5][0]);
    ui->comboBox_6->setCurrentIndex(serialParame[5][1].toInt());
    ui->comboBox_13->setCurrentIndex(serialParame[5][2].toInt());

    ui->lineEdit_16->setText(serialParame[6][0]);
    ui->comboBox_7->setCurrentIndex(serialParame[6][1].toInt());
    ui->comboBox_14->setCurrentIndex(serialParame[6][2].toInt());

    ui->lineEdit_17->setText(serialParame[7][0]);
    ui->comboBox_16->setCurrentIndex(serialParame[7][1].toInt());
    ui->comboBox_15->setCurrentIndex(serialParame[7][2].toInt());

    ui->checkBox->setChecked(vfdstart[0] == "1");
    ui->checkBox_2->setChecked(vfdstart[1] == "1");
    ui->checkBox_3->setChecked(vfdstart[2] == "1");
    ui->checkBox_4->setChecked(vfdstart[3] == "1");
    ui->checkBox_5->setChecked(vfdstart[4] == "1");
    ui->checkBox_6->setChecked(vfdstart[5] == "1");
    ui->checkBox_7->setChecked(vfdstart[6] == "1");
    ui->checkBox_8->setChecked(vfdstart[7] == "1");
}

void VFDWindow::initConect()
{
    connect(ui->lineEdit, &QLineEdit::textChanged, this, [this](const QString &text){
    serialParame[0][0] = text;
    });
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        serialParame[0][1] = QString::number(index);
    });
    connect(ui->comboBox_8, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        serialParame[0][2] = QString::number(index);
    });




    connect(ui->lineEdit_10, &QLineEdit::textChanged, this, [this](const QString &text){
   serialParame[1][0] = text;
   });
    connect(ui->comboBox_2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        serialParame[1][1] = QString::number(index);
    });
    connect(ui->comboBox_9, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
       serialParame[1][2] = QString::number(index);
   });




    connect(ui->lineEdit_4, &QLineEdit::textChanged, this, [this](const QString &text){
   serialParame[2][0] = text;
   });
    connect(ui->comboBox_3, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        serialParame[2][1] = QString::number(index);
    });
    connect(ui->comboBox_10, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        serialParame[2][2] = QString::number(index);
    });





    connect(ui->lineEdit_7, &QLineEdit::textChanged, this, [this](const QString &text){
   serialParame[3][0] = text;
   });
    connect(ui->comboBox_4, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        serialParame[3][1] = QString::number(index);
    });
    connect(ui->comboBox_11, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
       serialParame[3][2] = QString::number(index);
   });





    connect(ui->lineEdit_21, &QLineEdit::textChanged, this, [this](const QString &text){
   serialParame[4][0] = text;
   });
    connect(ui->comboBox_5, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        serialParame[4][1] = QString::number(index);
    });
    connect(ui->comboBox_12, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
       serialParame[4][2] = QString::number(index);
   });





    connect(ui->lineEdit_13, &QLineEdit::textChanged, this, [this](const QString &text){
   serialParame[5][0] = text;
   });
    connect(ui->comboBox_6, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        serialParame[5][1] = QString::number(index);
    });
    connect(ui->comboBox_13, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        serialParame[5][2] = QString::number(index);
    });





    connect(ui->lineEdit_16, &QLineEdit::textChanged, this, [this](const QString &text){
   serialParame[6][0] = text;
   });
    connect(ui->comboBox_7, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        serialParame[6][1] = QString::number(index);
    });
    connect(ui->comboBox_14, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
       serialParame[6][2] = QString::number(index);
   });


    connect(ui->lineEdit_17, &QLineEdit::textChanged, this, [this](const QString &text){
  serialParame[7][0] = text;
  });
    connect(ui->comboBox_16, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        serialParame[7][1] = QString::number(index);
    });
    connect(ui->comboBox_15, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
       serialParame[7][2] = QString::number(index);
   });


    connect(ui->checkBox, &QCheckBox::toggled, this, [=](bool checked){
   if (checked) {
       vfdstart[0]="1";
   } else {
       vfdstart[0]="0";
   }
   });

    connect(ui->checkBox_2, &QCheckBox::toggled, this, [=](bool checked){
        if (checked) {
        vfdstart[1]="1";
    } else {
        vfdstart[1]="0";
    }
    });

    connect(ui->checkBox_3, &QCheckBox::toggled, this, [=](bool checked){
       if (checked) {
       vfdstart[2]="1";
   } else {
       vfdstart[2]="0";
   }
   });

    connect(ui->checkBox_4, &QCheckBox::toggled, this, [=](bool checked){
       if (checked) {
       vfdstart[3]="1";
   } else {
       vfdstart[3]="0";
   }
   });

    connect(ui->checkBox_5, &QCheckBox::toggled, this, [=](bool checked){
       if (checked) {
       vfdstart[4]="1";
   } else {
       vfdstart[4]="0";
   }
   });

    connect(ui->checkBox_6, &QCheckBox::toggled, this, [=](bool checked){
       if (checked) {
       vfdstart[5]="1";
   } else {
       vfdstart[5]="0";
   }
   });

    connect(ui->checkBox_7, &QCheckBox::toggled, this, [=](bool checked){
       if (checked) {
       vfdstart[6]="1";
   } else {
       vfdstart[6]="0";
   }
   });

    connect(ui->checkBox_8, &QCheckBox::toggled, this, [=](bool checked){
      if (checked) {
      vfdstart[7]="1";
  } else {
      vfdstart[7]="0";
  }
  });


}


int VFDWindow::serialWriteSpeed(int handle,int station,uint16_t speed)
{
    Result<int> retSpeed = ProfinetManaged::profinet_write_uint16(handle, speedAddr, &speed, 1);

    Sleep(100);

    if (retSpeed.isSuccess!=0) {
        writeLog(QString("串口站号 %1 速度%2写入失败，%3").arg(station).arg(speed).arg(QString::fromStdString(retSpeed.message)));
        //QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 速度%2写入失败，%3").arg(serialParame[station][0]).arg(speed).arg(QString::fromStdString(retSpeed.message)));
        return -1;
    }
    writeLog(QString("串口站号 %1 速度%2写入成功").arg(station).arg(speed));
    return 0;
}


int VFDWindow::serialWriteDirection(int handle,int station,uint16_t direction)
{
    auto retDir = ProfinetManaged::profinet_write_uint16(handle, directionAddr, &direction, 1);

    Sleep(100);

    if (retDir.isSuccess!=0) {
        writeLog(QString("串口站号 %1 方向%2写入失败，%3").arg(station).arg(direction).arg(QString::fromStdString(retDir.message)));
        //QMessageBox::warning(nullptr, "错误", QString("串口站号 %1 方向写入失败，%2").arg(serialParame[station][0]).arg(QString::fromStdString(retDir.message)));
        return -1;
    }
    writeLog(QString("串口站号 %1 方向%2写入成功").arg(station).arg(direction));
    return 0;
}


void VFDWindow::serialWrite(int index)
{

    int handle=-1;
    uint16_t direction;
    uint16_t speed;
    int station=serialParame[index][0].toInt();

    if (Handle==-1) {
        //QMessageBox::warning(nullptr, "错误", QString("串口未找到句柄"));
        writeLog(QString("串口站号 %1 串口未找到句柄").arg(serialParame[index][0]));
        return;
    }
        handle = Handle;

   auto ret= ProfinetManaged::modify_modbus_rtu_station(handle,station);
    if (ret.isSuccess!=0)
    {
        //QMessageBox::warning(nullptr, "错误", QString("站号修改失败"));
        writeLog(QString("串口站号 %1 修改失败").arg(serialParame[index][0]));
        return;
    }
    Sleep(100);
    int ren=serialWriteDirection(handle,station,1150);
    if (ren!=0) return;
    writeLog(QString("写入停止"));
    Sleep(100);

    if (serialParame[index][1].toInt()==0)
        speed=1536;
    else if (serialParame[index][1].toInt()==1)
        speed=1836;
    else if (serialParame[index][1].toInt()==2)
        speed=8192;
    else if (serialParame[index][1].toInt()==3)
    {
        speed=12280;
    }
    else
        speed=16384;

    if (serialParame[index][2].toInt()==0)
        direction=3199;
    else direction=1151;

     ren =serialWriteSpeed(handle,station,speed);
    if (ren==0)
    {
        Sleep(100);
        serialWriteDirection(handle,station,direction);
    }

}

void VFDWindow::serialWrite10(int index)
{

    int handle=-1;
    uint16_t direction;
    uint16_t speed;
    int station=serialParame[index][0].toInt();

    if (Handle==-1) {
        //QMessageBox::warning(nullptr, "错误", QString("串口未找到句柄"));
        writeLog(QString("串口站号 %1 串口未找到句柄").arg(serialParame[index][0]));
        return;
    }
    handle = Handle;

    auto ret= ProfinetManaged::modify_modbus_rtu_station(handle,station);
    if (ret.isSuccess!=0)
    {
        //QMessageBox::warning(nullptr, "错误", QString("站号修改失败"));
        writeLog(QString("串口站号 %1 修改失败").arg(serialParame[index][0]));
        return;
    }

    if (serialParame[index][1].toInt()==0)
        speed=1280;
    else if (serialParame[index][1].toInt()==1)
        speed=4096;

    else if (serialParame[index][1].toInt()==2)
        speed=8192;
    else
        speed=16384;

    if (serialParame[index][2].toInt()==0)
        direction=3199;
    else direction=1151;

    int ren =serialWriteSpeed(handle,station,speed);
    if (ren==0)
    {
        Sleep(100);
        writeLog("写入速度");
        serialWriteDirection(handle,station,direction);
    }

}


void VFDWindow::serialStop(int index)
{

    int handle = -1;
    uint16_t direction = 1150;
    int station=serialParame[index][0].toInt();

    if (Handle==-1) {
        //QMessageBox::warning(nullptr, "错误", QString("串口未找到句柄"));
        writeLog(QString("串口站号 %1 串口未找到句柄").arg(serialParame[station][0]));
        return;
    } else {
        handle = Handle;
    }

    auto ret= ProfinetManaged::modify_modbus_rtu_station(handle,station);

    Sleep(100);

    if (ret.isSuccess!=0)
    {
        //QMessageBox::warning(nullptr, "错误", QString("站号修改失败"));
        writeLog(QString("串口站号 %1 修改失败").arg(serialParame[station][0]));
        return;
    }

    serialWriteDirection(handle,station,direction);
}


void VFDWindow::connectMotor(int index)
{
    if (Handle!=-1)
    {
        bool ret_remove=ProfinetManaged::remove(Handle);
        if (ret_remove)Handle=-1;
        else
        {
            writeLog("移除句柄失败");
            return;
        }
    }

    int handle=-1;
    auto ret_create=ProfinetManaged::modbus_rtu_create(serial.toStdString().c_str(), baud_rate.toInt(), 1, data_bits.toInt(),(ProfinetManaged::StopBits)stop_bits.toInt(),  (ProfinetManaged::Parity)parity.toInt(), &handle);
    if (ret_create.isSuccess==0){
        serial_Map.insert(handle,1);
        Handle=handle;
    }else {
        QMessageBox::warning(nullptr,"连接失败",QString("连接电机失败，%1").arg(QString::fromStdString(ret_create.message)));
        writeLog(QString("连接电机失败，%1").arg(QString::fromStdString(ret_create.message)));
    }
}



void VFDWindow::clearMaterials()
{
    serialStop(serialParame[2][0].toInt());
    serialStop(serialParame[3][0].toInt());

}

int VFDWindow::comState()
{
    return 0;

}

void VFDWindow::writeLog(const QString &logText) {

    QString currentDate = QDate::currentDate().toString("yyyyMMdd");
    QString dirPath = QString("D:/Data/Log/%1").arg(currentDate);
    QDir dir(dirPath);

    if (!dir.exists()) {
        if (!QDir().mkpath(dirPath)) {
            return;
        }
    }

    QString filePath = dir.filePath(QString("log-%1.log").arg(currentDate));

    QFile file(filePath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    out << "[" << time << "] " << logText << endl;
    file.close();
}
