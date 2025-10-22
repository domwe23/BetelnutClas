#include "ComWindow.h"
#include "ui_ComWindow.h"
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QDateTime>
#include <QTimer>
#include <QSettings>
#include <QNetworkInterface>

ComWindow::ComWindow(QWidget *parent)
        : VpMainWindow(parent), ui(new Ui::ComWindow),
          m_serialPort(nullptr), m_serialConnected(false),
          m_tcpSocket(nullptr), m_tcpServer(nullptr),
          m_networkConnected(false), m_isTcpServer(false),
          m_reconnectAttempts(0), m_lastRemotePort(0),
          m_reconnectTimer(new QTimer(this))
{
    ui->setupUi(this);

    // 初始化UI状态
    ui->tabWidget->setCurrentIndex(0);
    this->setWindowTitle("通讯设置");
    // 初始化通信设置
    initSerialPortSettings();
    initNetworkSettings();

    this->setFixedSize(670, 600);

    // 加载历史IP
    loadIpHistory();

    // 连接信号槽
    connect(ui->cmbTcpType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ComWindow::onTcpTypeChanged);

    // 连接按钮信号槽
    connect(ui->btnOpenUart, &QPushButton::clicked, this, &ComWindow::onSerialConnectClicked2);
    connect(ui->btnTcpConnect, &QPushButton::clicked, this, &ComWindow::onNetworkConnectClicked);
    connect(ui->btnClearRcvData, &QPushButton::clicked, this, &ComWindow::onClearReceiveDataClicked);
    connect(ui->btnClearSendData, &QPushButton::clicked, this, &ComWindow::onClearSendDataClicked);
    connect(ui->btnSendSerialData, &QPushButton::clicked, this, &ComWindow::onSendClicked2);
    connect(ui->btnSendTcpData, &QPushButton::clicked, this, &ComWindow::onSendClicked);

    // 设置重连定时器
    connect(m_reconnectTimer, &QTimer::timeout, this, &ComWindow::attemptReconnect);
    m_reconnectTimer->setInterval(5000); // 5秒重连间隔

    initparameter();
}

ComWindow::~ComWindow()
{
    // 保存IP历史记录
    saveIpHistory();

    // 断开所有连接
    if(m_serialConnected) {
        m_serialPort->close();
        delete m_serialPort;
    }

    if(m_networkConnected) {
        safeDisconnectNetwork();
    }

    delete ui;
}

// ====================== 串口通信相关函数 ======================

void ComWindow::initSerialPortSettings()
{
    // 1. 初始化串口号
    ui->cmbPortNames->clear();
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    if(ports.isEmpty()) {
        ui->cmbPortNames->addItem("无可用串口");
    } else {
        for (const QSerialPortInfo &port : ports) {
            QString displayName = port.portName();
            if(!port.description().isEmpty())
                displayName += " (" + port.description() + ")";
            ui->cmbPortNames->addItem(displayName, port.portName());
        }
    }

    // 2. 初始化波特率
    ui->cmbBaude->clear();
    const QList<qint32> baudRates = {
            110, 300, 600, 1200, 2400, 4800, 9600,
            14400, 19200, 38400, 56000, 57600,
            115200, 128000, 256000, 460800, 921600
    };
    for (qint32 baud : baudRates) {
        ui->cmbBaude->addItem(QString::number(baud), baud);
    }
    ui->cmbBaude->setCurrentText("9600");

    // 3. 初始化数据位
    ui->cmbDataBit->clear();
    ui->cmbDataBit->addItem("5", QSerialPort::Data5);
    ui->cmbDataBit->addItem("6", QSerialPort::Data6);
    ui->cmbDataBit->addItem("7", QSerialPort::Data7);
    ui->cmbDataBit->addItem("8", QSerialPort::Data8);
    ui->cmbDataBit->setCurrentIndex(3);

    // 4. 初始化校验位
    ui->cmbParity->clear();
    ui->cmbParity->addItem("无", QSerialPort::NoParity);
    ui->cmbParity->addItem("奇校验", QSerialPort::OddParity);
    ui->cmbParity->addItem("偶校验", QSerialPort::EvenParity);
    ui->cmbParity->setCurrentIndex(0);

    // 5. 初始化停止位
    ui->cmbStopBit->clear();
    ui->cmbStopBit->addItem("1", QSerialPort::OneStop);
    ui->cmbStopBit->addItem("1.5", QSerialPort::OneAndHalfStop);
    ui->cmbStopBit->addItem("2", QSerialPort::TwoStop);
    ui->cmbStopBit->setCurrentIndex(0);

    // 6. 站号初始化
    ui->txtSerialStation->setText("1");
    ui->txtSerialStation->setValidator(new QIntValidator(0, 247, this));
}

