//
// Created by simon on 2024-11-19.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QStatusBar>
#include <QCloseEvent>
#include <QTimer>
#include <QDateTime>
#include <VpMainUi.h>
#include <VpVarSettingUi.h>
#include "CVariant.h"
#include "VisionCore.h"
#include "VisionProject.h"
#include "VpSystemSetting.h"
#include "VpMessageBox.h"
#include "FrmProjList.h"
#include "FrmCameras.h"
#include "VpDialog.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>


#include <QList>
#include <format>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <VpLog.h>
#include <QSqlQuery>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QSqlError>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QSet>
#include <QSqlQuery>
#include <QSqlError>
#include <QSet>

#include "BlSizeWindow.h"

unsigned int MainWindow::Visual_Camera_Tri_Count_Result[3][512] = {0};

struct SummaryResult {
    int uniqueSnCount;    // 去重后的总条目数
    int allTrueResultCount; // Result全部为true的产品数
};

SummaryResult summarizeMultipleTables(QSqlDatabase &db, const QStringList &tableNames,
                                      const QDateTime &startTime, const QDateTime &endTime) {
    SummaryResult result = {0, 0};
    QSet<QString> allUniqueSns;  // 存储所有表中的唯一SN
    QSet<QString> snsWithAllTrue; // 存储所有Result都为true的SN

    // 检查时间有效性
    if (!startTime.isValid() || !endTime.isValid()) {

        LOG_ERROR(QString("错误: 无效的日期时间参数"));
        return result;
    }

    // 转换为SQLite支持的格式
    const QString timeFormat = "yyyy-MM-dd HH:mm:ss";
    QString startTimeStr = startTime.toString(timeFormat);
    QString endTimeStr = endTime.toString(timeFormat);

    // 遍历所有表
    for (const QString &tableName: tableNames) {
        // 查询当前表中指定时间范围内的唯一SN及其Result状态
        QString queryStr =
                "SELECT SN, MAX(Result) AS AllTrue "
                "FROM %1 "
                "WHERE InsertTime BETWEEN :startTime AND :endTime "
                "GROUP BY SN;";

        QSqlQuery query(db);
        query.prepare(queryStr.arg(tableName));
        query.bindValue(":startTime", startTimeStr);
        query.bindValue(":endTime", endTimeStr);

        if (query.exec()) {
            while (query.next()) {
                QString sn = query.value(0).toString();
                bool allTrue = query.value(1).toBool();

                // 收集唯一SN
                if (!sn.isEmpty() && !allUniqueSns.contains(sn)) {
                    allUniqueSns.insert(sn);

                    // 如果当前SN的所有Result都是true，则记录
                    if (allTrue) {
                        snsWithAllTrue.insert(sn);
                    }
                }
            }
        }
//        else {
//
//            LOG_ERROR(QString("查询表: %1 失败").arg(tableName) );
//
//            LOG_ERROR(QString("查询失败: %1 ").arg(query.lastError().text()));
//            LOG_ERROR(QString("查询状态: %1 ").arg(query.isActive() ? "活动" : "非活动"));
//        }
    }

    // 计算最终结果
    result.uniqueSnCount = allUniqueSns.size();

    // 验证每个SN是否在所有表中都没有false的Result
    for (const QString &sn: allUniqueSns) {
        if (!snsWithAllTrue.contains(sn)) {
            // 如果某个SN不在snsWithAllTrue集合中，说明它至少在一个表中有false的Result
            continue;
        }

        // 检查该SN是否存在于所有表中且都为true
        bool existsInAllTables = true;
        for (const QString &tableName: tableNames) {
            QString checkQueryStr =
                    "SELECT EXISTS (SELECT 1 FROM %1 "
                    "WHERE SN = :sn "
                    "  AND Result = 0 "
                    "  AND InsertTime BETWEEN :startTime AND :endTime);";

            QSqlQuery checkQuery(db);
            checkQuery.prepare(checkQueryStr.arg(tableName));
            checkQuery.bindValue(":sn", sn);
            checkQuery.bindValue(":startTime", startTimeStr);
            checkQuery.bindValue(":endTime", endTimeStr);

            if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toBool()) {
                // 如果在当前表中找到了该SN的Result为false的记录
                existsInAllTables = false;
                break;
            }
        }

        if (existsInAllTables) {
            result.allTrueResultCount++;
        }
    }

    return result;
}


struct QueryResult {
    int totalCount;       // 指定时间范围内的总条目数
    int trueResultCount;  // Result为true的条目数
};

QueryResult queryDataByTimeRange(QSqlDatabase &db, const QString &tableName,
                                 const QDateTime &startTime, const QDateTime &endTime) {
    QueryResult result = {0, 0};

    // 检查时间有效性
    if (!startTime.isValid() || !endTime.isValid()) {
        LOG_ERROR(QString("错误: 无效的日期时间参数"));
        return result;
    }

    // 转换为SQLite支持的格式 (YYYY-MM-DD HH:MM:SS)
    const QString timeFormat = "yyyy-MM-dd HH:mm:ss";
    QString startTimeStr = startTime.toString(timeFormat);
    QString endTimeStr = endTime.toString(timeFormat);

    QString queryStr =
            "SELECT "
            "    COUNT(*), "
            "    SUM(CASE WHEN Result THEN 1 ELSE 0 END) "
            "FROM %1 "
            "WHERE InsertTime BETWEEN :startTime AND :endTime;";

    QSqlQuery query(db);
    query.prepare(queryStr.arg(tableName));
    query.bindValue(":startTime", startTimeStr);
    query.bindValue(":endTime", endTimeStr);

//    qDebug() << "执行SQL查询:" << query.lastQuery();
    LOG_INFO(QString("查询状态：%3  startTime =%1 , endTime =%2").arg(startTimeStr).arg(endTimeStr).arg(tableName));
    if (query.exec() && query.next()) {
        result.totalCount = query.value(0).toInt();
        result.trueResultCount = query.value(1).toInt();

        LOG_INFO(QString("查询成功: 总条目数 =%1 Result为true的条目数 =%2").arg(result.totalCount).arg(
                result.trueResultCount));

    } else {
        LOG_ERROR(QString("查询失败: %1 ").arg(query.lastError().text()));
    }

    return result;
}




// 存储NG类型及其数量的映射
using NgCountMap = QMap<QString, int>;


// 辅助函数：处理JSON数据并更新NG计数
void processJsonData(const QString &jsonStr, NgCountMap &ngCounts) {
    if (jsonStr.isEmpty()) return;

    // 解析JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        LOG_ERROR(QString("JSON解析错误: %1 ").arg(parseError.errorString()));
        return;
    }

    // 处理JSON数组
    if (doc.isArray()) {
        QJsonArray items = doc.array();
        for (const auto &itemValue: items) {
            if (!itemValue.isObject()) continue;
            QJsonObject item = itemValue.toObject();
            QString label = item["label"].toString();
            if (label.isEmpty()) {
                continue;
            }
            bool result = item["result"].toBool(true); // 默认true

            if (!ngCounts.contains(label)) {
                ngCounts[label] = 0;
            }
            // 如果result为false（NG项），统计数量
            if (!result) {
                ngCounts[label]++;
            }
        }
    }
}

NgCountMap countNgItems(QSqlDatabase &db, const QString &tableName,
                        const QDateTime &startTime, const QDateTime &endTime) {
    NgCountMap ngCounts;

    // 检查时间有效性
    if (!startTime.isValid() || !endTime.isValid()) {
        LOG_ERROR(QString("错误: 无效的日期时间参数 "));
        return ngCounts;
    }

    // 转换为SQLite支持的格式
    const QString timeFormat = "yyyy-MM-dd HH:mm:ss";
    QString startTimeStr = startTime.toString(timeFormat);
    QString endTimeStr = endTime.toString(timeFormat);

    // 查询指定时间范围内的MeasureDetails和DetectDetails
    QString queryStr =
            "SELECT MeasureDetails, DetectDetails "
            "FROM %1 "
            "WHERE InsertTime BETWEEN :startTime AND :endTime;";

    QSqlQuery query(db);
    query.prepare(queryStr.arg(tableName));
    query.bindValue(":startTime", startTimeStr);
    query.bindValue(":endTime", endTimeStr);

    if (!query.exec()) {

        LOG_ERROR(QString("查询失败: %1 ").arg(query.lastError().text()));
        LOG_ERROR(QString("查询状态: %1 ").arg(query.isActive() ? "活动" : "非活动"));
        return ngCounts;
    }

    // 处理查询结果
    while (query.next()) {
        // 处理MeasureDetails
        QString measureJsonStr = query.value(0).toString();
        processJsonData(measureJsonStr, ngCounts);

        // 处理DetectDetails
        QString detectJsonStr = query.value(1).toString();
        processJsonData(detectJsonStr, ngCounts);
    }

    return ngCounts;
}


struct Product {
    QString SN;
    struct Details {

        QList<QString> name;
        QList<double> value;
        QList<double> min;
        QList<double> max;
        QList<bool> result;
    }
            MeasureDetails,
            DetectDetails;


};

// 辅助函数：处理JSON数据并更新NG计数
void JsonData2Product(const QString &jsonStr, Product::Details &details) {
    if (jsonStr.isEmpty()) return;

    // 解析JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        LOG_ERROR(QString("JSON解析错误: %1 ").arg(parseError.errorString()));
        return;
    }

    // 处理JSON数组
    if (doc.isArray()) {
        QJsonArray items = doc.array();
        for (const auto &itemValue: items) {
            if (!itemValue.isObject()) continue;
            QJsonObject item = itemValue.toObject();
            QString label = item["label"].toString();
            double value = item["value"].toDouble();
            double max = item["max"].toDouble();
            double min = item["min"].toDouble();
            if (label.isEmpty()) {
                continue;
            }
            bool result = item["result"].toBool(true); // 默认true
            details.name.append(label);
            details.value.append(value);
            details.max.append(max);
            details.min.append(min);
            details.result.append(result);

        }
    }
}


QList<Product> measureItems(QSqlDatabase &db, const QString &tableName,
                            const QDateTime &startTime, const QDateTime &endTime) {
    QList<Product> lstProduct;

    // 检查时间有效性
    if (!startTime.isValid() || !endTime.isValid()) {
        LOG_ERROR(QString("错误: 无效的日期时间参数"));
        return lstProduct;
    }

    // 转换为SQLite支持的格式
    const QString timeFormat = "yyyy-MM-dd HH:mm:ss";
    QString startTimeStr = startTime.toString(timeFormat);
    QString endTimeStr = endTime.toString(timeFormat);

    // 查询指定时间范围内的MeasureDetails和DetectDetails
    QString queryStr =
            "SELECT SN, MeasureDetails, DetectDetails  "
            "FROM %1 "
            "WHERE InsertTime BETWEEN :startTime AND :endTime;";

    QSqlQuery query(db);
    query.prepare(queryStr.arg(tableName));
    query.bindValue(":startTime", startTimeStr);
    query.bindValue(":endTime", endTimeStr);

    if (!query.exec()) {

        LOG_ERROR(QString("查询失败: %1 ").arg(query.lastError().text()));
        LOG_ERROR(QString("查询状态: %1 ").arg(query.isActive() ? "活动" : "非活动"));
        return lstProduct;
    }

    // 处理查询结果
    while (query.next()) {
        QString sn = query.value(0).toString();
        // 处理MeasureDetails
        QString measureJsonStr = query.value(1).toString();
        QString detectJsonStr = query.value(2).toString();
        Product product;
        product.SN = sn;
        JsonData2Product(measureJsonStr, product.MeasureDetails);
        JsonData2Product(detectJsonStr, product.DetectDetails);
        lstProduct.append(product);
    }

    return lstProduct;
}


bool insertRecord(const QString &tableName, const QString &sn, const QString &machineName,
                  const QString &productName, const QString &lot, const QString &yieldNum,
                  const QString &measureDetails, const QString &detectDetails, bool result) {
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        LOG_ERROR(QString("Database is not open: %1").arg(db.lastError().text()));
        return false;
    }

    // 检查表是否存在，不存在则创建
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name=:tableName");
    checkQuery.bindValue(":tableName", tableName);

    if (!checkQuery.exec()) {
        LOG_ERROR(QString("Check table existence failed: %1").arg(checkQuery.lastError().text()));
        return false;
    }

    if (!checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        // 表不存在，创建表
        QString createTableSql = QString(
                "CREATE TABLE %1 ("
                "    SN             TEXT PRIMARY KEY"
                "                        NOT NULL,"
                "    MachineName    TEXT,"
                "    ProductName    TEXT,"
                "    Lot            TEXT,"
                "    YieldNum       TEXT,"
                "    InsertTime     TEXT DEFAULT (datetime('now', 'localtime')),"
                "    MeasureDetails TEXT,"
                "    DetectDetails  TEXT,"
                "    Result         BLOB"
                ");"
        ).arg(tableName);

        QSqlQuery createQuery(db);
        if (!createQuery.exec(createTableSql)) {
            LOG_ERROR(QString("Create table failed: %1").arg(createQuery.lastError().text()));
            return false;
        }
        LOG_INFO(QString("Table %1 created successfully").arg(tableName));
    }

    // 执行插入操作
    QSqlQuery insertQuery(db);
    insertQuery.prepare(
            QString("INSERT INTO %1 (SN, MachineName, ProductName, Lot, YieldNum, InsertTime, MeasureDetails, DetectDetails, Result) "
                    "VALUES (:sn, :machineName, :productName, :lot, :yieldNum, datetime('now', 'localtime'), :measureDetails, :detectDetails, :result)")
                    .arg(tableName));

    insertQuery.bindValue(":sn", sn);
    insertQuery.bindValue(":machineName", machineName);
    insertQuery.bindValue(":productName", productName);
    insertQuery.bindValue(":lot", lot);
    insertQuery.bindValue(":yieldNum", yieldNum);
    insertQuery.bindValue(":measureDetails", measureDetails);
    insertQuery.bindValue(":detectDetails", detectDetails);
    insertQuery.bindValue(":result", result);

    if (!insertQuery.exec()) {
        LOG_ERROR(QString("Insert failed: %1").arg(insertQuery.lastError().text()));
        return false;
    }


    return true;
}

