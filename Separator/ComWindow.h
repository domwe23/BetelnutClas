#ifndef COMWINDOW_H
#define COMWINDOW_H

#include <QWidget>
#include <QSerialPort>
#include <QTcpSocket>
#include <QTcpServer>
#include "VpMainWindow.h"
#include <QDir>

#include "ProfinetManaged.h"


QT_BEGIN_NAMESPACE
namespace Ui { class ComWindow; }
QT_END_NAMESPACE

class ComWindow : public VpMainWindow
{
Q_OBJECT

public:
    explicit ComWindow(QWidget *parent = nullptr);
    ~ComWindow() override;

private slots:
    // 串口通信相关槽函数
    void onSerialConnectClicked(); //原连接函数
    void onSerialConnectClicked2();
    void handleSerialReadyRead();
    void handleSerialError(QSerialPort::SerialPortError error);

    // 网络通信相关槽函数
    void onNetworkConnectClicked();
    void handleNewConnection();
    void handleNetworkReadyRead();
    void handleNetworkError(QAbstractSocket::SocketError socketError);
    void attemptReconnect();

    // 数据操作相关槽函数
    void onSendClicked();//原发送数据
    void onSendClicked2();
    void onClearReceiveDataClicked();
    void onClearSendDataClicked();
    void onTcpTypeChanged(int index);

private:
    // UI相关
    Ui::ComWindow *ui;

    // 串口通信相关成员
    QSerialPort *m_serialPort;
    bool m_serialConnected;

    // 网络通信相关成员
    QTcpSocket *m_tcpSocket;
    QTcpServer *m_tcpServer;
    bool m_networkConnected;
    bool m_isTcpServer;
    int m_reconnectAttempts;  // 重连尝试次数
    QString m_lastRemoteIp;   // 最后连接的远程IP
    quint16 m_lastRemotePort; // 最后连接的远程端口


    QString serial="COM4";
    QString baud_rate="9600";
    QString data_bits="8";
    QString stop_bits="1";
    QString parity="0";
    QString station="1";
    int handle=-1;
    const char* directionAddr = "40001";

    // 初始化函数
    void initSerialPortSettings();
    void initNetworkSettings();
    void initparameter();

    // 日志记录函数
    void appendToLog(const QString &message, bool isSend = false, bool isSerial = false);

    // 网络连接辅助函数
    void safeDisconnectNetwork();
    void startReconnectTimer();
    void stopReconnectTimer();
    bool isValidIpAddress(const QString &ip);
    QString formatIpAddress(const QHostAddress &address);
    QString getLocalNetworkIp();

    // IP历史记录管理
    void updateIpHistory(const QString &ip);
    void saveIpHistory();
    void loadIpHistory();
    void writeLog(const QString &logText);

    // 定时器用于重连
    QTimer *m_reconnectTimer;
};

#endif // COMWINDOW_H