void ComWindow::onSerialConnectClicked()
{
    if (m_serialConnected) {
        m_serialPort->close();
        delete m_serialPort;
        m_serialPort = nullptr;
        m_serialConnected = false;
        ui->btnOpenUart->setText("连接");
        appendToLog("串口已断开");
        return;
    }

    // 检查端口有效性
    QString displayText = ui->cmbPortNames->currentText();
    QString portName;
    QRegularExpression re("^(COM\\d+)");
    QRegularExpressionMatch match = re.match(displayText);
    if (match.hasMatch()) {
        portName = match.captured(1);
    }

    if(portName.isEmpty()) portName = "COM1";

    if(!QSerialPortInfo(portName).isValid()) {
        appendToLog(QString("错误：端口 %1 不存在或不可用").arg(portName));

        // 打印系统所有可用端口
        appendToLog("系统检测到的可用串口：");
                foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
                appendToLog(QString("  %1 (%2)").arg(info.portName()).arg(info.description()));
            }
        return;
    }

    // 初始化串口
    m_serialPort = new QSerialPort(this);
    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(ui->cmbBaude->currentText().toInt());
    m_serialPort->setDataBits(static_cast<QSerialPort::DataBits>(ui->cmbDataBit->currentData().toInt()));
    m_serialPort->setStopBits(static_cast<QSerialPort::StopBits>(ui->cmbStopBit->currentData().toInt()));
    m_serialPort->setParity(static_cast<QSerialPort::Parity>(ui->cmbParity->currentData().toInt()));

    // 连接信号槽
    connect(m_serialPort, &QSerialPort::readyRead, this, &ComWindow::handleSerialReadyRead);
    connect(m_serialPort, QOverload<QSerialPort::SerialPortError>::of(&QSerialPort::error),
            this, &ComWindow::handleSerialError);

    // 尝试连接
    if (m_serialPort->open(QIODevice::ReadWrite)) {
        m_serialConnected = true;
        ui->btnOpenUart->setText("断开");
        appendToLog("串口连接成功");
    } else {
        appendToLog(QString("连接失败: %1").arg(m_serialPort->errorString()));
        delete m_serialPort;
        m_serialPort = nullptr;
    }
}

void ComWindow::onSerialConnectClicked2() {
    handle=-1;
    auto ret=ProfinetManaged::modbus_rtu_create(serial.toStdString().c_str(), baud_rate.toInt(), station.toInt(), data_bits.toInt(),(ProfinetManaged::StopBits)stop_bits.toInt(),  (ProfinetManaged::Parity)parity.toInt(), &handle);
    if (ret.isSuccess==0){
        QMessageBox::information(nullptr,"连接成功",QString("串口站号 %1 连接成功").arg(station));

    }else {
        QMessageBox::warning(nullptr,"连接失败",QString("串口站号 %1 连接失败，%2").arg(station).arg(QString::fromStdString(ret.message)));
    }
}

void ComWindow::handleSerialReadyRead()
{
    if (!m_serialPort || !m_serialConnected) return;

    QByteArray data = m_serialPort->readAll();
    if (data.isEmpty()) return;

    // 十六进制格式显示
    QString hexData = data.toHex(' ').toUpper();
    appendToLog(QString("串口数据: %1").arg(hexData), false, true);

    // 可选：同时显示ASCII格式
    QString asciiData;
    for (char ch : data) {
        asciiData += (ch >= 32 && ch <= 126) ? ch : '.';
    }
    appendToLog(QString("ASCII格式: %1").arg(asciiData), false, true);
}

void ComWindow::handleSerialError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) return;

    QString errorMsg;
    switch (error) {
        case QSerialPort::DeviceNotFoundError: errorMsg = "设备未找到"; break;
        case QSerialPort::PermissionError: errorMsg = "权限不足"; break;
        case QSerialPort::OpenError: errorMsg = "打开失败"; break;
        case QSerialPort::NotOpenError: errorMsg = "设备未打开"; break;
        case QSerialPort::WriteError: errorMsg = "写入失败"; break;
        case QSerialPort::ReadError: errorMsg = "读取失败"; break;
        default: errorMsg = "未知错误";
    }

    appendToLog(QString("串口错误: %1 (%2)").arg(errorMsg).arg(m_serialPort->errorString()));

    // 严重错误时自动断开
    if (error == QSerialPort::ResourceError || error == QSerialPort::UnsupportedOperationError) {
        onSerialConnectClicked();
    }
}