MainWindow::MainWindow(QWidget *parent) :
        VpMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupTitleBar();

    init();

    writeLogToFile("init()");
    test();
    myBoardCard=new BoardCard(this);
    comWindow=new ComWindow(this);
    vfdWindow=new VFDWindow(this);
    mesWindow=new MESWindow(this);
    blsizeWindow=new BlSizeWindow(this);

    calibrationWindow=new CalibrationWindow(this);
    calibrationWindow->myBoardCard=myBoardCard=this->myBoardCard;



    //入料数相机计数改变
    connect(myBoardCard, &BoardCard::camconChanged,this, &MainWindow::onCamconChanged);

    connect(myBoardCard, &BoardCard::materialChanged,this, &MainWindow::onMaterialChanged);


    //plc启动、停止
    connect(myBoardCard, &BoardCard::plcStartMotor,this, &MainWindow::onPlcStartMotor);
    connect(myBoardCard, &BoardCard::plcStartMotor,this, &MainWindow::onPlcStopMotor);

    /*connect(myBoardCard, &BoardCard::plcStartMotor,this, [=]()
    {
        if (start==0) buttonStatus->click();
    });
    connect(myBoardCard, &BoardCard::plcStopMotor,this, [=]()
    {
        if (start==1) buttonStatus->click();
    });*/



    //相机比例变化
    connect(blsizeWindow,&BlSizeWindow::camRatioCheck,this,&MainWindow::onCamRatioChanged);

    //yBoardCard->show();
    connect(ui->action_5, &QAction::triggered, this, [=]() {
        if (!m_User.switch_project()) {
      VpMessageBox::Warning(this, tr("权限不足"), tr("您没有权限执行此操作"));
      return;
  }
        myBoardCard->show();
    });

    connect(ui->action_7, &QAction::triggered, this, [this]() {
        if (!m_User.switch_project()) {
      VpMessageBox::Warning(this, tr("权限不足"), tr("您没有权限执行此操作"));
      return;
  }
       calibrationWindow->show();
    });

    connect(ui->action_6, &QAction::triggered, this, [this]() {
        //vfdWindow->setAttribute(Qt::WA_DeleteOnClose);
            if (!m_User.switch_project()) {

                 VpMessageBox::Warning(this, tr("权限不足"), tr("您没有权限执行此操作"));
                 return;}

        vfdWindow->show();
    });

    connect(ui->actionMES, &QAction::triggered, this, [this]() {
        //mesWindow->setAttribute(Qt::WA_DeleteOnClose);
        mesWindow->show();
    });
    writeLogToFile("绑定菜单");
    m_LabVer = new QLabel("1.0.0.5");
    m_CurrentDate = new QLabel();
    auto bar = statusBar();
    startVisionThread();

    m_CurrentUser = new QLabel();
    bar->addWidget(m_CurrentDate);
    bar->addPermanentWidget(m_CurrentUser);
    bar->addPermanentWidget(m_LabVer);

    m_PlcInfo = new QLabel("");
    m_PlcState = new QLabel("");
    m_PlcState->setPixmap(none);
    bar->addWidget(m_PlcInfo);
    bar->addWidget(m_PlcState);

    //通信菜单
    connect(ui->action_4, &QAction::triggered, this, [=]() {
    //comWindow->setAttribute(Qt::WA_DeleteOnClose);
    comWindow->show();
    });
    ui->menu_5->menuAction()->setVisible(false);
    ui->action_4->setVisible(false);



    connect(ui->action1_1_3, &QAction::triggered, this, [=]() {
        if (!m_User.switch_project()) {
     VpMessageBox::Warning(this, tr("权限不足"), tr("您没有权限执行此操作"));
     return;
 }
        ui->action_6->setVisible(true);
        passage1->setText("通道1");
        passage2->setText("通道2");
        passage3->setText("通道3");

        passage=1;
        saveAppConfig();
        myBoardCard->loadAppConfig();
        QStringList newChannelNames = { "通道1", "通道2", "通道3" };

        ui->action1_1_3->setChecked(true);
        ui->action2_4_6->setChecked(false);
        ui->action3_7_9->setChecked(false);
        ui->action4_10_12->setChecked(false);

        for (int i = 0; i < newChannelNames.size(); ++i) {
        int baseRow = i * 2;
        QTableWidgetItem *item = middleTab->item(baseRow, 0);
        if (item) {
             item->setText(newChannelNames[i]);
                 }
        }

    });
    connect(ui->action2_4_6, &QAction::triggered, this, [=]() {
        if (!m_User.switch_project()) {
     VpMessageBox::Warning(this, tr("权限不足"), tr("您没有权限执行此操作"));
     return;
 }
        ui->action_6->setVisible(false);
        passage1->setText("通道4");
        passage2->setText("通道5");
        passage3->setText("通道6");

        passage=4;
        saveAppConfig();
        myBoardCard->loadAppConfig();

        QStringList newChannelNames = { "通道4", "通道5", "通道6" };

        ui->action1_1_3->setChecked(false);
        ui->action2_4_6->setChecked(true);
        ui->action3_7_9->setChecked(false);
        ui->action4_10_12->setChecked(false);

              for (int i = 0; i < newChannelNames.size(); ++i) {
              int baseRow = i * 2;
              QTableWidgetItem *item = middleTab->item(baseRow, 0);
              if (item) {
                   item->setText(newChannelNames[i]);
                       }
              }

    });
    connect(ui->action3_7_9, &QAction::triggered, this, [=]() {
        if (!m_User.switch_project()) {
     VpMessageBox::Warning(this, tr("权限不足"), tr("您没有权限执行此操作"));
     return;
 }
        ui->action_6->setVisible(false);
        passage1->setText("通道7");
        passage2->setText("通道8");
        passage3->setText("通道9");

        passage=7;
        saveAppConfig();
        myBoardCard->loadAppConfig();


        QStringList newChannelNames = { "通道7", "通道8", "通道9" };

        ui->action1_1_3->setChecked(false);
        ui->action2_4_6->setChecked(false);
        ui->action3_7_9->setChecked(true);
        ui->action4_10_12->setChecked(false);

                      for (int i = 0; i < newChannelNames.size(); ++i) {
                      int baseRow = i * 2;
                      QTableWidgetItem *item = middleTab->item(baseRow, 0);
                      if (item) {
                           item->setText(newChannelNames[i]);
                               }
                      }

    });
    connect(ui->action4_10_12, &QAction::triggered, this, [=]() {
        if (!m_User.switch_project()) {
     VpMessageBox::Warning(this, tr("权限不足"), tr("您没有权限执行此操作"));
     return;
 }
        ui->action_6->setVisible(false);
        passage1->setText("通道10");
        passage2->setText("通道11");
        passage3->setText("通道12");

        passage=10;
        saveAppConfig();
        myBoardCard->loadAppConfig();

        QStringList newChannelNames = { "通道10", "通道11", "通道12" };

        ui->action1_1_3->setChecked(false);
        ui->action2_4_6->setChecked(false);
        ui->action3_7_9->setChecked(false);
        ui->action4_10_12->setChecked(true);

                              for (int i = 0; i < newChannelNames.size(); ++i) {
                              int baseRow = i * 2;
                              QTableWidgetItem *item = middleTab->item(baseRow, 0);
                              if (item) {
                                   item->setText(newChannelNames[i]);
                                       }
                              }

    });



    connect(ui->actionCamera, &QAction::triggered, this, [=]()
    {
        if (!m_User.switch_project()) {
     VpMessageBox::Warning(this, tr("权限不足"), tr("您没有权限执行此操作"));
     return;
        }

        VpDialog::ShowDialog<FrmCameras>();
    });


    connect(ui->action_10, &QAction::triggered, this, [=]()
    {
        if (!m_User.switch_project()) {
     VpMessageBox::Warning(this, tr("权限不足"), tr("您没有权限执行此操作"));
     return;
        }
        blsizeWindow->show();
    });


    QTimer *timerClear = new QTimer(this);//清料
    connect(timerClear, &QTimer::timeout, this, &MainWindow::onClearMaterials);
    timerClear->start(6000);


    QTimer *timerTrig = new QTimer(this); // 吹气次数打印
    connect(timerTrig, &QTimer::timeout, this, [this]() {
       if (buttonStatus->text() == "停止") {
           Get_Trig_Blow();
       }
   });
    //timerTrig->start(3000);



    QTimer *timerUpdate = new QTimer(this); // 数据刷新

    connect(timerUpdate, &QTimer::timeout, this, [this]() {
        if (buttonStatus->text() == "停止") {
            updataTab1();
            updataTab2();
            saveDataToIni2(middleTab);
        }
    });
    timerUpdate->start(500);




    if (passage==1)
    {


    }  else if (passage==4)
    {
        ui->action2_4_6->setChecked(true);
    }else if(passage==7)
    {
        ui->action3_7_9->setChecked(true);
    }else if(passage==10)
    {
        ui->action4_10_12->setChecked(true);
    }


    // m_1STimer = new QTimer(this);
    // connect(m_1STimer, &QTimer::timeout, this, &MainWindow::On1sTimeout);
    // m_1STimer->setInterval(1000);
    // m_1STimer->start();

    //connect(ui->btnExportReport, &QPushButton::clicked, this, &MainWindow::onBtnExportReport);
    //connect(ui->btnClearData, &QPushButton::clicked, this, &MainWindow::onBtnClearData);


    //connect(ui->btnStart, &QToolButton::clicked, this, &MainWindow::btnStartClicked);
    initCameraState();
    writeLogToFile("初始化相机状态");


    connect(VisionProject::Current(), &VisionProject::TaskFinished, this, &MainWindow::onTaskFinished);
    connect(VpUserManager::Glob(), &VpUserManager::UserChanged, this, &MainWindow::onUserLogin);
    connect(ui->actionSwitchProj, &QAction::triggered, this, &MainWindow::onActionSwitchProjClicked);
    onUserLogin(VpUserManager::Glob()->Current());


    QTimer *timer2 = new QTimer(this);//灯光
    connect(timer2, &QTimer::timeout, this, &MainWindow::updateLight);
    timer2->start(50);



    //数据更新
    /*QTimer* timer3 = new QTimer(this);//检测数据
    connect(timer3, &QTimer::timeout, this, &MainWindow::updatedata);
    timer3->start(1000);*/
    writeLogToFile("检测数据");


    myBoardCard->openBoard();
    myBoardCard->InitBoardConfig();
    writeLogToFile("板卡初始化");


    //初始化相机倍率
    camRatio[0]=blsizeWindow->camRatio1.toDouble();
    camRatio[1]=blsizeWindow->camRatio2.toDouble();
    camRatio[2]=blsizeWindow->camRatio3.toDouble();



    //单次保存
    //loadCSVToTable(middleTab);//加载表格
    //loadCSVToData();

    isDataFile();//是否加载数据


    //loadCSVToData(); //csv写入data
    //updataTab1(true); //更新入料和拍照
    //updataTab2(true);//更新分类
    myBoardCard->clearcount();


    loadAppConfig();//加载配置
    if (passage==1)
    {
        ui->action1_1_3->trigger();

    }else if (passage==4)
    {
        ui->action2_4_6->trigger();
    }else if (passage==7)
    {
        ui->action3_7_9->trigger();
    }else if (passage==10)
    {
        ui->action4_10_12->trigger();
    }

}

MainWindow::~MainWindow() {
    saveDataToIni2(middleTab);
    saveToCSV(middleTab);
    delete ui;
}

void MainWindow::ShowVpEdit() {
    if (!VpUserManager::Current().edit_project()) {
        VpMessageBox::Information(this, tr("权限不足"), tr("您没有权限执行此操作"));
        return;
    }
    VpMainUi *mainUi = new VpMainUi(this);
    mainUi->setWindowModality(Qt::WindowModal);
    mainUi->setAttribute(Qt::WA_DeleteOnClose);
    mainUi->showMaximized();
}

// void MainWindow::On1sTimeout() {
//
//
//     auto res = summarizeMultipleTables(db, g_MachineInfo.WorkspaceName, g_MachineInfo.StartTime,
//                                        QDateTime::currentDateTime());
//
//     ui->labTatalCount->setText(QString::number(res.uniqueSnCount));
//     ui->labOkCount->setText(QString::number(res.allTrueResultCount));
//     if (res.uniqueSnCount != 0) {
//         auto yield = res.allTrueResultCount * 100.0 / res.uniqueSnCount;
//         ui->labYield->setText(QString::number(yield) + " %");
//         ui->labDefect->setText(QString::number(100 - yield) + " %");
//     } else {
//
//         ui->labYield->setText("nan");
//         ui->labDefect->setText("nan");
//     }
//
//
//     auto now = QDateTime::currentDateTime().toString("yyyy MM dd HH:mm:ss");
//     m_CurrentDate->setText(now);
//
//     auto cameras = VisionCore::Camera::List();
//     auto camerasSet = cameras.toSet();
//     auto deletedCamera = m_Cameras.subtract(camerasSet);
//     m_Cameras = camerasSet;
//     for (auto camera: deletedCamera) {
//         auto labState = m_CameraWidget->findChild<QLabel *>(tr("cam_%1_state").arg(camera));
//         if (labState) {
//             m_CameraLayout->removeWidget(labState);
//             labState->deleteLater();
//         }
//         auto label = m_CameraWidget->findChild<QLabel *>(tr("cam_%1_lab").arg(camera));
//         if (label) {
//             m_CameraLayout->removeWidget(label);
//             label->deleteLater();
//         }
//     }
//
//
//     for (auto camera: cameras) {
//         auto stateName = tr("cam_%1_state").arg(camera);
//         auto labState = m_CameraWidget->findChild<QLabel *>(stateName);
//         if (labState == nullptr) {
//             QLabel *label = new QLabel(tr("相机：%1").arg(camera));
//             labState = new QLabel();
//             labState->setObjectName(stateName);
//             label->setObjectName(tr("cam_%1_lab").arg(camera));
//             m_CameraLayout->addWidget(label);
//             m_CameraLayout->addWidget(labState);
//
//         }
//
//
//         auto isConnect = VisionCore::Camera::IsConnected(camera);
//         if (isConnect) {
//             labState->setPixmap(success);
//         } else {
//             labState->setPixmap(failed);
//         }
//     }
//
// }

// void MainWindow::btnStartClicked() {
//     if (!m_User.run_project()) {
//         VpMessageBox::Warning(this, tr("权限不足"), tr("您没有权限执行此操作"));
//         return;
//     }
//
//     if (m_IsRun) {
//         VisionCore::Project::Stop();
//         ui->btnStart->setText("开始");
//         ui->btnStart->setStyleSheet("color: rgb(0, 255, 0)");
//         m_IsRun = false;
//     } else {
//         QString dir = QString("%4/%1_%2_%3").arg(g_MachineInfo.ProductName)
//                 .arg(g_MachineInfo.Lot)
//                 .arg(g_MachineInfo.YieldNum).arg(g_MachineInfo.Dir);
//         auto btn = VpMessageBox::Question(this, "询问", QString("是否使用上次的导出路径：\n %1").arg(dir));
//         if (btn != QMessageBox::Ok) {
//             VpDialog::ShowDialog<FrmMachineInfo>();
//         }
//
//         VisionCore::Project::Run();
//         ui->btnStart->setText("停止");
//         ui->btnStart->setStyleSheet("color: rgb(255, 0, 0)");
//         m_IsRun = true;
//     }
// }

void MainWindow::initCameraState() {
    auto cameras = VisionCore::Camera::List();
    m_CameraWidget = new QWidget();
    m_CameraLayout = new QHBoxLayout(m_CameraWidget);
    if (vfdWindow->vfdConnect==1)
    {
        QLabel *vfdlabel = new QLabel(QString("串口：%1").arg(vfdWindow->serial));
        m_CameraLayout->addWidget(vfdlabel);
    }else
    {
        QLabel *vfdlabel = new QLabel(QString("串口连接失败"));
        m_CameraLayout->addWidget(vfdlabel);
    }

    for (auto camera: cameras) {
        if (VisionCore::Camera::IsConnected(camera)) {
            auto trigger = VisionCore::Camera::TriggerSource(camera);
            if (trigger == 0) {
                VisionCore::Camera::SetTrigger(camera, 1);
            }
        }

        QLabel *label = new QLabel(tr("相机：%1").arg(camera));
        QLabel *labState = new QLabel();
        labState->setObjectName(tr("cam_%1_state").arg(camera));
        label->setObjectName(tr("cam_%1_lab").arg(camera));
        m_CameraLayout->addWidget(label);
        m_CameraLayout->addWidget(labState);
    }
    m_CameraLayout->setMargin(0);
    m_CameraWidget->setLayout(m_CameraLayout);
    statusBar()->addWidget(m_CameraWidget);
    m_Cameras = cameras.toSet();

}

