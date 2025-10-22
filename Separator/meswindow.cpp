//
// Created by Adminstrator on 25-7-27.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MESWindow.h" resolved

#include "meswindow.h"

#include <QFileInfo>
#include <QThread>
#include <VpLog.h>

#include "ui_MESWindow.h"


MESWindow::MESWindow(QWidget *parent) :
    VpMainWindow(parent), ui(new Ui::MESWindow) {
    ui->setupUi(this);
    this->setWindowTitle("通讯设置");
    ui->verticalLayout->setStretch(0,1);
    ui->verticalLayout->setStretch(1,5);
    ui->verticalLayout->setStretch(2,1);

    this->setFixedSize(680, 440);

    connect(ui->pushButton, &QPushButton::clicked, this, &MESWindow::saveParameter);
    connect(ui->pushButton_con, &QPushButton::clicked, this, &MESWindow::connectParameter);
    loadParameter();

    m_BoardCard = new BoardCard();

}


MESWindow::~MESWindow() {
    delete ui;
}


void MESWindow::savePageToIni()
{
    QFileInfo fileInfo(pathFile);
    QDir dir=fileInfo.dir();

    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            return;
        }
    }

    QSettings settings(pathFile,QSettings::IniFormat);
    settings.beginGroup("com");
    settings.setValue("ip", IP);
    settings.setValue("port", Port);
    settings.setValue("ip_mes", IP_Mes);
    settings.setValue("port_mes", Port_Mes);
    settings.setValue("parameter", Parameter.join(","));
    settings.endGroup();
    settings.sync();
}

void MESWindow::loadIniToPage()
{
    QFile iniFile(pathFile);
    if (!iniFile.exists())
    {
        QMessageBox::warning(this, "错误", "配置文件 Config.ini 不存在！");
        writeLog("MES---配置文件 Config.ini 不存在！");
        return;
    }
    QSettings settings(pathFile,QSettings::IniFormat);
    settings.beginGroup("com");
    IP= settings.value("ip","192.168.0.1").toString();
    Port=settings.value("port" ,"502").toString();
    IP= settings.value("ip_mes","192.168.0.1").toString();
    Port=settings.value("port_mes" ,"502").toString();
    QString parameter=settings.value("parameter","0,0,0,0,0,0").toString();
    Parameter=parameter.split(',');
    settings.endGroup();
}


void MESWindow::saveParameter()
{
    IP=ui->lineEdit_ip->text();
    Port=ui->lineEdit_port->text();
    IP_Mes=ui->lineEdit_ip_mes->text();
    Port_Mes=ui->lineEdit_port_mes->text();
    if (ui->checkBox->isChecked()) Parameter[0]="1";
    else Parameter[0]="0";
    if (ui->checkBox_2->isChecked()) Parameter[1]="1";
    else Parameter[1]="0";
    if (ui->checkBox_3->isChecked()) Parameter[2]="1";
    else Parameter[2]="0";
    if (ui->checkBox_4->isChecked()) Parameter[3]="1";
    else Parameter[3]="0";
    if (ui->checkBox_5->isChecked()) Parameter[4]="1";
    else Parameter[4]="0";
    if (ui->checkBox_6->isChecked()) Parameter[5]="1";
    else Parameter[5]="0";
    savePageToIni();
}

void MESWindow::loadParameter()
{
    loadIniToPage();
    ui->lineEdit_ip->setText(IP);
    ui->lineEdit_port->setText(Port);
    ui->lineEdit_ip_mes->setText(IP_Mes);
    ui->lineEdit_port_mes->setText(Port_Mes);
    ui->checkBox->setChecked(Parameter[0].toInt());
    ui->checkBox_2->setChecked(Parameter[1].toInt());
    ui->checkBox_3->setChecked(Parameter[2].toInt());
    ui->checkBox_4->setChecked(Parameter[3].toInt());
    ui->checkBox_5->setChecked(Parameter[4].toInt());
    ui->checkBox_6->setChecked(Parameter[5].toInt());
}

void MESWindow::writeLog(const QString &logText) {

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



void MESWindow::startComThread() {
    comThread=QThread::create([this]() { });
    comThread->start();
}

void MESWindow::startBoarThread() {
    boarThread=QThread::create([this]() { });
    comThread->start();
}

void MESWindow::startReadThread() {
    listenThread=QThread::create([this]() { });
    listenThread->start();
}




void MESWindow::stopComThread() {
    if (comThread)
    {
        comThread->wait();
        delete comThread;
        comThread=nullptr;
    }
}

void MESWindow::stopReadThread() {
    if (listenThread)
    {
        listenThread->wait();
        delete listenThread;
        listenThread=nullptr;
    }
}

void MESWindow::stopBoarThread() {
    if (boarThread)
    {
        boarThread->wait();
        delete boarThread;
        boarThread=nullptr;
    }
}




void MESWindow::comListen() {

}

void MESWindow::readListen() {
    /*if (handle==-1) return;
    Result<ushort> ProfinetManaged::profinet_read_uint16(handle,m_add.toStdString().c_str(),1);*/

}

void MESWindow::boarListen() {

}

void MESWindow::createComConnect() {
    int handl;
    Result<int> ret= ProfinetManaged::profinet_create(IP.toStdString().c_str(),Port.toInt(),PROFINET_TYPE(3),&handl,1);
    if (ret.isSuccess!=0)
    {
        writeLog(QString("IP:%1,Port:%2创建连接失败").arg(IP).arg(Port));
        return;
    }
    handle=handl;
    writeLog(QString("IP:%1,Port:%2创建连接成功").arg(IP).arg(Port));
}


void MESWindow::initConnect()
{

}

void MESWindow::connectParameter() {
    if (handle != -1) {
        ProfinetManaged::profinet_free(handle);
        handle=-1;
    }

    ProfinetManaged::profinet_create(IP.toStdString().c_str(), Port.toInt(), PROFINET_TYPE(m_ProtocolType), &handle, m_PlcType);
    ProfinetManaged::profinet_setTimeout(handle, 300);
    if (m_ProtocolType != PROFINET_TYPE::SIEMENS) {
        ProfinetManaged::profinet_set_data_fromat(handle, Trans_Data_Format(m_DataFormat));
    }
}