// ====================== 网络通信相关函数 ======================

void ComWindow::initNetworkSettings()
{
    // 初始化IP地址下拉框
    ui->cmbRemoteIp->setEditable(true);

    // 1. 创建初始IP列表
    QStringList initialIps;
    initialIps<< "127.0.0.1";

    // 2. 添加本地网络IP（如果存在且不在列表中）
    QString localIp = getLocalNetworkIp();
    if (!localIp.isEmpty() && !initialIps.contains(localIp)) {
        initialIps << localIp;
    }

    // 3. 加载历史记录（修复编译错误，使用新方法）
    QSettings settings;
    QSet<QString> uniqueHosts;

    // 正确初始化QSet的方法
    for (const QString &ip : initialIps) {
        uniqueHosts.insert(ip);
    }

    // 加载历史IP（修复C2440错误）
    int size = settings.beginReadArray("ipHistory");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString ip = settings.value("address").toString().trimmed();

        if (!ip.isEmpty()) {
            // 标准化处理
            if (ip.compare("localhost", Qt::CaseInsensitive) == 0) {
                ip = "127.0.0.1";
            }
            uniqueHosts.insert(ip);  // 使用insert而不是unite
        }
    }
    settings.endArray();

    // 4. 转换为列表并排序（完全匹配图片顺序）
    QStringList ipList = uniqueHosts.values();

    // 调试输出ipList转换后内容
    appendToLog("转换后的ipList内容:", false, false);
    for (const QString& ip : ipList) {
        appendToLog(" - " + ip, false, false);
    }

    std::sort(ipList.begin(), ipList.end(), [](const QString &a, const QString &b) {
        // 保持图片中的顺序：127.0.0.1优先
        if (a == "127.0.0.1") return true;
        if (b == "127.0.0.1") return false;
        return a < b;
    });

    // 调试输出最终排序结果
    // 调试输出ipList转换后内容
    appendToLog("排序后的ipList内容:", false, false);
    for (const QString& ip : ipList) {
        appendToLog(" - " + ip, false, false);
    }

    // 5. 添加到下拉框（精确匹配图片UI）
    ui->cmbRemoteIp->clear();
    ui->cmbRemoteIp->addItems(ipList);

    // 设置默认选中项（精确匹配图片）
    int index = ui->cmbRemoteIp->findText("127.0.0.1");
    if (index != -1) {
        ui->cmbRemoteIp->setCurrentIndex(index);
    }

    // 6. 精确初始化其他控件（完全匹配图片）
    ui->txtTcpPort->setText("502");               // 图片中端口号
    ui->cmbTcpType->setCurrentText("TCP Client"); // 图片中TCP类型
    ui->cmbTcpMode->setCurrentText("无协议"); // 图片中协议类型
    ui->txtSerialStation->setText("0");           // 图片中站号

    // 7. 初始化按钮状态（匹配图片）
    ui->btnTcpConnect->setText("连接");          // 图片显示"断开"状态
    ui->btnSendTcpData->setEnabled(true);        // 图片中发送按钮可用
}