void MainWindow::onTaskFinished(QString task, int elapsed, QVariantMap output)
{

    QElapsedTimer timerStack;
    timerStack.start();
    int category;
    double widthT;
    double heightT;
    double width;
    double height;
    bool result;
    bool beteldata;
    bool isStack=false;
   // writeLog("onTaskFinished---进入相机回调");

    if (output.contains("result") && output["result"].canConvert<bool>() && output["result"].toBool())
    {
         result = output["result"].toBool();
    }else
    {
        result=false;
    }

    if (output.contains("beteldata")&& output["beteldata"].canConvert<QVector<VpAiInferResult>>())
    {
        beteldata=true;
    }else
    {
        beteldata=false;
    }

    if (output.contains("isStack")&& output["isStack"].canConvert<bool>())
    {
        isStack=output["isStack"].toBool();
    }

    auto results1 = output["beteldata"].value<QVector<VpAiInferResult>>();
    QString timestamp = output["timestamp"].value<QString>();
    //auto beteldata = output["beteldata"].value<QVector<VpAiInferResult>>();
    //writeLog(QString("返回结果%1，时间戳%2").arg(results).arg(timestamp));

    // writeLog(QString("收到相机处理结果：").arg(output["result"].value<bool>));
    try {
        QList<int> candidates = {2, 3, 4, 5, 6, 7, 8,9, 10};
        int index = QRandomGenerator::global()->bounded(candidates.size());
        int num = candidates[index];

        QList<int> candidates2 = {2, 3, 4, 5, 6, 7, 8,9, 10};
        int index2 = QRandomGenerator::global()->bounded(candidates.size());
        int num2 = candidates[index2];


        QList<int> candidates3 = {2, 3, 4, 5, 6, 7, 8,9, 10};
        int index3 = QRandomGenerator::global()->bounded(candidates.size());
        int num3 = candidates[index3];

        int count1=data1[0][1].toInt();
        int count2=data2[0][1].toInt();
        int count3=data3[0][1].toInt();
        int ret=-1;


        if (task == "CH01")
        {
            camnum[0]++;
            //LOG_INFO(QString("当前相机计数%1").arg(camnum[0]));
            writeLog(QString("onTaskFinished---相机1处理完成次数:%1").arg(camnum[0]),1);
            writeLog(QString("onTaskFinished---板卡1触发相机拍照次数:%1").arg(myBoardCard->camcons[0]),1);
            writeLog(QString("onTaskFinished---板卡1入料次数:%1").arg(myBoardCard->materials[0]),1);

            if (isStack)
            {
                writeLog(QString("onTaskFinished---相机%1叠料不吹气--板卡号%2，时间戳%3").arg(1).arg(myBoardCard->boards_number[0]).arg(timestamp),1);
                return;
            }

            if (result && beteldata)
            {
                writeLog("onTaskFinished---进入推理",1);
                auto results = output["beteldata"].value<QVector<VpAiInferResult>>();
                issueInstructions(outtest,camRatio[0],myBoardCard->boards_number[0],1,camnum[0],timestamp,results);
            }else
            {
                writeLog("onTaskFinished---推理/筛选失败",1);
                writeLog(QString("onTaskFinished--result:%1、beteldata:%2").arg(result).arg(beteldata),1);
                auto results = output["beteldata"].value<QVector<VpAiInferResult>>();
                //visionLoop2(myBoardCard->boards_number[0].toShort(), camnum[0],13,timestamp);
                //issueInstructions(14,camRatio[0],myBoardCard->boards_number[0],1,camnum[0],timestamp,results);
            }
        }

        if (task == "CH02")
        {
            camnum[1]++;
            //LOG_INFO(QString("当前相机计数%1").arg(camnum[0]));
            writeLog(QString("onTaskFinished---相机2处理完成次数:%1").arg(camnum[1]),2);
            writeLog(QString("onTaskFinished---板卡2触发相机拍照次数:%1").arg(myBoardCard->camcons[1]),2);
            writeLog(QString("onTaskFinished---板卡2入料次数:%1").arg(myBoardCard->materials[1]),2);

            if (isStack)
            {
                writeLog(QString("onTaskFinished---相机%1叠料不吹气--板卡号%2，时间戳%3").arg(2).arg(myBoardCard->boards_number[1]).arg(timestamp),2);
            }

            if (result && beteldata)
            {
                writeLog("onTaskFinished---进入推理",2);
                auto results = output["beteldata"].value<QVector<VpAiInferResult>>();
                issueInstructions(outtest,camRatio[1],myBoardCard->boards_number[1],2,camnum[1],timestamp,results);
            }else
            {
                writeLog("onTaskFinished---推理失败",2);
                writeLog(QString("onTaskFinished--result:%1、beteldata:%2").arg(result).arg(beteldata),2);
                auto results = output["beteldata"].value<QVector<VpAiInferResult>>();
                issueInstructions(14,camRatio[1],myBoardCard->boards_number[1],2,camnum[1],timestamp,results);
            }
        }


        if (task == "CH03")
        {
            camnum[2]++;
            //LOG_INFO(QString("当前相机计数%1").arg(camnum[0]));
            writeLog(QString("onTaskFinished---相机3处理完成次数:%1").arg(camnum[2]),3);
            writeLog(QString("onTaskFinished---板卡3触发相机拍照次数:%1").arg(myBoardCard->camcons[2]),3);
            writeLog(QString("onTaskFinished---板卡3入料次数:%1").arg(myBoardCard->materials[2]),3);

            if (isStack)
            {
                writeLog(QString("onTaskFinished---相机%1叠料不吹气--板卡号%2，时间戳%3").arg(3).arg(myBoardCard->boards_number[2]).arg(timestamp),3);
            }

            if (result && beteldata)
            {
                writeLog("onTaskFinished---进入推理",3);
                auto results = output["beteldata"].value<QVector<VpAiInferResult>>();
                issueInstructions(outtest,camRatio[2],myBoardCard->boards_number[2],3,camnum[2],timestamp,results);
            }else
            {
                writeLog("onTaskFinished---推理失败",3);
                writeLog(QString("onTaskFinished--result:%1、beteldata:%2").arg(result).arg(beteldata),3);
                auto results = output["beteldata"].value<QVector<VpAiInferResult>>();
                issueInstructions(14,camRatio[2],myBoardCard->boards_number[2],3,camnum[2],timestamp,results);
            }
        }



    } catch (std::exception ex) {
        writeLog(QString("输出解析失败,:%1").arg(ex.what()));
        VpMessageBox::Warning(this, "", QString("输出解析失败,:%1").arg(ex.what()));
    }
    /*camcons[0]++;
    std::lock_guard<std::mutex> lock(queueMutex);
    resultQueue.push(cameraResult(myBoardCard->boards_number[0].toShort(),camcons[0],4));
    LOG_INFO(QString::number( camcons[0]));*/

    qint64 stackElapsed = timerStack.elapsed();
    writeLog(QString("onTaskFinished耗时: %1 ms").arg(stackElapsed));
}


QStringList levelName{
        "访客",
        "OPN操作员",
        "OPN技师",
        "ME",
        "PE",
        "管理员",

};

void MainWindow::onUserLogin(VpUser user) {
    m_User = user;
    int level = user.Level();
    if (level > 5) {
        level = 5;
    }
    if (level < 0) {
        level = 0;
    }
    m_CurrentUser->setText(QString("用户：<font color='#ef5b9c'>%1【%2】</font>").arg(user.Name()).arg(levelName[level]));

}

#include "QThread"

void MainWindow::closeEvent(QCloseEvent *event) {
    if (!m_User.quit_software()) {
        VpMessageBox::Warning(this, tr("权限不足"), tr("您没有权限执行此操作"));
        event->ignore();
        return;
    }

    auto result = VpMessageBox::Question(this, tr("退出系统"), tr("您确定要退出系统吗？"));
    if (result == QMessageBox::Ok) {
        VisionProject::Current()->StopAndWait();
        if (myBoardCard) myBoardCard->close();
        LOG_INFO("板卡界面关闭");
        //if (comWindow) comWindow->close();
        LOG_INFO("通信界面关闭");
        if (vfdWindow) vfdWindow->close();
        LOG_INFO("变频器界面关闭");
        if (mesWindow) mesWindow->close();
        LOG_INFO("mes界面关闭");
        if (calibrationWindow) calibrationWindow->close();
        LOG_INFO("标定界面关闭");
        if (blsizeWindow) blsizeWindow->close();
        LOG_INFO("产品界面关闭");
        event->accept();
    } else {
        event->ignore();
    }
}


void MainWindow::onActionSwitchProjClicked() {
    if (!m_User.switch_project()) {
        VpMessageBox::Warning(this, tr("权限不足"), tr("您没有权限执行此操作"));
        return;
    }
    VpDialog::ShowDialog<FrmProjList>();

}

#include <QFileInfo>
#include <QDir>



std::wstring numberToExcelColumn(int num) {
    num += 1;//0 对应A
    std::wstring result;
    while (num > 0) {
        int remainder = (num - 1) % 26;
        result = static_cast<wchar_t>('A' + remainder) + result;
        num = (num - 1) / 26;
    }
    return result;
}




// void MainWindow::onBtnExportReport() {
//     QString dir = QString("%4/%1_%2_%3").arg(g_MachineInfo.ProductName)
//             .arg(g_MachineInfo.Lot)
//             .arg(g_MachineInfo.YieldNum).arg(g_MachineInfo.Dir);
//
//     QString filePath = QString("%6/report_%1_%2_%3_%4_%5.xlsx").arg(g_MachineInfo.MachineName).arg(
//                     g_MachineInfo.ProductName)
//             .arg(g_MachineInfo.Lot).arg(g_MachineInfo.YieldNum)
//             .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss")).arg(dir);
//
//     QFileInfo info(filePath);
//     auto dirInfo = info.dir();
//     dirInfo.makeAbsolute();
//     // 创建完整的目录路径（包括所有父级目录）
//     bool dirCreated = dirInfo.mkpath(".");
//     if (!dirCreated) {
//         VpMessageBox::Information(this, "", QString("路径创建失败:\n\t%1").arg(dirInfo.path()));
//     }
//     dirInfo.mkdir("MeasureData");
//     dirInfo.mkdir("DefectData");
//
//     auto endTime = QDateTime::currentDateTime();
//
//     auto stdFilePath = filePath.toStdWString();
//
//
//     Book *sumBook = createBook();
//     Sheet *sumSheet = sumBook->addSheet(L"Sheet1");
//     sumSheet->writeStr(2, 0, L"开始统计时间 :");
//     sumSheet->writeStr(2, 1, g_MachineInfo.StartTime.toString("yyyy-MM-dd HH:mm:ss").toStdWString().c_str());
//
//     sumSheet->writeStr(3, 0, L"结束统计时间 :");
//     sumSheet->writeStr(3, 0, endTime.toString("yyyy-MM-dd HH:mm:ss").toStdWString().c_str());
//
//     sumSheet->writeStr(4, 0, L"机台号 :");
//     sumSheet->writeStr(4, 1, g_MachineInfo.MachineName.toStdWString().c_str());
//
//     sumSheet->writeStr(5, 0, L"软件版本号 :");
//     sumSheet->writeStr(5, 1, g_MachineInfo.SoftwareVersion.toStdWString().c_str());
//
//     sumSheet->writeStr(6, 0, L"Lot号 : ");
//     sumSheet->writeStr(6, 1, g_MachineInfo.Lot.toStdWString().c_str());
//
//     sumSheet->writeStr(7, 0, L"产品型号 :");
//     sumSheet->writeStr(7, 1, g_MachineInfo.ProductName.toStdWString().c_str());
//
//     sumSheet->writeStr(8, 0, L"产品信息 :");
//
//     sumSheet->writeStr(9, 0, L"操作员 :");
//     sumSheet->writeStr(9, 1, g_MachineInfo.Operator.toStdWString().c_str());
//
//
//     sumSheet->writeStr(10, 0, L"单位 : ");
//     sumSheet->writeStr(10, 1, L"mm");
//
//
//     sumSheet->writeStr(13, 0, L"生产总数");
//     sumSheet->writeStr(14, 0, L"良品数");
//     sumSheet->writeStr(15, 0, L"良率");
//     sumSheet->writeStr(16, 0, L"不良数");
//
//     QMap<QString, NgCountMap> ngItems;
//     QSet<QString> ngNames;
//     auto sumCol = g_MachineInfo.WorkspaceName.size() + 1;
//     sumSheet->writeStr(12, sumCol, L"汇总数据");
//
//     sumSheet->writeFormula(13, sumCol, L"=B14");//总
//     sumSheet->writeFormula(14, sumCol,
//                            std::format(L"={}15", static_cast<wchar_t>(L'A' + sumCol - 1)).c_str());//良品数为最后一个相机ok的数据
//     auto sumColName = numberToExcelColumn(sumCol);//static_cast<wchar_t>(L'A' + sumCol);
//     sumSheet->writeFormula(15, sumCol, std::format(L"={}{}*100.0/{}{}", sumColName, 15, sumColName, 14).c_str());//良率
//     sumSheet->writeFormula(16, sumCol, std::format(L"={}{}-{}{}", sumColName, 14, sumColName, 15).c_str());//不良数
//     //  sumSheet->writeFormula(17, sumCol, std::format(L"=SUM(B17:{}17)",static_cast<wchar_t>(L'A' + sumCol - 1) ).c_str());//不良数
//
//
//     for (auto i = 0; i < g_MachineInfo.WorkspaceName.size(); ++i) {
//         auto tableName = g_MachineInfo.WorkspaceName[i];
//         auto result = queryDataByTimeRange(db, tableName, g_MachineInfo.StartTime, endTime);
//         auto col = i + 1;
//         sumSheet->writeStr(12, col, g_MachineInfo.WorkspaceName[i].toStdWString().c_str());
//         sumSheet->writeNum(13, col, result.totalCount);
//         sumSheet->writeNum(14, col, result.trueResultCount);
//         //sumSheet->writeNum(15, col, result.trueResultCount * 100.0f / result.totalCount);
//         auto colName = numberToExcelColumn(col);// static_cast<wchar_t>(L'A' + col);
//         sumSheet->writeFormula(15, col, std::format(L"={}{}*100.0/{}{}", colName, 15, colName, 14).c_str());
//         sumSheet->writeNum(16, col, result.totalCount - result.trueResultCount);
//
//         auto result2 = countNgItems(db, tableName, g_MachineInfo.StartTime, endTime);
//         ngItems[tableName] = result2;
//         ngNames.unite(result2.keys().toSet());
//     }
//
//     auto lstNgName = ngNames.values();
//     QMap<QString, int> nameMapRow;
//     for (int i = 0; i < lstNgName.size(); ++i) {//ng列表
//         auto ngName = lstNgName[i];
//         auto row = 17 + i;
//         nameMapRow.insert(ngName, row);
//         sumSheet->writeStr(row, 0, ngName.toStdWString().c_str());
//     }
//     for (auto i = 0; i < g_MachineInfo.WorkspaceName.size(); ++i) {//ng明细
//         auto tableName = g_MachineInfo.WorkspaceName[i];
//         QMap<QString, int> ngItem = ngItems[tableName];
//         auto col = i + 1;
//
//         QMapIterator<QString, int> it(ngItem);
//         while (it.hasNext()) {
//             it.next();
//             auto count = it.value();
//             auto row = nameMapRow[it.key()];
//             sumSheet->writeNum(row, col, count);
//
//         }
//
//         for (auto it = nameMapRow.begin(); it != nameMapRow.end(); ++it) {//写0
//             const QString &name = it.key();
//             int row = it.value();
//             if (!ngItem.contains(name)) {
//                 sumSheet->writeNum(row, col, 0);
//             }
//         }
//     }
//
//
//     bool rtn = sumBook->save(stdFilePath.c_str());
//     if (!rtn) {
//         VpMessageBox::Information(this, "", QString("数据导出失败:\n\t%1").arg(sumBook->errorMessage()));
//     }
//     sumBook->release();
//     sumSheet = nullptr;
//     sumBook = nullptr;
// //测量表
//
//     for (auto i = 0; i < g_MachineInfo.WorkspaceName.size(); ++i) {//导出测量流程
//         auto tableName = g_MachineInfo.WorkspaceName[i];
//         QString filePath = QString("%1/MeasureData/Measuredata_%2.xlsx").arg(dir).arg(tableName);
//         Book *measureBook = createBook();
//         Format *format = measureBook->addFormat();
//         format->setPatternBackgroundColor(COLOR_RED);
//
//         Sheet *measureSheet = measureBook->addSheet(L"Sheet1");
//
//         measureSheet->writeStr(2, 0, L"机台号 :");
//         measureSheet->writeStr(2, 1, g_MachineInfo.MachineName.toStdWString().c_str());
//
//         measureSheet->writeStr(3, 0, L"相机/工站号 :");
//         measureSheet->writeStr(3, 1, tableName.toStdWString().c_str());
//
//
//         measureSheet->writeStr(4, 0, L"视觉检测版本 :");
//         measureSheet->writeStr(4, 1, g_MachineInfo.Lot.toStdWString().c_str());
//
//         measureSheet->writeStr(5, 0, L"产品型号 :");
//         measureSheet->writeStr(5, 1, g_MachineInfo.ProductName.toStdWString().c_str());
//
//         measureSheet->writeStr(6, 0, L"lot号 :");
//         measureSheet->writeStr(6, 1, endTime.toString("yyyy-MM-dd HH:mm:ss").toStdWString().c_str());
//
//         measureSheet->writeStr(7, 0, L"开始统计时间 :");
//         measureSheet->writeStr(7, 1, g_MachineInfo.StartTime.toString("yyyy-MM-dd HH:mm:ss").toStdWString().c_str());
//
//         measureSheet->writeStr(8, 0, L"结束统计时间 :");
//         measureSheet->writeStr(8, 1, endTime.toString("yyyy-MM-dd HH:mm:ss").toStdWString().c_str());
//
//         measureSheet->writeStr(9, 0, L"操作员 :");
//         measureSheet->writeStr(9, 1, g_MachineInfo.Operator.toStdWString().c_str());
//
//         measureSheet->writeStr(10, 0, L"单位 : ");
//         measureSheet->writeStr(10, 1, L"mm");
//
//
//         measureSheet->writeStr(10, 0, L"单位 : ");
//         measureSheet->writeStr(10, 1, L"mm");
//
//
//         measureSheet->writeStr(13, 0, L"Summary:");
//
//         std::vector<std::wstring> title{
//                 L"Measurement items",
//                 L"LSL",
//                 L"Nominal",
//                 L"USL",
//                 L"Min",
//                 L"Ave",
//                 L"Max",
//                 L"Sigma",
//                 L"3Sigma",
//                 L"Cp",
//                 L"CPU",
//                 L"CPL",
//                 L"CPK",
//         };
//         for (int i = 0; i < title.size(); ++i) {
//             measureSheet->writeStr(14, i, title[i].c_str());
//         }
//         auto lstProduct = measureItems(db, tableName, g_MachineInfo.StartTime, endTime);
//         if (lstProduct.size() == 0) {
//             LOG_ERROR(QString("流程 %1 无产品数据").arg(tableName));
//             continue;
//         }
//         auto product = lstProduct[0];
//         int startItemRow = 15 + product.MeasureDetails.name.size() + 2;
//         int endItemRow = startItemRow + lstProduct.size();
//         int itemTitleRow = startItemRow - 1;//测量数据头
//
//         measureSheet->writeStr(itemTitleRow, 0, L"序号");
//         for (int i = 0; i < product.MeasureDetails.name.size(); ++i) {
//             auto name = product.MeasureDetails.name[i];
//             measureSheet->writeStr(15 + i, 0, name.toStdWString().c_str());
//             measureSheet->writeStr(itemTitleRow, i + 1, name.toStdWString().c_str());
//             auto row = 15 + i;
//
//             std::wstring range = std::format(L"{}{}:{}{}",
//                                              numberToExcelColumn(i + 1),//  static_cast<wchar_t>(L'B' + i),
//                                              startItemRow + 1,
//                                              numberToExcelColumn(i + 1),//  static_cast<wchar_t>(L'B' + i),
//                                              endItemRow);//B89:B12661
//             //LSL	Nominal	USL	Min	Ave	Max	Sigma	3Sigma	Cp	CPU	CPL	CPK
//             measureSheet->writeStr(row, 0, name.toStdWString().c_str());
//             measureSheet->writeNum(row, 1, product.MeasureDetails.min[i]);//LSL
//             measureSheet->writeNum(row, 2,
//                                    (product.MeasureDetails.min[i] + product.MeasureDetails.max[i]) * 0.5);//Nominal
//             measureSheet->writeNum(row, 3, product.MeasureDetails.max[i]);//USL
//
//             std::wstring min = std::format(L"=MIN({})", range);
//             auto formulaRow = row + 1;
//             measureSheet->writeFormula(row, 4, std::format(L"=MIN({})", range).c_str());//=MIN(B89:B12661)
//             measureSheet->writeFormula(row, 5, std::format(L"=AVERAGE({})", range).c_str());//=AVERAGE(B89:B12661)
//             measureSheet->writeFormula(row, 6, std::format(L"=MAX({})", range).c_str());//=MAX(B89:B12661)
//             measureSheet->writeFormula(row, 7, std::format(L"=STDEV({})", range).c_str());//=STDEV(B89:B12661)
//             measureSheet->writeFormula(row, 8, std::format(L"=3*STDEV({})", range).c_str());//=3*STDEV(B89:B12661)
//             measureSheet->writeFormula(row, 9, std::format(L"=SUM(-B{},D{})/(6*H{})", formulaRow, formulaRow,
//                                                            formulaRow).c_str());//=SUM(-B16,D16)/(6*H16)
//             measureSheet->writeFormula(row, 10, std::format(L"=SUM(D{},-F{})/(3*H{})", formulaRow, formulaRow,
//                                                             formulaRow).c_str());//=SUM(D16,-F16)/(3*H16)
//             measureSheet->writeFormula(row, 11, std::format(L"=SUM(F{},-B{})/(3*H{})", formulaRow, formulaRow,
//                                                             formulaRow).c_str());//=SUM(F16,-B16)/(3*H16)
//             measureSheet->writeFormula(row, 12,
//                                        std::format(L"=MIN(K{},L{})", formulaRow, formulaRow).c_str());//=MIN(K16,L16)
//
//
//         }
//
//         for (int i = 0; i < lstProduct.size(); ++i) {
//             auto p = lstProduct[i];
//             auto row = startItemRow + i;
//             measureSheet->writeStr(row, 0, p.SN.toStdWString().c_str());
//             for (auto j = 0; j < p.MeasureDetails.name.size(); ++j) {
//                 measureSheet->writeNum(row, j + 1, p.MeasureDetails.value[j]);
//                 if (!p.MeasureDetails.result[j]) {
//                     measureSheet->setCellFormat(row, j + 1, format);
//                 }
//             }
//
//
//         }
//
//         measureBook->save(filePath.toStdWString().c_str());
//         measureBook->release();
//
//
//     }
//
// }

