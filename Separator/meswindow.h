//
// Created by Adminstrator on 25-7-27.
//

#ifndef MESWINDOW_H
#define MESWINDOW_H

#include <QWidget>
#include "VpMainWindow.h"
#include <QDir>
#include <QSettings>
#include <QDate>
#include <QMessageBox>
#include <QTextStream>
#include "ProfinetManaged.h"
#include "boardcard.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MESWindow; }
QT_END_NAMESPACE

class MESWindow : public VpMainWindow {
Q_OBJECT

public:
    explicit MESWindow(QWidget *parent = nullptr);
    ~MESWindow() override;

    void savePageToIni();
    void loadIniToPage();
    void saveParameter();
    void loadParameter();
    void writeLog(const QString &logText);
    void startComThread();
    void stopComThread();
    void comListen();
    void readListen();
    void boarListen();
    void createComConnect();
    void connectParameter();
    void initConnect();
    void startBoarThread();

    void startReadThread();
    void stopReadThread();
    void stopBoarThread();


    int handle=-1;
    int m_ProtocolType=3;
    int m_PlcType = 1;
    int m_DataFormat = 0;
    BoardCard* m_BoardCard;
    QString m_add="1000";
    QString IP="192.168.0.1";
    QString Port="8080";
    QString IP_Mes="192.168.0.1";
    QString Port_Mes="8080";
    QStringList Parameter= QStringList()<<"0"<<"0"<<"0"<<"0"<<"0"<<"0";
    QString pathFile="C:/vpconfig/Config.ini";

    QThread *comThread=nullptr;
    QThread *listenThread=nullptr;
    QThread *boarThread=nullptr;

private:
    Ui::MESWindow *ui;
};


#endif //MESWINDOW_H