void ComWindow::onNetworkConnectClicked()
{
    if (m_networkConnected) {
        // 断开连接逻辑
        safeDisconnectNetwork();
        appendToLog("手动断开连接", false, false);
        return;
    }

    // 获取当前TCP模式（图片显示为TCP Client）
    m_isTcpServer = (ui->cmbTcpType->currentText() == "TCP Server");

    if (m_isTcpServer) {
        // 服务器模式（图片中未展示，但保留基本功能）
        m_tcpServer = new QTcpServer(this);
        if (m_tcpServer->listen(QHostAddress::Any, ui->txtTcpPort->text().toInt())) {
            connect(m_tcpServer, &QTcpServer::newConnection, this, &ComWindow::handleNewConnection);
            m_networkConnected = true;
            ui->btnTcpConnect->setText("断开");
            appendToLog(QString("TCP服务器已启动，监听端口 %1").arg(ui->txtTcpPort->text()), false, false);
        } else {
            appendToLog(QString("服务器启动失败: %1").arg(m_tcpServer->errorString()), false, false);
            delete m_tcpServer;
            m_tcpServer = nullptr;
        }
    } else {
        // 客户端模式（完全匹配图片中的功能）
        QString ip = ui->cmbRemoteIp->currentText().trimmed();
        quint16 port = ui->txtTcpPort->text().toInt();

        // 标准化处理（匹配图片中的IP格式）
        if (ip.compare("localhost", Qt::CaseInsensitive) == 0) {
            ip = "127.0.0.1";
        }

        // 验证IP和端口（端口8080来自图片日志）
        if (!isValidIpAddress(ip)) {
            appendToLog("错误：无效的IP地址格式", false, false);
            return;
        }
        if (port < 1 || port > 65535) {
            appendToLog("错误：端口号必须在1-65535之间", false, false);
            return;
        }

        // 保存连接信息用于重连
        m_lastRemoteIp = ip;
        m_lastRemotePort = port;
        m_reconnectAttempts = 0;

        // 更新IP历史记录
        QSettings settings;
        QStringList history;
        int size = settings.beginReadArray("ipHistory");
        for (int i = 0; i < size; ++i) {
            settings.setArrayIndex(i);
            history << settings.value("address").toString();
        }
        settings.endArray();

        if (!history.contains(ip)) {
            if (history.size() >= 5) {
                history.removeLast();
            }
            history.prepend(ip);

            settings.beginWriteArray("ipHistory");
            for (int i = 0; i < history.size(); ++i) {
                settings.setArrayIndex(i);
                settings.setValue("address", history[i]);
            }
            settings.endArray();
        }

        // 创建并配置Socket
        m_tcpSocket = new QTcpSocket(this);

        connect(m_tcpSocket, &QTcpSocket::connected, this, [this, ip, port]() {
            m_networkConnected = true;
            m_reconnectAttempts = 0;
            stopReconnectTimer();
            ui->btnTcpConnect->setText("断开");
            appendToLog(QString("已连接到服务器 %1:%2").arg(ip).arg(port), false, false);
        });

        connect(m_tcpSocket, &QTcpSocket::disconnected, this, [this]() {
            appendToLog("服务器连接已断开", false, false);  // 完全匹配图片日志
            if (m_networkConnected) {
                m_networkConnected = false;
                ui->btnTcpConnect->setText("连接");
                // 5秒后自动重连（匹配图片中的重连行为）
                QTimer::singleShot(5000, this, [this]() {
                    appendToLog("尝试重新连接...", false, false);
                    if (m_tcpSocket) {
                        m_tcpSocket->connectToHost(m_lastRemoteIp, m_lastRemotePort);
                    }
                });
            }
        });

        connect(m_tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
                this, [this](QAbstractSocket::SocketError error) {
                    Q_UNUSED(error)
                    appendToLog(QString("网络错误: %1").arg(m_tcpSocket->errorString()), false, false);  // 匹配图片错误格式
                });

        connect(m_tcpSocket, &QTcpSocket::readyRead, this, &ComWindow::handleNetworkReadyRead);

        // 开始连接（完全匹配图片中的连接日志）
        appendToLog(QString("正在连接 %1:%2...").arg(ip).arg(port), false, false);
        m_tcpSocket->connectToHost(ip, port);

        // 30秒连接超时（增强健壮性）
        QTimer::singleShot(30000, this, [this]() {
            if (m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectingState) {
                appendToLog("错误：连接超时", false, false);
                safeDisconnectNetwork();
            }
        });
    }
}
//void ComWindow::onNetworkConnectClicked()
//{
//    if (m_networkConnected) {
//        // 断开连接逻辑
//        safeDisconnectNetwork();
//        return;
//    }
//
//    // 获取当前TCP模式
//    m_isTcpServer = (ui->cmbTcpType->currentText() == "TCP Server");
//
//    if (m_isTcpServer) {
//        // 服务器模式
//        m_tcpServer = new QTcpServer(this);
//        if (m_tcpServer->listen(QHostAddress::Any, ui->txtTcpPort->text().toInt())) {
//            connect(m_tcpServer, &QTcpServer::newConnection, this, &ComWindow::handleNewConnection);
//            m_networkConnected = true;
//            ui->btnTcpConnect->setText("断开");
//            appendToLog(QString("TCP服务器已启动，监听端口 %1").arg(ui->txtTcpPort->text()), false, false);
//        } else {
//            appendToLog(QString("服务器启动失败: %1").arg(m_tcpServer->errorString()), false, false);
//            delete m_tcpServer;
//            m_tcpServer = nullptr;
//        }
//    } else {
//        // 客户端模式
//        QString ip = ui->cmbRemoteIp->currentText().trimmed();
//        quint16 port = ui->txtTcpPort->text().toInt();
//
//        // 标准化IP地址
//        if (ip.compare("localhost", Qt::CaseInsensitive) == 0) {
//            ip = "127.0.0.1";
//        }
//
//        // 验证IP和端口有效性
//        if (!isValidIpAddress(ip)) {
//            appendToLog("错误：无效的IP地址格式", false, false);
//            return;
//        }
//        if (port < 1 || port > 65535) {
//            appendToLog("错误：端口号必须在1-65535之间", false, false);
//            return;
//        }
//
//        // 保存连接信息用于重连
//        m_lastRemoteIp = ip;
//        m_lastRemotePort = port;
//        m_reconnectAttempts = 0;
//
//        // 更新IP历史记录
//        updateIpHistory(ip);
//
//        // 创建并配置Socket
//        m_tcpSocket = new QTcpSocket(this);
//
//        // 连接信号槽
//        connect(m_tcpSocket, &QTcpSocket::connected, this, [this, ip, port]() {
//            m_networkConnected = true;
//            m_reconnectAttempts = 0; // 重置重连计数器
//            stopReconnectTimer();    // 停止重连定时器
//            ui->btnTcpConnect->setText("断开");
//            appendToLog(QString("已连接到服务器 %1:%2").arg(ip).arg(port), false, false);
//        });
//
//        connect(m_tcpSocket, &QTcpSocket::disconnected, this, [this]() {
//            appendToLog("服务器连接已断开", false, false);
//            if (m_networkConnected) {
//                m_networkConnected = false;
//                ui->btnTcpConnect->setText("连接");
//                startReconnectTimer(); // 开始尝试重连
//            }
//        });
//
//        connect(m_tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
//                this, &ComWindow::handleNetworkError);
//
//        connect(m_tcpSocket, &QTcpSocket::readyRead, this, &ComWindow::handleNetworkReadyRead);
//
//        // 开始连接
//        appendToLog(QString("正在连接 %1:%2...").arg(ip).arg(port), false, false);
//        m_tcpSocket->connectToHost(ip, port);
//    }
//}