#include <QSettings>

// void MainWindow::onBtnClearData() {
//
//
//     onBtnExportReport();
//
//     g_MachineInfo.StartTime = QDateTime::currentDateTime();
//     QSettings ini("./Setting.ini", QSettings::IniFormat);
//     ini.setIniCodec("utf-8");
//     ini.beginGroup("MachineInfo");
//     auto start = g_MachineInfo.StartTime.toString("yyyy-MM-dd hh:mm:ss");
//     ini.setValue("StartTime", start);
//     ini.endGroup();
//     ini.sync();
//
// }

void MainWindow::init() {
    // 中央部件
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 主垂直布局
    mainLayout = new QVBoxLayout(centralWidget);


    // 内容区域水平布局
    contentLayout = new QHBoxLayout();
    mainLayout->addLayout(contentLayout);

    //底部栏***********************************************************************************************************
    downLayout=new QHBoxLayout();
    mainLayout->addLayout(downLayout);

    // 设置标题栏和内容区的比例（垂直方向）
    //mainLayout->setStretch(0, 1);  // 标题栏
    mainLayout->setStretch(0, 20); // 内容区
    mainLayout->setStretch(1, 1); // 底部




    // 中间区域******************************************************************************************************
    middleWidget = new QWidget();
    QVBoxLayout *middleBoxLayout = new QVBoxLayout(middleWidget);
    QGroupBox *groupBox = new QGroupBox("");
    QHBoxLayout *middleLayout1 = new QHBoxLayout(groupBox);


    // 通道1
    QWidget *channelWidget1 = new QWidget;
    QVBoxLayout *channelLayout1 = new QVBoxLayout(channelWidget1);
    channelLayout1->setSpacing(0);
    channelLayout1->setContentsMargins(0, 0, 0, 0);

    passage1 = new QLabel("通道1");
    passage1->setAlignment(Qt::AlignCenter);
    passage1->setStyleSheet(R"(
    background-color: #2ecc71;
    color: black;
    font-size: 18px;
    padding: 6px;
)");

    ImageWidget *img1 = new ImageWidget(channelWidget1);
    img1->setWidgetName("img1");

    channelLayout1->addWidget(passage1);
    channelLayout1->addWidget(img1);

    // 通道2
    QWidget *channelWidget2 = new QWidget;
    QVBoxLayout *channelLayout2 = new QVBoxLayout(channelWidget2);
    channelLayout2->setSpacing(0);
    channelLayout2->setContentsMargins(0, 0, 0, 0);

    passage2 = new QLabel("通道2");
    passage2->setAlignment(Qt::AlignCenter);
    passage2->setStyleSheet(R"(
    background-color: #e74c3c;
    color: white;
    font-size: 18px;
    padding: 6px;
)");
    ImageWidget *img2 = new ImageWidget(channelWidget1);
    img2->setWidgetName("img2");

    channelLayout2->addWidget(passage2);
    channelLayout2->addWidget(img2);

    // 通道3
    QWidget *channelWidget3 = new QWidget;
    QVBoxLayout *channelLayout3 = new QVBoxLayout(channelWidget3);
    channelLayout3->setSpacing(0);
    channelLayout3->setContentsMargins(0, 0, 0, 0);

    passage3 = new QLabel("通道3");
    passage3->setAlignment(Qt::AlignCenter);
    passage3->setStyleSheet(passage1->styleSheet());

    ImageWidget *img3 = new ImageWidget(channelWidget1);
    img3->setWidgetName("img3");

    channelLayout3->addWidget(passage3);
    channelLayout3->addWidget(img3);

    // 加入横向布局
    middleLayout1->addWidget(channelWidget1);
    middleLayout1->addWidget(channelWidget2);
    middleLayout1->addWidget(channelWidget3);



    middleBoxLayout->addWidget(groupBox);


    QLabel *titleLabel = new QLabel("检测数据");
    titleLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    titleLabel->setStyleSheet("font-size: 35px; color: white;");
    middleTab = createDetectionDataTable();
    //middleBoxLayout->addWidget(titleLabel);
    middleBoxLayout->addWidget(middleTab);
    middleBoxLayout->addWidget(ui->groupBox_4);
   // middleBoxLayout->addStretch();

    middleBoxLayout->setStretch(0,5);
    middleBoxLayout->setStretch(1, 3);
    middleBoxLayout->setStretch(2, 2);
    //middleBoxLayout->setStretch(3, 2);


    contentLayout->addWidget(middleWidget);

    // 右状态栏******************************************************************************************************
    // rightWidget = new QWidget();
    // rightLayout = new QFormLayout(rightWidget);
    // rightLayout->addRow(ui->groupBox);
    // rightLayout->addRow(ui->groupBox_2);
    // rightLayout->addRow(ui->groupBox_4);
    // contentLayout->addWidget(rightWidget);
    rightWidget = new QWidget();
    rightLayout = new QVBoxLayout(rightWidget);
    rightWidget->setLayout(rightLayout);
    rightLayout->setSpacing(15);  // 整体控件间距
    rightLayout->setContentsMargins(10, 10, 10, 10);  // 外边距

    // 状态信息
    QWidget* statusInfoWidget = new QWidget();
    QVBoxLayout* infoLayout = new QVBoxLayout(statusInfoWidget);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(5);


    //启动停止
    buttonStatus = new QPushButton("启动");
    buttonStatus->setCheckable(true);  // 启用切换状态
    buttonStatus->setStyleSheet(R"(
    QPushButton {
        color: white;
        background-color: green;
        padding: 8px;
        font-size: 35px;
        border-radius: 15px;
    }
    QPushButton:checked {
        background-color: red;
    }
)");
    buttonStatus->setMinimumWidth(150);
    buttonStatus->setCursor(Qt::PointingHandCursor);
    buttonStatus->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  // 关键：允许垂直方向拉伸
    infoLayout->addWidget(buttonStatus);
    // 连接点击事件
    connect(buttonStatus, &QPushButton::clicked, this, [=](bool checked) {
        if (checked) {
            startFunction();
            start=1;
            buttonStatus->setText("停止");
        } else {
            stopFunction();
            start=0;
            buttonStatus->setText("启动");
        }
    });


    // 创建报警清除按钮
    QPushButton* alarmClear = new QPushButton("报警清除");
    alarmClear->setStyleSheet(R"(
    QPushButton {
        color: white;
        background-color: orange;
        padding: 8px;
        font-size: 35px;
        border-radius: 15px;
    }
)");
    alarmClear->setMinimumWidth(150);
    alarmClear->setCursor(Qt::PointingHandCursor);
    alarmClear->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  // 与上一个按钮保持一致
    //infoLayout->addWidget(alarmClear);

    // 可选：绑定点击事件
    connect(alarmClear, &QPushButton::clicked, this, [=]() {
        //clearAlarm();
    });
    infoLayout->addWidget(alarmClear);
    alarmClear->setVisible(false);

    // 清料
    clearMaterials = new QCheckBox("清料");
    clearMaterials->setStyleSheet("font-size: 12px; padding: 5px;");
    clearMaterials->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    infoLayout->addWidget(clearMaterials);

    // 勾选/取消勾选
    connect(clearMaterials, &QCheckBox::toggled, this, [=](bool checked) {
      if (checked) {
          clearStartTime = QDateTime::currentDateTime();
          LOG_INFO("清料开启");
      } else {
          clearStartTime = QDateTime();
          LOG_INFO("清料取消");
      }
  });


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateEncoderInfo);
    timer->start(1000);

    infoLayout->setStretch(0,1);
    infoLayout->setStretch(1,1);
    //infoLayout->setStretch(2,1);

    rightLayout->addWidget(statusInfoWidget);
    rightLayout->addSpacing(5);

     // 板卡相机状态
    rightLayout->addWidget(createStatusGroup("板卡状态", 3,1));
    rightLayout->addWidget(createStatusGroup("入料状态", 3,2));
    rightLayout->addWidget(createStatusGroup("相机状态", 3,3));



     rightLayout->addSpacing(5);




    //编码怕脉冲
    QGroupBox *groupBox1 = new QGroupBox("");
    QVBoxLayout* infoLayout2 = new QVBoxLayout(groupBox1);
    infoLayout2->setContentsMargins(0, 0, 0, 0);
    infoLayout2->setSpacing(5);
    labelEncoderPulse = new QLabel("编码器脉冲1：");
    labelEncoderPulse->setStyleSheet("color: white; background-color: #bbbbbb; padding: 8px;font-size: 23px;");
    infoLayout2->addWidget(labelEncoderPulse);

    labelEncoderPulse2 = new QLabel("编码器脉冲2：");
    labelEncoderPulse2->setStyleSheet("color: white; background-color: #bbbbbb; padding: 8px;font-size: 23px;");
    infoLayout2->addWidget(labelEncoderPulse2);


    labelEncoderPulse3 = new QLabel("编码器脉冲2：");
    labelEncoderPulse3->setStyleSheet("color: white; background-color: #bbbbbb; padding: 8px;font-size: 23px;");
    infoLayout2->addWidget(labelEncoderPulse3);

    rightLayout->addWidget(groupBox1);



     // 通道信息
     QWidget* channelInfoWidget = new QWidget();
     QVBoxLayout* channelLayout = new QVBoxLayout(channelInfoWidget);
     channelLayout->setContentsMargins(0, 0, 0, 0);
     channelLayout->setSpacing(8);

    channelSpeed = new QLabel(QString("通道1： --- Pcs/Min"));
    channelSpeed->setStyleSheet("color: white; background-color: #444444; padding: 6px; border: 1px solid white; font-size: 25px;");
    channelLayout->addWidget(channelSpeed);

    channelSpeed2 = new QLabel(QString("通道2： --- Pcs/Min"));
    channelSpeed2->setStyleSheet("color: white; background-color: #444444; padding: 6px; border: 1px solid white; font-size: 25px;");
    channelLayout->addWidget(channelSpeed2);

    channelSpeed3 = new QLabel(QString("通道3： --- Pcs/Min"));
    channelSpeed3->setStyleSheet("color: white; background-color: #444444; padding: 6px; border: 1px solid white; font-size: 25px;");
    channelLayout->addWidget(channelSpeed3);


    rightLayout->addWidget(channelInfoWidget);

    QWidget* testwight = new QWidget();
    QVBoxLayout* testloay = new QVBoxLayout(testwight);
    QLineEdit *test = new QLineEdit(testwight);
    QPushButton *testbot = new QPushButton("写入");
    testloay->addWidget(test);
    testloay->addWidget(testbot);
    connect(testbot, &QPushButton::clicked, this, [=]() {
        outtest=test->text().toInt();
        LOG_INFO(QString::number(outtest));
    });

    //if (m_User.Level()==5) {
        rightLayout->addWidget(testwight);
       // return;}




    rightLayout->setStretch(0, 3);  // 启动
    rightLayout->setStretch(2, 2);  // 板卡状态
    rightLayout->setStretch(3,2);  // DI0状态
    rightLayout->setStretch(4,2);  // 相机状态
    rightLayout->setStretch(6, 5);  //脉冲
    rightLayout->setStretch(7, 5);  // 通道



    contentLayout->addWidget(rightWidget);


    //contentLayout->setStretch(0, 2);  // 左侧
    contentLayout->setStretch(0, 10); // 中间
    contentLayout->setStretch(1, 2);  // 右侧
}




QGroupBox* MainWindow::createStatusGroup(const QString& title, int lightCount,int index) {
    QGroupBox* groupBox = new QGroupBox();
    groupBox->setStyleSheet(R"(
        QGroupBox {
            border: 2px solid white;
            border-radius: 2px;
        }
    )");
    QGridLayout* mainLayout = new QGridLayout(groupBox);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    //标题
    QLabel* titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("color: white; font-size: 20px;");
    mainLayout->addWidget(titleLabel, 0, 0, Qt::AlignLeft| Qt::AlignTop);

    //水平灯光
    QHBoxLayout* lightLayout = new QHBoxLayout();
    lightLayout->setSpacing(10);
    lightLayout->setContentsMargins(0, 0, 0, 0);

    if (index==1) {
        light1 = new QLabel();
        light1->setFixedSize(50, 50);
        light1->setStyleSheet("background-color: red; border-radius: 25px;");
        lightLayout->addWidget(light1);

        light2 = new QLabel();
        light2->setFixedSize(50, 50);
        light2->setStyleSheet("background-color: red; border-radius: 25px;");
        lightLayout->addWidget(light2);

        light3 = new QLabel();
        light3->setFixedSize(50, 50);
        light3->setStyleSheet("background-color: red; border-radius: 25px;");
        lightLayout->addWidget(light3);
    }
    else if (index==2) {
        light4 = new QLabel();
        light4->setFixedSize(50, 50);
        light4->setStyleSheet("background-color: red; border-radius: 25px;");
        lightLayout->addWidget(light4);

        light5 = new QLabel();
        light5->setFixedSize(50, 50);
        light5->setStyleSheet("background-color: red; border-radius: 25px;");
        lightLayout->addWidget(light5);

        light6 = new QLabel();
        light6->setFixedSize(50, 50);
        light6->setStyleSheet("background-color: red; border-radius: 25px;");
        lightLayout->addWidget(light6);
    }
    else{
        light7 = new QLabel();
        light7->setFixedSize(50, 50);
        light7->setStyleSheet("background-color: red; border-radius: 25px;");
        lightLayout->addWidget(light7);

        light8 = new QLabel();
        light8->setFixedSize(50, 50);
        light8->setStyleSheet("background-color: red; border-radius: 25px;");
        lightLayout->addWidget(light8);

        light9 = new QLabel();
        light9->setFixedSize(50, 50);
        light9->setStyleSheet("background-color: red; border-radius: 25px;");
        lightLayout->addWidget(light9);
    }



    QWidget* lightContainer = new QWidget();
    lightContainer->setLayout(lightLayout);
    mainLayout->addWidget(lightContainer, 1, 0);
    mainLayout->setRowStretch(0, 2);
    mainLayout->setRowStretch(1, 4);
    return groupBox;
}