void ComWindow::attemptReconnect()
{
    if (m_reconnectAttempts >= 3) {
        appendToLog("重连失败次数超过3次，停止重连", false, false);
        stopReconnectTimer();
        safeDisconnectNetwork();
        return;
    }

    m_reconnectAttempts++;
    appendToLog(QString("尝试第%1次重连...").arg(m_reconnectAttempts), false, false);

    if (m_tcpSocket) {
        m_tcpSocket->connectToHost(m_lastRemoteIp, m_lastRemotePort);
    }
}

void ComWindow::startReconnectTimer()
{
    if (!m_reconnectTimer->isActive()) {
        m_reconnectTimer->start();
    }
}

void ComWindow::stopReconnectTimer()
{
    if (m_reconnectTimer->isActive()) {
        m_reconnectTimer->stop();
    }
}

void ComWindow::handleNewConnection()
{
    if (!m_tcpServer) return;

    // 断开已有连接（单连接模式）
    if (m_tcpSocket) {
        m_tcpSocket->disconnectFromHost();
        m_tcpSocket->deleteLater();
    }

    m_tcpSocket = m_tcpServer->nextPendingConnection();
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &ComWindow::handleNetworkReadyRead);
    connect(m_tcpSocket, &QTcpSocket::disconnected, [this]() {
        appendToLog("客户端断开连接", false, false);
        m_tcpSocket->deleteLater();
        m_tcpSocket = nullptr;
    });

    QString ip = formatIpAddress(m_tcpSocket->peerAddress());
    appendToLog(QString("新客户端连接: %1:%2").arg(ip).arg(m_tcpSocket->peerPort()), false, false);
}