QTableWidget* MainWindow::createDetectionDataTable() {
    int rows = 6;   // 3通道 * 2行（数量 + 占比）
    int cols = 16;  // 1列通道 + 1列类别 + 13个数据列

    QTableWidget *table = new QTableWidget(rows, cols);
    QFont tableFont("Microsoft YaHei", 9);
    QFont hFont = table->horizontalHeader()->font();
    QHeaderView *header2 = table->horizontalHeader();
    header2->setFixedHeight(80);  // 设置表头高度为 40px

    // 设置列头标签
    QStringList headers;
    headers << "通道" << "类别" << "来料总数" <<"拍照总数"<< "超长\nL(>6.5)" << "100元\nL(4.8-6.0)\nW(1.3-2.4)" << "长50元\nL(6.0-6.5)\nW(1.3-2.4)" << "笔杆\nL(<1.3)" << "50元\nL(4.3-4.8)\nW(1.3-2.4)"
    << "超短\nL(<3.5)"<<"200元\nL(5.3-5.7)\nW(1.3-2.4)" << "月亮弯/虫咬"<< "30元\nL(4.0-4.3)\nW(1.3-2.4)"<< "破废品"<< "10-20元\nL(3.5-4.0)\nW(1.3-2.4)" << "葫芦籽\nW(>2.4)"<< "笔杆\nL(<1.3)" ;

    //headers << "通道" << "类别" << "来料总数" <<"拍照总数"<< "超长\nL(>6.5)" << "长50元\nL(6.0-6.5)\nW(1.3-2.4)" << "100元\nL(4.8-6.0)\nW(1.3-2.4)"  << "50元\nL(4.3-4.8)\nW(1.3-2.4)" << "超短\nL(<3.5)"
    //<< "30元\nL(4.0-4.3)\nW(1.3-2.4)"<<"200元\nL(5.3-5.7)\nW(1.3-2.4)" << "10-20元\nL(3.5-4.0)\nW(1.3-2.4)"<< "葫芦籽\nW(>2.4)"<< "笔杆\nL(<1.3)"<< "月亮弯/虫咬" << "破废品" ;
    /*headers << "通道" << "类别" << "来料总数" <<"拍照总数"<< "10-20元\nL(3.5-4.0)\nW(1.3-2.4)" << "30元\nL(4.0-4.3)\nW(1.3-2.4)" << "50元\nL(4.3-4.8)\nW(1.3-2.4)" << "100元\nL(4.8-6.0)\nW(1.3-2.4)" << "200元\nL(5.3-5.7)\nW(1.3-2.4)" << "长50元\nL(6.0-6.5)\nW(1.3-2.4)"
    << "超短\nL(<3.5)"<<"超长\nL(>6.5)" << "葫芦籽\nW(>2.4)" << "笔杆\nL(<1.3)"<< "月亮弯/虫咬" << "破废品"  ;*/
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setStyleSheet("QHeaderView::section {"
                                            "font: bold 14px 'Microsoft YaHei';"
                                            "}");


    // 设置数据
    for (int i = 0; i < 3; ++i) {
        int baseRow = i * 2;

        // 通道列合并
        table->setSpan(baseRow, 0, 2, 1);
        QTableWidgetItem *channelItem = new QTableWidgetItem(QString("通道%1").arg(i + 1));
        channelItem->setTextAlignment(Qt::AlignCenter);
        table->setItem(baseRow, 0, channelItem);

        // 类别列
        QTableWidgetItem *quantityItem = new QTableWidgetItem("数量");
        quantityItem->setTextAlignment(Qt::AlignCenter);
        table->setItem(baseRow, 1, quantityItem);

        QTableWidgetItem *ratioItem = new QTableWidgetItem("占比");
        ratioItem->setTextAlignment(Qt::AlignCenter);
        table->setItem(baseRow + 1, 1, ratioItem);

        // 填写“数量”行数据
        // 填写“数量”行数据（居中）
        QStringList amountRow = {"0","0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0","0"};
        for (int j = 0; j < amountRow.size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(amountRow[j]);
            item->setTextAlignment(Qt::AlignCenter);  // 设置居中
            table->setItem(baseRow, j + 2, item);
        }

        // 填写“占比”行数据（居中）
        QStringList ratioRow = {"–", "–", "0%", "0%", "0%", "0%", "0%", "0%", "0%", "0%", "0%", "0%", "0%","0%"};
        for (int j = 0; j < ratioRow.size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(ratioRow[j]);
            item->setTextAlignment(Qt::AlignCenter);  // 设置居中
            table->setItem(baseRow + 1, j + 2, item);
        }

    }



    table->setFont(tableFont);
    table->verticalHeader()->hide();
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHeaderView *header = table->horizontalHeader();
    header->setFont(tableFont);
    return table;
}


void MainWindow::createImgWindow(int index) {
    // MyPassage* passage = new MyPassage(index);
    // myPassage.push_back(passage);
    // navList->addItem(passage->Item);
    // passage->imagesWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //
    // int count = myPassage.size();
    //
    // int cols = 3;
    // int rows = (count + cols - 1) / cols;
    // int remainder = count % cols;
    //
    // if (remainder != 0) {
    //     int cols2 = 2;
    //     int rows2 = (count + cols2 - 1) / cols2;
    //     int lastRowCount3 = remainder;
    //     int lastRowCount2 = count % cols2;
    //     if (lastRowCount2 > lastRowCount3) {
    //         cols = cols2;
    //         rows = rows2;
    //     }
    // }
    //
    // // 清理布局
    // QLayoutItem* item;
    // while ((item = middleLayout->takeAt(0)) != nullptr) {
    //     if (item->widget()) {
    //         item->widget()->setParent(nullptr);
    //     }
    //     delete item;
    // }
    //
    // for (int i = 0; i < count; ++i) {
    //     int row = i / cols;
    //     int col = i % cols;
    //     middleLayout->addWidget(myPassage[i]->imagesWindow, row, col);
    // }
}


void MainWindow::deleteImgWindow(int index) {
    // if (index < 0 || index >= myPassage.size()) return;
    //
    // MyPassage* passage = myPassage[index];
    //
    // middleLayout->removeWidget(passage->imagesWindow);
    // passage->imagesWindow->deleteLater();
    //
    // delete navList->takeItem(index);
    //
    // delete passage;
    // myPassage.erase(myPassage.begin() + index);
    //
    // relayoutAllImgWindows();
}



void MainWindow::initWindow(int nums) {
    // for (int i = 0; i < 3; ++i) {
    //     MyPassage* passage = new MyPassage(i);
    //     myPassage.push_back(passage);
    //
    //     navList->addItem(passage->Item);  // 如果你还需要显示在导航栏
    //     passage->imagesWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //
    //     middleLayout->addWidget(passage->imagesWindow, 0, i);  // 固定一行三列
    // }
}


void MainWindow::relayoutAllImgWindows() {
    // int count = myPassage.size();
    // if (count == 0) return;
    //
    // int cols = 3;
    // int rows = (count + cols - 1) / cols;
    // int remainder = count % cols;
    //
    // if (remainder != 0) {
    //     int cols2 = 2;
    //     int rows2 = (count + cols2 - 1) / cols2;
    //
    //     int lastRowCount3 = remainder;
    //     int lastRowCount2 = count % cols2;
    //
    //     if (lastRowCount2 > lastRowCount3) {
    //         cols = cols2;
    //         rows = rows2;
    //     }
    // }
    //
    // // 清理布局
    // QLayoutItem* item;
    // while ((item = middleLayout->takeAt(0)) != nullptr) {
    //     if (item->widget()) {
    //         item->widget()->setParent(nullptr);
    //     }
    //     delete item;
    // }
    //
    // for (int i = 0; i < count; ++i) {
    //     int row = i / cols;
    //     int col = i % cols;
    //     middleLayout->addWidget(myPassage[i]->imagesWindow, row, col);
    // }
}



void MainWindow::test() {
    // unsigned short Connection_Number = 1;
    // unsigned short Station_Number[2] = {0, 1};
    // unsigned short Station_Type[2] = {2, 2};
    //
    // short ret3=MCF_Sorting_Init_Net(0);
    // printf("ret3=%d\n",ret3);
    //
    // // unsigned short *i =2;
    // short ret=MCF_Open_Net(Connection_Number,Station_Number,Station_Type);
    //
    // short ret2=MCF_Get_Link_State_Net(0);
    // if (ret2 == 0) {
    //
    // }
    //
    // // ret2=MCF_Set_Trigger_Output_Bit_Net(0,1,0);
    // printf("ret2=%d\n",ret2);
    //
    // //ret2=MCF_Get_Output_Bit_Net(0,0);
    // printf("ret2=%d\n",ret2);
    //
    // short ret4=MCF_Set_Output_Bit_Net(1,0,0);
    // printf("ret4=%d\n",ret4);
    //
    // ret4=MCF_Set_Output_Bit_Net(1,1,0);
    // printf("ret4=%d\n",ret4);
    //
    // ret4= MCF_Set_Output_Net(0xffffffff,0);
    // printf("ret4=%d\n",ret4);
    //
    // ret4= MCF_Set_Output_Net(00000000,0);
    // printf("ret4=%d\n",ret4);
}

void MainWindow::updateEncoderInfo() {

    short ret=MCF_Get_Link_State_Net();
    if (ret ==0) {
        labelEncoderPulse->setText(QString("编码器脉冲%1：%2").arg(passage).arg(myBoardCard->Encoders[0]));
        labelEncoderPulse2->setText(QString("编码器脉冲%1：%2").arg(passage+1).arg(myBoardCard->Encoders[1]));
        labelEncoderPulse3->setText(QString("编码器脉冲%1：%2").arg(passage+2).arg(myBoardCard->Encoders[2]));
        channelSpeed->setText(QString("通道%1：%2 Pcs/Min").arg(passage).arg(myBoardCard->channelSpeed[0]));
        channelSpeed2->setText(QString("通道%1：%2 Pcs/Min").arg(passage+1).arg(myBoardCard->channelSpeed[1]));
        channelSpeed3->setText(QString("通道%1：%2 Pcs/Min").arg(passage+2).arg(myBoardCard->channelSpeed[2]));
    }else {
        labelEncoderPulse->setText(QString("编码器脉冲%1：%2").arg(passage).arg("---"));
        labelEncoderPulse2->setText(QString("编码器脉冲%1：%2").arg(passage+1).arg("---"));
        labelEncoderPulse3->setText(QString("编码器脉冲%1：%2").arg(passage+2).arg("---"));
        channelSpeed->setText(QString("通道%1：--- Pcs/Min").arg(passage));
        channelSpeed2->setText(QString("通道%1：--- Pcs/Min").arg(passage+1));
        channelSpeed3->setText(QString("通道%1：--- Pcs/Min").arg(passage+2));
    }

}


void MainWindow::startFunction() {

    //saveToCSV(middleTab);

    /*if (myBoardCard->openBoard() != 0) {
        return;
    }

    if (myBoardCard->InitBoardConfig() != 0) {
        return;
    }*/

    if(myBoardCard->stopSift() !=0) {
        //return;
        LOG_INFO("启动前关闭筛选失败");
        writeLog("启动前关闭筛选失败");
    }

    /*Sleep(200);
    if (myBoardCard->clearcount() !=0){
        LOG_INFO("启动前编码器清零");
        writeLog("启动前编码器清零");
    }*/

    Sleep(800);

    VisionCore::Project::Run();
    LOG_INFO("开启视觉");
    writeLog("[startFunction]开启视觉");

    myBoardCard->setCameraParameter();
    myBoardCard->setValveParameter();



    if (myBoardCard->openSift() != 0) {
        LOG_INFO("开启筛选失败");
        writeLog("[startFunction]开启筛选失败");
        //return;
    }
    LOG_INFO("开启筛选成功");
    writeLog("[startFunction]开启筛选成功");
    /*if (vfdWindow->initSerial() != 0) {
        return;
    }*/

    for (int i=0;i<myBoardCard->boards_counts.toInt();i++) {
        cams[i]=myBoardCard->camcons[i];
        camnum[i]=myBoardCard->camcons[i];
    }


    if (passage==1)
    {
         vfdWindow->initSerial3();
        LOG_INFO("开启电机");
        writeLog("[startFunction]开启电机");
    }

    writeLog(QString("启动---板卡1入料次数:%1").arg(myBoardCard->materials[0]),1);
    writeLog(QString("启动---板卡2入料次数:%1").arg(myBoardCard->materials[1]),2);
    writeLog(QString("启动---板卡3入料次数:%1").arg(myBoardCard->materials[2]),3);
}

void MainWindow::stopFunction() {

    if (passage==1)
    {
        vfdWindow-> initSerialStop();
        LOG_INFO("电机停止");
        writeLog("[stopFunction]电机停止");
    }

    if(myBoardCard->stopSift() !=0) {
        //return;
        LOG_INFO("关闭筛选失败");
        writeLog("[stopFunction]关闭筛选失败");
    }
    LOG_INFO("关闭筛选成功");
    writeLog("[stopFunction]关闭筛选成功");

    VisionCore::Project::Stop();



    /*if(myBoardCard->stopBoard()==0) {
        writeLogToFile("[stopFunction]板卡初始化");
        Sleep(800);
        myBoardCard->openBoard();
        myBoardCard->InitBoardConfig();
        writeLogToFile("[stopFunction]板卡初始化");
    }*/



}



void MainWindow::handleLog(const QString &msg)
{
    qDebug() << "[Log] " << msg;
}


// 线程主循环函数
void MainWindow::visionLoop()
{
    int Result = 0;
    int index = 0; // 假设用0
    while (true) {
        if (threadRunning == 0)
            return;

        int board = boards_number[index];
        int countIndex = Comprehensive_1_Count_Dly[index] & 0xff;

        // 索引边界检查（可选）
        if (board < 0 || board >= 3) {
            // 错误处理
            continue;
        }

        if (Visual_Camera_Tri_Count_Result[board][countIndex] == 1) {
            Result = 0;  // OK
        } else {
            Result = 1;  // NG or other
        }

        Visual_Camera_Tri_Count_Result[board][countIndex] = 0;

        Comprehensive_1_Count_Dly[index]++;

        if (Result == 1) {
            MCF_Sorting_Set_Trig_Blow_Net(1, Comprehensive_1_Count_Dly[index], board);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}



void MainWindow::visionLoop(short stationNumber, int photoCount, int photoResult)
{
   short ret= MCF_Sorting_Set_Trig_Blow_Net(photoResult, photoCount, stationNumber);
    if (ret==0) {
        QString log = QString("已经下发吹气：板卡号：%1\n拍照计数：%2\n气阀号：%3").arg(stationNumber).arg(photoCount).arg(photoResult);
        writeLog(log);
        myBoardCard->writeLogToFile(log);
    }
     //myBoardCard->Get_Trig_Blow();
}

void MainWindow::visionLoop2(short stationNumber, int photoCount, int photoResult, QString timestamp,int cam)
{
    short ret= MCF_Sorting_Set_Trig_Blow_Net(photoResult, photoCount, stationNumber);
    if (ret==0) {
        QString log = QString("[visionLoop]下发吹气命令成功：板卡号：%1，拍照计数：%2，气阀号：%3，时间戳%4").arg(stationNumber).arg(photoCount).arg(photoResult).arg(timestamp);
        writeLog(log,cam);//气阀1触发
        myBoardCard->writeLogToFile(log);
    }
    //myBoardCard->Get_Trig_Blow();
}




// 启动线程
void MainWindow::startVisionThread()
{
    threadRunning = true;
    visionThread = QThread::create([=]() {
        while (threadRunning) {
        checkCameraQueue();
        QThread::msleep(5);
    }
    });

    visionThread->start();
}

// 停止线程
void MainWindow::stopVisionThread()
{
    threadRunning = false;

    if (visionThread) {
        visionThread->quit();
        visionThread->wait();
        delete visionThread;
        visionThread = nullptr;
    }
}



// 模拟拍照
int MainWindow::getFakeCameraResult(int index)
{
    if (index % 5 == 0) return 2;
    else return 1;
}


int MainWindow::classify(double &diameter, double &length,int category,int category2,int category3) {
    /*if (category)
        length-=diff;*/
    diameter/=10.0;
    length/=10.0;
    if (category==1)//花苞
    {
        if (length>6.3) length-=0.45;
        else if (length>5.1) length-=0.3;
        else  length-=0.25;

    }
    /*if (category2==4)
    {
        diameter-=0.2;
    }*/
    //diameter-=0.05;
    writeLog(QString("classify:宽%1、长%2").arg(diameter).arg(length));
    LOG_INFO(QString("宽%1、长%2").arg(diameter).arg(length));


    // 虫咬
    if ( category2==3) {
        return 9;
    }

    // 月亮
    if ( category2==2&&category3==5) {
        return 9;
    }

    // 破废
    if (category2 == 5) {
        return 11;
    }

    // 超短
    if (length < 3.5) {
        return 7;
    }

    // 超长
    if (length >6.5) {
        return 2;
    }

    // 葫芦
    if (category2==4 ||
        (diameter > 2.5)) {
        return 13;
        }

    // 笔杆
    if (diameter < 1.3) {
        return 5;
    }

    // 200元
    if (category2==6 &&
        diameter >= 1.3 && diameter <= 2.4 &&
        (length >= 5.3 && length < 5.7)) {
        return 8;
        }

    // 10-20
    if (diameter >= 1.3 && diameter <= 2.4 &&
        ((length >= 3.5 && length < 3.95))) {
        return 12;
        }

    // 30元
    if (diameter >= 1.3 && diameter <= 2.4 &&
        ((length >= 4.0 && length < 4.25))) {
        return 10;
        }

    // 50元
    if (diameter >= 1.3 && diameter <= 2.4 &&
        ((length >=4.3 && length < 4.8))) {
        return 6;
        }

    // 100元
    if (diameter >= 1.3 && diameter <= 2.4 &&
        ((length >= 4.8 && length < 6.0))) {
        return 3;
        }



    // 长50元
    if (diameter >= 1.3 && diameter <= 2.4 &&
        ((length >= 6.0 && length < 6.5))) {
        return 4;
        }


    diameter*=10.0;
    length*=10.0;
    return 14;
}



void MainWindow::checkCameraQueue() {
    cameraResult outResult;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (resultQueue.empty()) {
            return;
        }
        outResult = resultQueue.front();
        resultQueue.pop();
        visionLoop(outResult.stationNumber, outResult.photoCount, outResult.photoResult);
    }

}


void MainWindow::updateLight() {

    //板卡灯光
    if (myBoardCard->BoarState==0) {
        light1->setStyleSheet("background-color: green; border-radius: 25px;");
        light2->setStyleSheet("background-color: green; border-radius: 25px;");
        light3->setStyleSheet("background-color: green; border-radius: 25px;");
    }else {
        light1->setStyleSheet("background-color: red; border-radius: 25px;");
        light2->setStyleSheet("background-color: red; border-radius: 25px;");
        light3->setStyleSheet("background-color: red; border-radius: 25px;");
    }




    /*
    //入料灯光
    if (myBoardCard->DI0Status[0]==0) {
        //int num=data1[0][0].toInt();
        light4->setStyleSheet("background-color: green; border-radius: 25px;");
        //data1[0][0]=QString::number(num+1);
        //updataTab1();
        if (clearMaterials) clearStartTime = QDateTime::currentDateTime();
    }else if (myBoardCard->DI0Status[0]==1) {
        light4->setStyleSheet("background-color: red; border-radius: 25px;");
    }

    if (myBoardCard->DI0Status[1]==0) {
        //int num=data1[1][0].toInt();
        light5->setStyleSheet("background-color: green; border-radius: 25px;");
        //data1[1][0]=QString::number(num+1);
        //updataTab1();
        if (clearMaterials) clearStartTime = QDateTime::currentDateTime();

    }else if (myBoardCard->DI0Status[1]==1) {
        light5->setStyleSheet("background-color: red; border-radius: 25px;");
    }

    if (myBoardCard->DI0Status[2]==0) {
        //int num=data1[2][0].toInt();
        light6->setStyleSheet("background-color: green; border-radius: 25px;");
        //data1[2][0]=QString::number(num+1);
        //updataTab1();
        if (clearMaterials) clearStartTime = QDateTime::currentDateTime();
    }else if (myBoardCard->DI0Status[2]==1) {
        light6->setStyleSheet("background-color: red; border-radius: 25px;");
    }
    */



    /*static int lastStatus[3] = {1, 1, 1};
    for (int i = 0; i < 3; i++) {
        int currentStatus = myBoardCard->DI0Status[i];

        if (currentStatus == 0) {
            // 状态从 1 变 0 时才加
            if (lastStatus[i] == 1) {
                int num = data1[i][0].toInt();
                data1[i][0] = QString::number(num + 1);
                updataTab1();
                if (clearMaterials) {
                    clearStartTime = QDateTime::currentDateTime();
                }
            }
            // 灯变绿
            if (i == 0) light4->setStyleSheet("background-color: green; border-radius: 25px;");
            if (i == 1) light5->setStyleSheet("background-color: green; border-radius: 25px;");
            if (i == 2) light6->setStyleSheet("background-color: green; border-radius: 25px;");
        }
        else { // currentStatus == 1
            // 灯变红
            if (i == 0) light4->setStyleSheet("background-color: red; border-radius: 25px;");
            if (i == 1) light5->setStyleSheet("background-color: red; border-radius: 25px;");
            if (i == 2) light6->setStyleSheet("background-color: red; border-radius: 25px;");
        }
        // 更新上一次状态
        lastStatus[i] = currentStatus;
    }*/








    //相机灯光
    auto cameras = VisionCore::Camera::List();
    for (int i=0; i<cameras.size(); i++) {
        auto isConnect = VisionCore::Camera::IsConnected(cameras[i]);
        if (isConnect&&i==0) {
            light7->setStyleSheet("background-color: green; border-radius: 25px;");
        }else if (isConnect&&i==1) {
            light8->setStyleSheet("background-color: green; border-radius: 25px;");
        }else if (isConnect&&i==2) {
            light9->setStyleSheet("background-color: green; border-radius: 25px;");
        }
    }

}








void MainWindow::updatedata() {
    middleTab->item(0, 2)->setText(QString::number(myBoardCard->materials[0]));
    middleTab->item(0, 3)->setText(QString::number(myBoardCard->camcons[0]));

    middleTab->item(2, 2)->setText(QString::number(myBoardCard->materials[1]));
    middleTab->item(2, 3)->setText(QString::number(myBoardCard->camcons[1]));

    middleTab->item(4, 2)->setText(QString::number(myBoardCard->materials[2]));
    middleTab->item(4, 3)->setText(QString::number(myBoardCard->camcons[2]));
}


/*void MainWindow::updataTab1(bool isStart) {
    if (myBoardCard->BoarState==0 || isStart)
    {
       // LOG_INFO("表1更新");
        int materialsnum1=myBoardCard->materials[0]+materials[0];
        //LOG_INFO(QString("通道1入料计数-%1").arg(materialsnum1));
        data1[0][0]=QString::number(materialsnum1);
        middleTab->item(0, 2)->setText(QString::number(materialsnum1));
        int materialsnum2=myBoardCard->materials[1]+materials[1];
        //LOG_INFO(QString("通道2入料计数-%1").arg(materialsnum2));
        data2[0][0]=QString::number(materialsnum2);
        middleTab->item(2, 2)->setText(QString::number(materialsnum2));
        int materialsnum3=myBoardCard->materials[2]+materials[2];
        //LOG_INFO(QString("通道3入料计数-%1").arg(materialsnum3));
        data3[0][0]=QString::number(materialsnum3);
        middleTab->item(4, 2)->setText(QString::number(materialsnum3));

        int camconsnum1=myBoardCard->camcons[0]+camcons[0];
        //LOG_INFO(QString("通道1拍照计数-%1").arg(camconsnum1));
        data1[0][1]=QString::number(camconsnum1);
        middleTab->item(0, 3)->setText(QString::number(camconsnum1));
        int camconsnum2=myBoardCard->camcons[1]+camcons[1];
        //LOG_INFO(QString("通道2拍照计数-%1").arg(camconsnum2));
        data2[0][1]=QString::number(camconsnum2);
        middleTab->item(2, 3)->setText(QString::number(camconsnum2));
        int camconsnum3=myBoardCard->camcons[2]+camcons[2];
        //LOG_INFO(QString("通道3拍照计数-%1").arg(camconsnum3));
        data3[0][1]=QString::number(camconsnum3);
        middleTab->item(4, 3)->setText(QString::number(camconsnum3));
    }

}*/


/*void MainWindow::updataTab1(bool isStart) {
    if (myBoardCard->BoarState==0 || isStart)
    {
        // LOG_INFO("表1更新");
        int materialsnum1=materials[0];
        //LOG_INFO(QString("通道1入料计数-%1").arg(materialsnum1));
        data1[0][0]=QString::number(materialsnum1);
        middleTab->item(0, 2)->setText(QString::number(materialsnum1));
        int materialsnum2=materials[1];
        //LOG_INFO(QString("通道2入料计数-%1").arg(materialsnum2));
        data2[0][0]=QString::number(materialsnum2);
        middleTab->item(2, 2)->setText(QString::number(materialsnum2));
        int materialsnum3=materials[2];
        //LOG_INFO(QString("通道3入料计数-%1").arg(materialsnum3));
        data3[0][0]=QString::number(materialsnum3);
        middleTab->item(4, 2)->setText(QString::number(materialsnum3));

        int camconsnum1=camcons[0];
        //LOG_INFO(QString("通道1拍照计数-%1").arg(camconsnum1));
        data1[0][1]=QString::number(camconsnum1);
        middleTab->item(0, 3)->setText(QString::number(camconsnum1));
        int camconsnum2=camcons[1];
        //LOG_INFO(QString("通道2拍照计数-%1").arg(camconsnum2));
        data2[0][1]=QString::number(camconsnum2);
        middleTab->item(2, 3)->setText(QString::number(camconsnum2));
        int camconsnum3=camcons[2];
        //LOG_INFO(QString("通道3拍照计数-%1").arg(camconsnum3));
        data3[0][1]=QString::number(camconsnum3);
        middleTab->item(4, 3)->setText(QString::number(camconsnum3));
    }

}*/


void MainWindow::updataTab1(bool isStart) {
    //if (myBoardCard->BoarState==0 || isStart)
   // {
        for (int i=2; i<4; i++)
        {
            int count1=data1[0][i-2].toInt();
            int count2=data2[0][i-2].toInt();
            int count3=data3[0][i-2].toInt();
            middleTab->item(0, i)->setText(QString::number(count1));
            middleTab->item(2, i)->setText(QString::number(count2));
            middleTab->item(4, i)->setText(QString::number(count3));
        }
    //}
}


void MainWindow::updataTab2(bool isStart) {
  //  if (myBoardCard->BoarState==0 || isStart)
   // {
        for (int i=4; i<16; i++)
        {
            int count1=data1[0][i-2].toInt();
            int count2=data2[0][i-2].toInt();
            int count3=data3[0][i-2].toInt();
            middleTab->item(0, i)->setText(QString::number(count1));
            middleTab->item(2, i)->setText(QString::number(count2));
            middleTab->item(4, i)->setText(QString::number(count3));
        }
        //更新百分比
        updataTabProportion();
  //  }
}



void MainWindow::updataTabValue(int station,int index,int value)
{
    if (index==-1) return;
    if (station == myBoardCard->boards_number[0].toInt())
    {
        data1[0][index] =QString::number(data1[0][index].toInt() + value);
        data1[0][0]=QString::number(data1[0][0].toInt() + value);
        data1[0][1]=QString::number(data1[0][1].toInt() + value);
    }
    if (station == myBoardCard->boards_number[1].toInt())
    {
        data2[0][index] =QString::number(data2[0][index].toInt() + value);
        data2[0][0]=QString::number(data2[0][0].toInt() + value);
        data2[0][1]=QString::number(data2[0][1].toInt() + value);
    }
    if (station == myBoardCard->boards_number[2].toInt())
    {
        data3[0][index] =QString::number(data3[0][index].toInt() + value);
        data3[0][0]=QString::number(data3[0][0].toInt() + value);
        data3[0][1]=QString::number(data3[0][1].toInt() + value);
    }
    //LOG_INFO(QString("计数+%1，板卡号%2，气阀号%3").arg(value).arg(station).arg(index));
}


void MainWindow::updataTabProportion()
{
    double num1=data1[0][1].toDouble();
    double num2=data2[0][1].toDouble();
    double num3=data3[0][1].toDouble();
    for (int i=4; i<13; i++)
    {
        double count1=data1[0][i-2].toDouble();
        double count2=data2[0][i-2].toDouble();
        double count3=data3[0][i-2].toDouble();
        if (num1!=0)
        {
            double percent1 = count1 / num1 * 100;
            middleTab->item(1, i)->setText(QString::number(percent1, 'f', 2) + "%");
        }
        if (num2!=0)
        {
            double percent2 = count2 / num2 * 100;
            middleTab->item(3, i)->setText(QString::number(percent2, 'f', 2) + "%");
        }
        if (num3!=0)
        {
            double percent3 = count3 / num3 * 100;
            middleTab->item(5, i)->setText(QString::number(percent3, 'f', 2) + "%");
        }

    }
}

void MainWindow::savepData() {
    QFileInfo fileInfo(pathFile);
    QDir dir = fileInfo.dir();

    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            return;
        }
    }

    QSettings settings(pathFile, QSettings::IniFormat);
    settings.beginGroup("data");

    // 将 QVector<int> 转为字符串列表再保存
    QStringList list1, list2, list3;
    for (int val : materialCount1) list1 << QString::number(val);
    for (int val : materialCount2) list2 << QString::number(val);
    for (int val : materialCount3) list3 << QString::number(val);

    settings.setValue("materialCount1", list1.join(","));
    settings.setValue("materialCount2", list2.join(","));
    settings.setValue("materialCount3", list3.join(","));

    settings.endGroup();
}

void MainWindow::loadpData() {
    /*QSettings settings(pathFile, QSettings::IniFormat);
    settings.beginGroup("data");

    // 先读为 QStringList
    QStringList list1 = settings.value("materialCount1").toString().split(",", Qt::SkipEmptyParts);
    QStringList list2 = settings.value("materialCount2").toString().split(",", Qt::SkipEmptyParts);
    QStringList list3 = settings.value("materialCount3").toString().split(",", Qt::SkipEmptyParts);

    settings.endGroup();

    // 然后将 QStringList 转为 QVector<int>
    for (int i = 0; i < list1.size() && i < materialCount1.size(); ++i) {
        materialCount1[i] = list1[i].toInt();
    }
    for (int i = 0; i < list2.size() && i < materialCount2.size(); ++i) {
        materialCount2[i] = list2[i].toInt();
    }
    for (int i = 0; i < list3.size() && i < materialCount3.size(); ++i) {
        materialCount3[i] = list3[i].toInt();
    }*/
}

void MainWindow::saveToCSV(QTableWidget *table) {
    // 构建目录
    QString directory = "D:/Data/Yeild";
    QDir dir;
    if (!dir.exists(directory)) {
        dir.mkpath(directory);
    }

    // 文件路径（最新文件）
    QString fileLatest = QDir(directory).filePath(QString("Yeild-%1.csv").arg(QDate::currentDate().toString("yyyyMMdd")));

    // lambda 写 CSV，支持特殊字符转义
    auto writeCSV = [table](const QString &filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            qWarning() << "无法打开文件：" << filePath;
            return;
        }

        QTextStream stream(&file);

        // 写列头
        for (int col = 0; col < table->columnCount(); ++col) {
            QString header = table->horizontalHeaderItem(col) ? table->horizontalHeaderItem(col)->text() : "";
            header.replace("\"", "\"\""); // 转义引号
            stream << "\"" << header << "\"";
            if (col < table->columnCount() - 1) stream << ",";
        }
        stream << "\n";

        // 写数据行
        for (int row = 0; row < table->rowCount(); ++row) {
            for (int col = 0; col < table->columnCount(); ++col) {
                QString cellText = table->item(row, col) ? table->item(row, col)->text() : "";
                if ((row % 2 != 0) && col == 0) cellText = ""; // 奇数行第一列空
                cellText.replace("\"", "\"\""); // 转义引号
                stream << "\"" << cellText << "\"";
                if (col < table->columnCount() - 1) stream << ",";
            }
            stream << "\n";
        }

        file.close();
    };

    // 写最新文件
    writeCSV(fileLatest);
}




void MainWindow::loadCSVToTable(QTableWidget *middleTab) {
    if (!middleTab) return;

    // 构建今天的文件路径
    QString dirPath = "D:/Data/Yeild";
    QString fileName = QString("Yeild-%1.csv").arg(QDate::currentDate().toString("yyyyMMdd"));
    QString filePath = QDir(dirPath).filePath(fileName);

    QFile file(filePath);
    if (!file.exists()) {
        //文件不存在
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //无法打开文件
        return;
    }

    QTextStream in(&file);
    int row = 0;

    while (!in.atEnd()) {
        QString line = in.readLine();

        // 跳过表头
        if (row == 0) {
            row++;
            continue;
        }

        QStringList fields = line.split(",");

        // 从第二行开始写入数据
        if (row > 0) {
            for (int col = 2; col < fields.size(); ++col) {
                // 数据从 middleTab 的第一行第三列开始
                QTableWidgetItem *newItem = new QTableWidgetItem(fields[col]);
                newItem->setTextAlignment(Qt::AlignCenter);
                middleTab->setItem(row - 1, col - 1, newItem);
            }
        }
        row++;
    }

    file.close();
}