void ComWindow::handleNetworkReadyRead()
{
    if (!m_tcpSocket) return;

    QByteArray data = m_tcpSocket->readAll();
    if (data.isEmpty()) return;

    QString hexData = data.toHex(' ').toUpper();
    appendToLog(QString("网络收到数据: %1").arg(hexData), false, false);

    // 可选：ASCII格式显示
    QString asciiData;
    for (char ch : data) {
        asciiData += (ch >= 32 && ch <= 126) ? ch : '.';
    }
    appendToLog(QString("ASCII格式: %1").arg(asciiData), false, false);
}

void ComWindow::handleNetworkError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    if (!m_tcpSocket) return;

    appendToLog(QString("网络错误: %1").arg(m_tcpSocket->errorString()));
}

void ComWindow::safeDisconnectNetwork()
{
    stopReconnectTimer();

    if (m_tcpSocket) {
        m_tcpSocket->disconnect();
        m_tcpSocket->abort();
        m_tcpSocket->deleteLater();
        m_tcpSocket = nullptr;
    }

    if (m_tcpServer) {
        m_tcpServer->close();
        delete m_tcpServer;
        m_tcpServer = nullptr;
    }

    m_networkConnected = false;
    ui->btnTcpConnect->setText("连接");
}

// ====================== 数据操作相关函数 ======================

void ComWindow::onSendClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    QString data = ui->txtSendData->toPlainText();
    if (data.isEmpty()) return;

    if (button == ui->btnSendSerialData) {
        // 串口发送按钮逻辑
        if (!m_serialConnected) {
            appendToLog("错误：请先连接串口");
            return;
        }
        m_serialPort->write(data.toLocal8Bit());
        appendToLog("发送串口数据: " + data, true, true);
    }
    else if (button == ui->btnSendTcpData) {
        // 网络发送按钮逻辑
        if (!m_networkConnected) {
            appendToLog("错误：请先建立网络连接");
            return;
        }
        m_tcpSocket->write(data.toLocal8Bit());
        appendToLog("发送网络数据: " + data, true, false);
    }
}

void ComWindow::onSendClicked2()
{
    QString data = ui->txtSendData->toPlainText();
    if (data.isEmpty()) return;

    int16 data2 = data.toInt();

    auto retDir = ProfinetManaged::profinet_write_int16(handle, "40001", &data2, 1);
    auto retDir2 = ProfinetManaged::profinet_read_int16(handle, "40001", 16);
    //auto retDir = ProfinetManaged::profinet_read_int32(handle, "40001", 1);

    if (retDir.isSuccess!=0) {
        QMessageBox::warning(nullptr, "错误", QString("串口写入失败"));
    }
    else if(retDir.isSuccess==0){
        QMessageBox::information(nullptr,"成功",QString("串口写入成功"));

    }
}

void ComWindow::onClearReceiveDataClicked()
{
    ui->txtRcvData->clear();
}

void ComWindow::onClearSendDataClicked()
{
    ui->txtSendData->clear();
}

void ComWindow::onTcpTypeChanged(int index)
{
    Q_UNUSED(index)
    m_isTcpServer = (ui->cmbTcpType->currentText() == "TCP Server");
    ui->cmbRemoteIp->setEnabled(!m_isTcpServer);
}

// ====================== 日志记录相关函数 ======================

void ComWindow::appendToLog(const QString &message, bool isSend, bool isSerial)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString prefix = isSend ? "[发送] " : "[接收] ";

    QColor color;
    if (isSend) {
        color = isSerial ? QColor(0, 0, 255)    // 串口发送：蓝色
                         : QColor(138, 43, 226); // 网络发送：紫色
    } else {
        color = isSerial ? QColor(0, 100, 0)    // 串口接收：深绿色
                         : QColor(0, 139, 139);  // 网络接收：深青色
    }

    // 保存原始格式
    QTextCharFormat originalFormat = ui->txtRcvData->currentCharFormat();

    // 设置新格式
    QTextCharFormat format;
    format.setForeground(color);
    ui->txtRcvData->setCurrentCharFormat(format);

    // 添加日志
    ui->txtRcvData->append(QString("[%1]%2%3")
                                   .arg(timestamp)
                                   .arg(prefix)
                                   .arg(message));

    // 恢复原始格式
    ui->txtRcvData->setCurrentCharFormat(originalFormat);

    // 自动滚动和行数限制
    QTextCursor cursor = ui->txtRcvData->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->txtRcvData->setTextCursor(cursor);

    const int maxLines = 1000;
    if (ui->txtRcvData->document()->lineCount() > maxLines) {
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, 100);
        cursor.removeSelectedText();
    }
}

// ====================== IP历史记录管理 ======================

void ComWindow::updateIpHistory(const QString &ip)
{
    // 标准化处理
    QString standardizedIp = ip;
    if (ip.compare("localhost", Qt::CaseInsensitive) == 0) {
        standardizedIp = "127.0.0.1";
    }

    // 更新下拉框（去重）
    int existingIndex = ui->cmbRemoteIp->findText(standardizedIp);
    if (existingIndex != -1) {
        ui->cmbRemoteIp->removeItem(existingIndex);
    }
    ui->cmbRemoteIp->insertItem(0, standardizedIp);
    ui->cmbRemoteIp->setCurrentIndex(0);
}

void ComWindow::saveIpHistory()
{
    QSettings settings;
    QStringList history;

    // 收集当前下拉框中的IP（最多5个）
    for (int i = 0; i < qMin(5, ui->cmbRemoteIp->count()); ++i) {
        QString ip = ui->cmbRemoteIp->itemText(i);
        if (!ip.isEmpty()) {
            history << ip;
        }
    }

    // 保存到配置文件
    settings.beginWriteArray("ipHistory");
    for (int i = 0; i < history.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("address", history[i]);
    }
    settings.endArray();
}

void ComWindow::loadIpHistory()
{
    QSettings settings;
    QSet<QString> uniqueHosts;

    // 添加默认项
    uniqueHosts << "127.0.0.1";

    // 添加本地网络IP
    QString localIp = getLocalNetworkIp();
    if (!localIp.isEmpty() && localIp != "127.0.0.1") {
        uniqueHosts << localIp;
    }

    // 加载历史记录
    int size = settings.beginReadArray("ipHistory");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString ip = settings.value("address").toString();
        if (!ip.isEmpty()) {
            if (ip.compare("localhost", Qt::CaseInsensitive) == 0) {
                ip = "127.0.0.1";
            }
            uniqueHosts << ip;
        }
    }
    settings.endArray();

    // 添加到下拉框
    ui->cmbRemoteIp->addItems(uniqueHosts.values());
}

// ====================== 辅助函数 ======================

bool ComWindow::isValidIpAddress(const QString &ip)
{
    QHostAddress address;
    if (address.setAddress(ip)) {
        return true;
    }

    // 检查是否是有效的主机名
    if (ip.compare("localhost", Qt::CaseInsensitive) == 0) {
        return true;
    }

    // 简单的域名格式检查
    QRegularExpression re("^[a-zA-Z0-9.-]+$");
    return re.match(ip).hasMatch();
}

QString ComWindow::formatIpAddress(const QHostAddress &address)
{
    if (address.protocol() == QAbstractSocket::IPv4Protocol) {
        return address.toString();
    }

    // 处理IPv4-mapped IPv6地址
    if (address.protocol() == QAbstractSocket::IPv6Protocol) {
        QString ipv6 = address.toString();
        if (ipv6.startsWith("::ffff:")) {
            return ipv6.mid(7); // 提取IPv4部分
        }
        return ipv6;
    }

    return "unknown";
}

QString ComWindow::getLocalNetworkIp()
{
            foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
            if (address.protocol() == QAbstractSocket::IPv4Protocol
                && address != QHostAddress(QHostAddress::LocalHost)) {
                return address.toString();
            }
        }
    return "";
}


void ComWindow::initparameter()
{
    connect(ui->cmbPortNames, &QComboBox::currentTextChanged, this, [=]() {
               serial=ui->cmbPortNames->currentText().left(4);
        });
    connect(ui->cmbBaude, &QComboBox::currentTextChanged, this, [=]() {
               baud_rate=ui->cmbBaude->currentText();
        });
    connect(ui->cmbDataBit, &QComboBox::currentTextChanged, this, [=]() {
               data_bits=ui->cmbDataBit->currentText();
        });
    connect(ui->cmbStopBit, &QComboBox::currentTextChanged, this, [=]() {
               stop_bits=ui->cmbStopBit->currentText();
        });
    connect(ui->cmbParity, QOverload<int>::of(&QComboBox::currentIndexChanged),this, [=](int index) {
               parity = QString::number(index);
        });
    connect(ui->txtSerialStation, &QLineEdit::editingFinished, this, [=]() {
            station=ui->txtSerialStation->text();
    });

}

void ComWindow::writeLog(const QString &logText) {

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