void MainWindow::loadIniToTable(QTableWidget *middleTab)
{
    if (!middleTab) return;

    QString dirPath = "D:/Data/Yeild";
    QString fileName = QString("Yeild-%1.txt").arg(QDate::currentDate().toString("yyyyMMdd"));
    QString filePath = QDir(dirPath).filePath(fileName);

    QFileInfo checkFile(filePath);
    if (!checkFile.exists()) {
        return;
    }

    QSettings settings(filePath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8"); // 保证中文显示



    QStringList headers = {"MaterialTotal","PhotoTotal","ExtraLong","100","Long50","PenBarrel","50","SuperShort","200","Moon","30","Damaged","10-20","Gourd"};

    for (int ch = 0; ch < 3; ++ch) {
        int baseRow = ch * 2;       // 数量行
        int ratioRow = baseRow + 1; // 占比行

        QString section = QString("Channel%1").arg(ch + 1);
        settings.beginGroup(section);

        for (int col = 0; col < headers.size(); ++col) {
            int tableCol = col + 2; // 表格前两列是通道和类别
            if (tableCol >= middleTab->columnCount()) continue;

            if (headers[col] == "MaterialTotal" || headers[col] == "PhotoTotal") {
                QString value = settings.value(headers[col], "").toString();
                if (value.isEmpty()) value = "–";
                middleTab->setItem(baseRow, tableCol, new QTableWidgetItem(value));
                middleTab->setItem(ratioRow, tableCol, new QTableWidgetItem("–"));
            } else {
                QString countValue = settings.value(headers[col] + "_Count", "").toString();
                QString ratioValue = settings.value(headers[col] + "_Proportion", "").toString();

                if (countValue.isEmpty()) countValue = "–";
                if (ratioValue.isEmpty()) ratioValue = "–";

                middleTab->setItem(baseRow, tableCol, new QTableWidgetItem(countValue));
                middleTab->setItem(ratioRow, tableCol, new QTableWidgetItem(ratioValue));
            }
        }

        settings.endGroup();
    }
}


void MainWindow::loadTableToData(QTableWidget *middleTab)
{
    int startCol = 2;   // "来料总数"
    int endCol   = 15;  // "E5"

    // 清空旧数据
    data1.clear();
    data2.clear();
    data3.clear();

    auto extractChannelData = [&](int rowStart, int rowEnd, QVector<QStringList> &target) {
        for (int r = rowStart; r < rowEnd; ++r) {
            QStringList row;

            // 提取表格数据
            for (int c = startCol; c <= endCol; ++c) {
                QTableWidgetItem *item = middleTab->item(r, c);
                QString text = (item ? item->text() : "0");

                // 如果是 "-" 就转为 "0"
                if (text == "-") text = "0";
                row << text;
            }
            row << "0" << "0" << "0";

            target.append(row);
        }
    };

    // 通道1：第0、1行
    extractChannelData(0, 2, data1);
    // 通道2：第2、3行
    extractChannelData(2, 4, data2);
    // 通道3：第4、5行
    extractChannelData(4, 6, data3);

}







void MainWindow::saveDataToIni2(QTableWidget *middleTab)
{
    if (!middleTab) return;

    QString dirPath = "D:/Data/Yeild";
    QDir dir;
    if (!dir.exists(dirPath)) {
        if (!dir.mkpath(dirPath)) {
            qWarning() << "创建目录失败:" << dirPath;
            return;
        }
    }

    QString fileName = QString("Yeild-%1.txt").arg(QDate::currentDate().toString("yyyyMMdd"));
    QString filePath = dirPath + "/" + fileName;

    QSettings settings(filePath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    QStringList headers = {"MaterialTotal","PhotoTotal","ExtraLong","100","Long50","PenBarrel","50","SuperShort","200","Moon","30","Damaged","10-20","Gourd"};

    //QStringList headers = {"MaterialTotal","PhotoTotal","ExtraLong","Long50","100","50","SuperShort","30","200","10-20","Gourd","PenBarrel","Moon","Damaged"};
    //QStringList headers = {"MaterialTotal","PhotoTotal","10-20","30","50","100","200","Long50","SuperShort","ExtraLong","Gourd","PenBarrel","Moon","Damaged"};

    int totalRows = middleTab->rowCount();
    int totalCols = middleTab->columnCount();

    for (int ch = 0; ch < 3; ++ch) {
        int baseRow = ch * 2;       // 数量行
        int ratioRow = baseRow + 1; // 占比行

        if (baseRow >= totalRows || ratioRow >= totalRows) continue;

        QString section = QString("Channel%1").arg(ch + 1); // Section 用英文
        settings.beginGroup(section);

        for (int col = 0; col < headers.size(); ++col) {
            int tableCol = col + 2; // 假设表格前两列是通道和类别
            if (tableCol >= totalCols) continue;

            QTableWidgetItem *countItem = middleTab->item(baseRow, tableCol);
            QString countText = countItem ? countItem->text() : "0";
            if (countText == "–") countText = ""; // “–”改为空

            QTableWidgetItem *ratioItem = middleTab->item(ratioRow, tableCol);
            QString ratioText = ratioItem ? ratioItem->text() : "0%";
            if (ratioText == "–") ratioText = ""; // “–”改为空

            if (headers[col] == "MaterialTotal" || headers[col] == "PhotoTotal") {
                settings.setValue(headers[col], countText);
            } else {
                settings.setValue(headers[col] + "_Count", countText);
                settings.setValue(headers[col] + "_Proportion", ratioText);
            }
        }

        settings.endGroup();
    }

    settings.sync();
    qDebug() << "已保存 QSettings ini 文件:" << filePath;
}





void MainWindow::loadCSVToData() {
    // 获取当前日期
    QString currentDate = QDate::currentDate().toString("yyyy-MM-dd");
    // 构造CSV文件路径
    QString directory = "C:/vpconfig/data";
    QString filePath = directory + "/data.csv";

    QDir dir(directory);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    if (!QFile::exists(filePath)) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.close();
        } else {

        }
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        //QMessageBox::warning(nullptr, "文件打开失败", "无法打开文件 " + filePath);
        LOG_INFO(QString("文件：&1打开失败").arg(filePath));
        return;
    }
    // 使用 QTextStream 逐行读取文件
    QTextStream in(&file);
    // 数据行数组，按顺序填充
    QVector<QStringList*> dataArrays = {&data1[0],&data1[1], &data2[0], &data2[1],&data3[0],&data3[1],};

    // 读取每一行并填充数据
    int lineIndex = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");

        // 跳过空行
        if (fields.size() < 14) continue;

        // 填充数据：每一通道的数据根据行号逐行填充
        if (lineIndex !=0) {
            int channelIndex = lineIndex - 1;  // 计算通道索引

            QStringList* currentData = dataArrays[channelIndex]; // 获取对应通道的数据

            for (int i = 0; i < 13; ++i) {
                (*currentData)[i] = fields[i + 2].remove('%');
            }
        }
        lineIndex++;
    }
    // 关闭文件
    file.close();

    //日志
    writeLogToFile(data1[0].join(","));
    writeLogToFile(data1[1].join(","));
    writeLogToFile(data2[0].join(","));
    writeLogToFile(data2[1].join(","));
    writeLogToFile(data3[0].join(","));
    writeLogToFile(data3[1].join(","));

    materials[0]=data1[0][0].toInt();
    materials[1]=data2[0][0].toInt();
    materials[2]=data3[0][0].toInt();

    camcons[0]=data1[0][1].toInt();
    camcons[1]=data2[0][1].toInt();
    camcons[2]=data3[0][1].toInt();

}

void MainWindow::onClearMaterials() {
    if (!clearMaterials) return;

    if (clearMaterials->isChecked()) {
        if (!clearStartTime.isNull()) {
            qint64 minutesPassed = clearStartTime.secsTo(QDateTime::currentDateTime()) / 60;
            if (minutesPassed >= 15) {
                vfdWindow->clearMaterials();
                QMessageBox::information(this, "提示", "15分钟无入料，停止上料电机");
            }
        }
    }
}


void MainWindow::writeLogToFile(const QString &logText) {

    QString exePath = QCoreApplication::applicationDirPath();

    QString filePath = exePath + "/log02.txt";

    QFile file(filePath);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);

        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        out << "[" << time << "] " << logText << "\n";

        file.close();
    }
}


void MainWindow::writeLog(const QString &logText,int cam) {
    QString currentDate = QDate::currentDate().toString("yyyyMMdd");
    QString dirPath = QString("D:/Data/Log/%1").arg(currentDate);
    QDir dir(dirPath);

    if (!dir.exists()) {
        if (!QDir().mkpath(dirPath)) {
            return;
        }
    }

    QString filePath = dir.filePath(QString("log-%1.log").arg(currentDate));
    if (cam==0)
    {
        filePath = dir.filePath(QString("log-%1.log").arg(currentDate));
    }else
    {
        filePath = dir.filePath(QString("log%1-%2.log").arg(cam).arg(currentDate));
    }

    QFile file(filePath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    out << "[" << time << "] " << logText << endl;
    file.close();
}




void MainWindow::saveAppConfig()
{
    if (!QFile::exists(pathFile)) {
        QMessageBox::warning(nullptr, "配置文件不存在",
                             QString("配置文件 \"%1\" 不存在，请检查文件路径或先保存配置。").arg(pathFile));
        return;
    }

    QSettings settings(pathFile, QSettings::IniFormat);

    settings.beginGroup("config");
    settings.setValue("passage",passage);
    settings.endGroup();

}


void MainWindow::loadAppConfig()
{
    if (!QFile::exists(pathFile)) {
        QMessageBox::warning(nullptr, "配置文件不存在",
                             QString("配置文件 \"%1\" 不存在，请检查文件路径或先保存配置。").arg(pathFile));
        return;
    }

    QSettings settings(pathFile, QSettings::IniFormat);

    settings.beginGroup("config");
    passage = settings.value("passage", 1).toInt();

    settings.endGroup();

}



QStringList MainWindow::Get_Trig_Blow() {
    QStringList trigList;


    unsigned long num = 0;
    for (int i = 1; i < 14; ++i) {
        short ret = MCF_Sorting_Get_Trig_Blow_Count_Net(i, &num, 0);
        if (ret == 0) {
            QString trig = QString("气阀%1触发%2次").arg(i).arg(num);
            trigList << trig;
        } else {
            trigList << QString("气阀%1读取失败，错误码：%2").arg(i).arg(ret);
        }
    }
    /*writeLogToFile(trigList.join("\n"));
    LOG_INFO(trigList.join("\n"));*/
    writeLog(trigList.join("\n"));
    return trigList;
}


QStringList MainWindow::Get_Trig_Cam() {
    QStringList trigList;


    unsigned long num = 0;
    for (int i = 1; i < 4; ++i) {
        short ret = MCF_Sorting_Get_Trig_Camera_Count_Net(i, &num, 0);
        if (ret == 0) {
            QString trig = QString("相机%1触发%2次").arg(i).arg(num);
            trigList << trig;
        } else {
            trigList << QString("相机%1读取失败，错误码：%2").arg(i).arg(ret);
        }
    }
    /*writeLogToFile(trigList.join("\n"));
    LOG_INFO(trigList.join("\n"));*/
    writeLog(trigList.join("\n"));
    return trigList;
}


void MainWindow::onCamconChanged(int index, int value)
{
    materials[index]++;
    writeLog(QString("板卡%1入料数＋1，当前入料数%2").arg(index).arg(value));
}

void MainWindow::onMaterialChanged(int index, int value)
{
    //camcons[index]++;
    //writeLog(QString("板卡%1相机计数＋1，当前相机计数%2").arg(index).arg(value));

    //入料灯光
    if (myBoardCard->DI0Status[0]==0) {
        //int num=data1[0][0].toInt();
        light4->setStyleSheet("background-color: green; border-radius: 25px;");
        //data1[0][0]=QString::number(num+1);
        //updataTab1();
        if (clearMaterials) clearStartTime = QDateTime::currentDateTime();
    }else if (myBoardCard->DI0Status[0]==1) {
        light4->setStyleSheet("background-color: red; border-radius: 25px;");
    }

    if (myBoardCard->DI0Status[1]==0) {
        //int num=data1[1][0].toInt();
        light5->setStyleSheet("background-color: green; border-radius: 25px;");
        //data1[1][0]=QString::number(num+1);
        //updataTab1();
        if (clearMaterials) clearStartTime = QDateTime::currentDateTime();

    }else if (myBoardCard->DI0Status[1]==1) {
        light5->setStyleSheet("background-color: red; border-radius: 25px;");
    }

    if (myBoardCard->DI0Status[2]==0) {
        //int num=data1[2][0].toInt();
        light6->setStyleSheet("background-color: green; border-radius: 25px;");
        //data1[2][0]=QString::number(num+1);
        //updataTab1();
        if (clearMaterials) clearStartTime = QDateTime::currentDateTime();
    }else if (myBoardCard->DI0Status[2]==1) {
        light6->setStyleSheet("background-color: red; border-radius: 25px;");
    }

}

void MainWindow::onCamRatioChanged(int index,double ratio)
{
    camRatio[index]=ratio;
}

void MainWindow::onPlcStartMotor(int index)
{
    if (start==0) buttonStatus->click();
}

void MainWindow::onPlcStopMotor(int index)
{
    if (start==1) buttonStatus->click();
}

void MainWindow::isDataFile()
{
    QString dirPath = "D:/Data/Yeild";
    QString fileName = QString("Yeild-%1.txt").arg(QDate::currentDate().toString("yyyyMMdd"));
    QString filePath = QDir(dirPath).filePath(fileName);

    // 确保目录存在
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    if (!QFile::exists(filePath)) {

    } else {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "加载数据", "是否加载今天的数据？",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            loadIniToTable(middleTab);
            loadTableToData(middleTab);
        } else {

        }
    }
}




void MainWindow::issueInstructions(int outtest,double camRatio,QString boardsNumber,int camNumber,int camCount,QString timestamp,QVector<VpAiInferResult> vpresult)
{
        writeLog(QString("issueInstructions---相机%1处理完成次数:%2").arg(camNumber).arg(camCount),camNumber);

        QString labelsStr;
        for (const auto &res : vpresult) {
           labelsStr += QString::number(res.label) + " ";
         }
      writeLog(labelsStr,camNumber);
    if (vpresult.last().label==9)
    {
        writeLog(QString("issueInstructions---相机%1无产品不吹气--板卡号%2，相机计数%3，时间戳%4").arg(camNumber).arg(boardsNumber).arg(camCount).arg(timestamp),camNumber);
        //LOG_INFO(QString("issueInstructions---相机%1无产品不吹气--板卡号%2，相机计数%3，时间戳%4").arg(camNumber).arg(boardsNumber).arg(camCount).arg(timestamp));
        //visionLoop2(boardsNumber.toShort(),camCount,13,timestamp);
        return;
    }
        //汇总
        AiResult aiResult= inferenceResult(vpresult,camRatio,camNumber);
        LOG_INFO(QString("type1:%1,type2:%2,type3:%3").arg(aiResult.type1).arg(aiResult.type2).arg(aiResult.type3));
         if (aiResult.type2==9)
             {
                   writeLog(QString("issueInstructions---相机%1无产品不吹气--板卡号%2，相机计数%3，时间戳%4").arg(camNumber).arg(boardsNumber).arg(camCount).arg(timestamp),camNumber);
                   //LOG_INFO(QString("issueInstructions---相机%1无产品不吹气--板卡号%2，相机计数%3，时间戳%4").arg(camNumber).arg(boardsNumber).arg(camCount).arg(timestamp));
                    //visionLoop2(boardsNumber.toShort(),camCount,13,timestamp);
                   return;
             }

                writeLog("issueInstructions---进入推理",camNumber);
                if (outtest==-1)
                {
                    writeLog(QString("issueInstructions---outtest=%1").arg(outtest),camNumber);
                    if (!vpresult.isEmpty()) {
                        writeLog(QString("issueInstructions---results.size() > 0"),camNumber);

                        int ret = classify(aiResult.width, aiResult.height,aiResult.type1,aiResult.type2,aiResult.type3);
                        //LOG_INFO(QString("相机1--类别%1、宽%2、长%3；像素宽%4、长%5").arg(category).arg(width).arg(height).arg(result.rotatedRect.size.width).arg(result.rotatedRect.size.height));
                        writeLog(QString("issueInstructions---相机%1--类别%2、宽%3mm、长%4mm；像素宽%5、长%6").arg(camNumber).arg(aiResult.type2).arg(aiResult.width*10).arg(aiResult.height*10).arg(aiResult.width/camRatio*10).arg(aiResult.height/camRatio*10),camNumber);
                        writeLog(QString("issueInstructions---相机%1下发吹气--板卡号%2，相机计数%3，气阀号%4，时间戳%5").arg(camNumber).arg(boardsNumber).arg(camCount).arg(ret).arg(timestamp),camNumber);
                        if (ret==14)
                        {
                            writeLog(QString("issueInstructions---相机%1ng14--板卡号%2，相机计数%3，时间戳%4").arg(camNumber).arg(boardsNumber).arg(camCount).arg(timestamp),camNumber);
                        }else
                        {
                            visionLoop2(boardsNumber.toShort(),camCount,ret,timestamp,camNumber);
                        }

                        updataTabValue(boardsNumber.toInt(),ret,1);
                    }else
                    {
                        writeLog(QString("issueInstructions---results.size() <= 0"),camNumber);
                        visionLoop2(boardsNumber.toShort(),camCount,11,timestamp,camNumber);
                        //LOG_INFO(QString("CH01下发吹气--站号%1，相机计算%2，气阀号%3").arg(myBoardCard->boards_number[0]).arg(myBoardCard->camcons[0]).arg(11));
                        writeLog(QString("issueInstructions---相机%1下发NG吹气--站号%2，相机计算%3，气阀号%4，数据戳%5").arg(camNumber).arg(boardsNumber).arg(boardsNumber).arg(11).arg(timestamp),camNumber);
                        writeLog(QString("issueInstructions---相机%1-NG原因-推理失败").arg(camNumber),camNumber);
                        updataTabValue(boardsNumber.toInt(),outtest,1);
                    }
                }
                else
                {
                    writeLog(QString("issueInstructions---指定吹气，气阀号%1").arg(outtest),camNumber);
                    visionLoop2(boardsNumber.toShort(),camCount,outtest,timestamp,camNumber);
                    //LOG_INFO(QString("CH01下发吹气--站号%1，相机计算%2，气阀号%3").arg(myBoardCard->boards_number[0]).arg(myBoardCard->camcons[0]).arg(outtest));
                    writeLog(QString("issueInstructions---相机%1下发吹气--站号%2，相机计算%3，气阀号%4").arg(camNumber).arg(boardsNumber).arg(camCount).arg(outtest),camNumber);
                    updataTabValue(boardsNumber.toInt(),outtest,1);
                }
}



/*AiResult MainWindow::inferenceResult(QVector<VpAiInferResult> vpresult,double camRatio)
{
    double maxWidth = 0.0;
    double maxHeight = 0.0;
    int maxLabel = std::numeric_limits<int>::min(); // 最大 label
   if (!vpresult.isEmpty())
   {
       if (vpresult.last().label==9 ) return AiResult(9,0,0);

       for (const auto &item : vpresult) {
           // 更新最大 label
           if (item.label > maxLabel && item.label!=9) {
               maxLabel = item.label;
           }

           // 计算缩放后的宽高
           double widthT = item.rotatedRect.size.width * camRatio;
           double heightT = item.rotatedRect.size.height * camRatio;
           double width = std::min(widthT, heightT);
           double height = std::max(widthT, heightT);

           if (width > maxWidth) maxWidth = width;
           if (height > maxHeight) maxHeight = height;
       }
       return AiResult(maxLabel,maxWidth,maxHeight);
   }else
   {
       return AiResult(9,0,0);
   }
}*/


/*AiResult MainWindow::inferenceResult(QVector<VpAiInferResult> vpresult, double camRatio, int cam)
{
    double maxWidth = 0.0;
    double maxHeight = 0.0;
    QStringList whList;

    // 定义优先级，值越小优先级越高
    auto getPriority = [](int label) -> int {
        switch (label) {
        case 5: return 0;
        case 4: return 1;
        case 2:
        case 3: return 2;
        case 6: return 3;
        case 1: return 4;
        case 0: return 5;
        default: return 999; // 不参与比较 (如 9)
        }
    };

    if (!vpresult.isEmpty()) {

        // 统计 label=3 和 label=6 的数量
        int label3Count = std::count_if(vpresult.begin(), vpresult.end(),
                                        [](const VpAiInferResult &item){ return item.label == 3; });

        int label6Count = std::count_if(vpresult.begin(), vpresult.end(),
                                        [](const VpAiInferResult &item){ return item.label == 6; });

        int bestLabel = -1;   // 最优 label
        int bestRank = 999;

        int type2Label = -1;  // 专门记录 label=0 或 1 的最佳
        int type2Rank = 999;

        QVector<double> validHeights; // 用于计算平均高度

        for (auto &item : vpresult) {

            // 宽或高为 0，则强制改成 9
            if (item.rotatedRect.size.width == 0 || item.rotatedRect.size.height == 0) {
                item.label = 9;
            }

            // 如果 label=3，但数量不足 3 个，则改成 9
            if (item.label == 3 && label3Count < 3) {
                item.label = 9;
            }

            // 如果 label=6，但数量不足 3 个，则改成 9
            if (item.label == 6 && label6Count < 2) {
                item.label = 9;
            }

            int rank = getPriority(item.label);

            // 更新总体最优
            if (rank < bestRank) {
                bestRank = rank;
                bestLabel = item.label;
            }

            // 单独更新 label=0 或 1 的最优
            if ((item.label == 0 || item.label == 1) && rank < type2Rank) {
                type2Rank = rank;
                type2Label = item.label;
            }

            // 计算缩放后的宽高
            double widthT = item.rotatedRect.size.width * camRatio;
            double heightT = item.rotatedRect.size.height * camRatio;
            double width = std::min(widthT, heightT);
            double height = std::max(widthT, heightT);

            if (width > maxWidth) maxWidth = width;
            if (height > maxHeight) maxHeight = height;

            // 保存非零高度
            if (height > 0.0) {
                validHeights.append(height);
            }

            whList << QString("label=%1, w=%2, h=%3, wpx=%4, hpx=%5")
                          .arg(item.label)
                          .arg(width / 10, 0, 'f', 2)
                          .arg(height / 10, 0, 'f', 2)
                          .arg(item.rotatedRect.size.width)
                          .arg(item.rotatedRect.size.height);
        }

        // 打印日志
        for (const QString &info : whList) {
            writeLog(info, cam);
        }

        // 如果全为 9，则返回 9
        if (std::all_of(vpresult.begin(), vpresult.end(),
                        [](const VpAiInferResult &item){ return item.label == 9; })) {
            return AiResult(9, -1, 0, 0);
        }

        // 没有任何有效 label
        if (bestLabel == -1)
            return AiResult(9, -1, 0, 0);

        // 如果 type2Label == 1，则取所有非零 height 的平均值
        if (type2Label == 1 && !validHeights.isEmpty()) {
            double sum = std::accumulate(validHeights.begin(), validHeights.end(), 0.0);
            maxHeight = sum / validHeights.size();
        }

        return AiResult(bestLabel, type2Label, maxWidth, maxHeight);
    }
    else {
        return AiResult(9, -1, 0, 0);
    }
}*/



/*
AiResult MainWindow::inferenceResult(QVector<VpAiInferResult> vpresult, double camRatio, int cam)
{
    QStringList whList;

    auto getPriority = [](int label) -> int {
        switch (label) {
        case 5: return 0;
        case 4: return 1;
        case 2:
        case 3: return 2;
        case 6: return 3;
        case 1: return 4;
        case 0: return 5;
        default: return 999;
        }
    };

    if (vpresult.isEmpty())
        return AiResult(9, -1, 0, 0);

    int label3Count = std::count_if(vpresult.begin(), vpresult.end(),
                                    [](const VpAiInferResult &item){ return item.label == 3; });
    int label6Count = std::count_if(vpresult.begin(), vpresult.end(),
                                    [](const VpAiInferResult &item){ return item.label == 6; });

    int bestLabel = -1;
    int bestRank = 999;

    int type2Label = -1;
    int type2Rank = 999;

    QVector<double> validWidths;
    QVector<double> validHeights;

    for (auto &item : vpresult) {

        if (item.rotatedRect.size.width == 0 || item.rotatedRect.size.height == 0)
            item.label = 9;

        if (item.label == 3 && label3Count < 1)
            item.label = 9;

        if (item.label == 6 && label6Count < 3)
            item.label = 9;

        int rank = getPriority(item.label);

        if (rank < bestRank) {
            bestRank = rank;
            bestLabel = item.label;
        }

        if ((item.label == 0 || item.label == 1) && rank < type2Rank) {
            type2Rank = rank;
            type2Label = item.label;
        }

        // 按旋转矩形短边/长边计算宽高
        double w = std::min(item.rotatedRect.size.width, item.rotatedRect.size.height) * camRatio;
        double h = std::max(item.rotatedRect.size.width, item.rotatedRect.size.height) * camRatio;

        // 只取非零值
        if (w > 0.0) validWidths.append(w);
        if (h > 0.0) validHeights.append(h);

        whList << QString("label=%1, w=%2, h=%3, wpx=%4, hpx=%5")
                      .arg(item.label)
                      .arg(w / 10, 0, 'f', 2)
                      .arg(h / 10, 0, 'f', 2)
                      .arg(item.rotatedRect.size.width)
                      .arg(item.rotatedRect.size.height);
    }

    for (const QString &info : whList)
        writeLog(info, cam);

    if (std::all_of(vpresult.begin(), vpresult.end(),
                    [](const VpAiInferResult &item){ return item.label == 9; }) || bestLabel == -1)
        return AiResult(9, -1, 0, 0);

    // 计算非零平均
    double avgWidth = validWidths.isEmpty() ? 0.0 :
                      std::accumulate(validWidths.begin(), validWidths.end(), 0.0) / validWidths.size();
    double avgHeight = validHeights.isEmpty() ? 0.0 :
                       std::accumulate(validHeights.begin(), validHeights.end(), 0.0) / validHeights.size();

    writeLog(QString("平均宽%1，高%2").arg(avgWidth).arg(avgHeight));
    return AiResult(bestLabel, type2Label, avgWidth, avgHeight);
}
*/



/*AiResult MainWindow::inferenceResult(QVector<VpAiInferResult> vpresult, double camRatio, int cam)
{
    QStringList whList;

    // 定义优先级（值越小优先级越高）
    auto getPriority = [](int label) -> int {
        switch (label) {
        case 2:
        case 3: return 0;
        case 5: return 1;
        case 4: return 2;
        case 6: return 3;
        case 1: return 4;
        case 0: return 5;
        default: return 999;
        }
    };

    if (vpresult.isEmpty())
        return AiResult(9, -1, 0, 0);

    // 统计特定标签数量
    int label3Count = std::count_if(vpresult.begin(), vpresult.end(),
                                    [](const VpAiInferResult &item){ return item.label == 2; });
    int label6Count = std::count_if(vpresult.begin(), vpresult.end(),
                                    [](const VpAiInferResult &item){ return item.label == 6; });
    int label0Count = std::count_if(vpresult.begin(), vpresult.end(),
                                    [](const VpAiInferResult &item){ return item.label == 0; });
    int label1Count = std::count_if(vpresult.begin(), vpresult.end(),
                                    [](const VpAiInferResult &item){ return item.label == 1; });

    int bestLabel = -1;
    int bestRank = 999;

    QVector<double> validWidths;
    QVector<double> validHeights;

    for (auto &item : vpresult) {

        // 宽或高为 0 → 标记为无效
        if (item.rotatedRect.size.width == 0 || item.rotatedRect.size.height == 0)
            item.label = 9;

        // 数量不足的过滤规则
        if (item.label == 2 && label3Count < 2)
            item.label = 9;
        if (item.label == 6 && label6Count < 3)
            item.label = 9;




        // 旋转矩形宽高（短边为宽，长边为高）
        double w = std::min(item.rotatedRect.size.width, item.rotatedRect.size.height) * camRatio;
        double h = std::max(item.rotatedRect.size.width, item.rotatedRect.size.height) * camRatio;

        if (w > 0.0) validWidths.append(w);
        if (h > 0.0) validHeights.append(h);


        if (item.label == 2 && w!=0 && h/w>2.45 )
           item.label = 9;

        int rank = getPriority(item.label);

        // 更新最优 label

        if (rank < bestRank) {
            bestRank = rank;
            bestLabel = item.label;
        }

        whList << QString("label=%1, w=%2, h=%3, wpx=%4, hpx=%5")
                      .arg(item.label)
                      .arg(w / 10, 0, 'f', 2)
                      .arg(h / 10, 0, 'f', 2)
                      .arg(item.rotatedRect.size.width)
                      .arg(item.rotatedRect.size.height);
    }

    // 打印尺寸日志
    for (const QString &info : whList)
        writeLog(info, cam);

    // 全部无效则返回 9
    if (std::all_of(vpresult.begin(), vpresult.end(),
                    [](const VpAiInferResult &item){ return item.label == 9; }) || bestLabel == -1)
        return AiResult(9, -1, 0, 0);

    // 非零平均宽高
    double avgWidth = validWidths.isEmpty() ? 0.0 :
                      std::accumulate(validWidths.begin(), validWidths.end(), 0.0) / validWidths.size();
    double avgHeight = validHeights.isEmpty() ? 0.0 :
                       std::accumulate(validHeights.begin(), validHeights.end(), 0.0) / validHeights.size();

    //  type2Label 判断逻辑：
    //   0多选0；
    //   1多或相等或都没有 → 选1；
    int type2Label = 1;
    if (label0Count > label1Count)
        type2Label = 0;

    writeLog(QString("平均宽=%1, 高=%2").arg(avgWidth).arg(avgHeight));

    return AiResult(bestLabel, type2Label, avgWidth, avgHeight);
}*/



AiResult MainWindow::inferenceResult(QVector<VpAiInferResult> vpresult, double camRatio, int cam)
{
    QStringList whList;

    // 优先级
    auto getPriority = [](int label) -> int {
        switch (label) {
        case 3: return 0;
        case 2: return 1;
        case 5: return 2;
        case 4: return 3;
        case 6: return 4;
        case 1: return 5;
        case 0: return 6;
        default: return 999;
        }
    };

    if (vpresult.isEmpty() || vpresult.last().label == 9)
        return AiResult(1, 9, -1, 0, 0);

    QVector<double> validWidths;
    QVector<double> validHeights;

    int label2Count = 0;
    int label6Count = 0;
    int label0Count = 0;
    int label1Count = 0;

    // 有效数量统计
    for (auto &item : vpresult)
    {
        double w = std::min(item.rotatedRect.size.width, item.rotatedRect.size.height);
        double h = std::max(item.rotatedRect.size.width, item.rotatedRect.size.height);

        if (item.rotatedRect.size.width == 0 || item.rotatedRect.size.height == 0)
        {
            item.label = 9;
            continue;
        }

        if (item.label == 2 && w != 0 && h / w > 2.45)
        {
            item.label = 9;
            continue;
        }

        if (item.label == 2) ++label2Count;
        else if (item.label == 6) ++label6Count;
        else if (item.label == 0) ++label0Count;
        else if (item.label == 1) ++label1Count;
    }

    // 处理
    struct LabelRank {
        int label;
        int rank;
    };
    QVector<LabelRank> rankedLabels;

    for (auto &item : vpresult)
    {
        double w = std::min(item.rotatedRect.size.width, item.rotatedRect.size.height) * camRatio;
        double h = std::max(item.rotatedRect.size.width, item.rotatedRect.size.height) * camRatio;

        if (item.rotatedRect.size.width == 0 || item.rotatedRect.size.height == 0)
            item.label = 9;

        // label 过滤
        if (item.label == 6 && label6Count < 3)
            item.label = 9;

        if (w > 0.0) validWidths.append(w);
        if (h > 0.0) validHeights.append(h);

        int rank = getPriority(item.label);
        if (item.label != 9)
            rankedLabels.append({item.label, rank});

        whList << QString("label=%1, w=%2, h=%3, wpx=%4, hpx=%5")
                      .arg(item.label)
                      .arg(w / 10, 0, 'f', 2)
                      .arg(h / 10, 0, 'f', 2)
                      .arg(item.rotatedRect.size.width)
                      .arg(item.rotatedRect.size.height);
    }

    for (const QString &info : whList)
        writeLog(info, cam);


    int invalidCount = std::count_if(vpresult.begin(), vpresult.end(),
                                     [](const VpAiInferResult &item){ return item.label == 9; });

    if (rankedLabels.isEmpty() || invalidCount >= 7)
        return AiResult(1, 9, -1, 0, 0);

    // 次优先级
    std::sort(rankedLabels.begin(), rankedLabels.end(), [](const LabelRank &a, const LabelRank &b){
        return a.rank < b.rank;
    });

    int type2 = rankedLabels[0].label;
    int type3 = -1;

    // 选取 type3，确保不与 type2 重复
    for (int i = 1; i < rankedLabels.size(); ++i)
    {
        if (rankedLabels[i].label != type2)
        {
            type3 = rankedLabels[i].label;
            break;
        }
    }

    //宽平均 ----------
    double avgWidth = validWidths.isEmpty() ? 0.0 :
                      std::accumulate(validWidths.begin(), validWidths.end(), 0.0) / validWidths.size();
    double avgHeight = validHeights.isEmpty() ? 0.0 :
                       std::accumulate(validHeights.begin(), validHeights.end(), 0.0) / validHeights.size();


    //type1
    int type1 = (label0Count > label1Count) ? 0 : 1;

    bool allLabel4 = std::all_of(vpresult.begin(), vpresult.end(), [](const VpAiInferResult &item){
    return item.label == 4 || item.label == 9;
    });

    if (allLabel4)
        type1 = 0;

    writeLog(QString("平均宽=%1, 高=%2, type1=%3, type2=%4, type3=%5, 无效数=%6")
             .arg(avgWidth).arg(avgHeight).arg(type1).arg(type2).arg(type3).arg(invalidCount));

    return AiResult(type1, type2, type3, avgWidth, avgHeight);
}



