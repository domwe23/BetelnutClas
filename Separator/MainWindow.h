//
// Created by simon on 2024-11-19.
//

#ifndef FQI_MAINWINDOW_H
#define FQI_MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QHBoxLayout>
#include <QSet>
#include <QPixmap>
#include <QSqlDatabase>
#include <QFileInfo>
#include <QDateTime>
#include "VpUserManager.h"
#include "VpMainWindow.h"
#include <QWidget>
#include <QListWidget>
#include <QFormLayout>
#include "FrmCameras.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QVector>
#include <QTimer>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>

#include "boardcard.h"
//#include "MCDLL_NET.h"
#include "vfdwindow.h"

#include "ComWindow.h"
#include "BlSizeWindow.h"
#include "meswindow.h"
#include "ImageWidget.h"
#include <queue>
#include "MCDLL_NET_SORTING.h"
#include "calibrationWindow.h"
#include "VpLog.h"
#include <QStringList>
#include <QString>
#include <QRandomGenerator>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QDir>
#include <QTableWidget>
#include <QMessageBox>
#include <QLineEdit>
#include <QCheckBox>
#include "CDetectResult.h"
#include <QElapsedTimer>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class VpUser;

struct AiResult
{
    int type1;
    int type2;
    int type3;
    double width;
    double height;

    AiResult() : type1(0),type2(0),type3(-1), width(0.0), height(0.0) {}
    AiResult(int t1,int t2,int t3, double w, double h) : type1(t1),type2(t2),type3(t3), width(w), height(h) {}
};

class MainWindow : public VpMainWindow {
Q_OBJECT


    struct cameraResult {
    short stationNumber;
    int photoCount;
    int photoResult;

    cameraResult(short station = 0, int count = 0, int result = 0)
        : stationNumber(station), photoCount(count), photoResult(result) {}
};


public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void initCameraState();

    void test();

    void init();

    void checkCameraQueue();

    ComWindow *comWindow;
    VFDWindow *vfdWindow;
    MESWindow *mesWindow;
    BoardCard* myBoardCard;
    CalibrationWindow* calibrationWindow;
    BlSizeWindow* blsizeWindow;

public slots:
    void ShowVpEdit();

    //void On1sTimeout();

    //void btnStartClicked();

    void onActionSwitchProjClicked();

    void onUserLogin(VpUser user);

    //void onBtnExportReport();

   //void onBtnClearData();

    void updateEncoderInfo();

    void startFunction();

    void stopFunction();

    int getFakeCameraResult(int index);

    int classify(double& diameter, double& length,int category,int category2,int category3);

    void onCamconChanged(int index, int value);

    void onMaterialChanged(int index, int value);

    void isDataFile();

    void onCamRatioChanged(int index,double ratio);

    void onPlcStartMotor(int index);

    void onPlcStopMotor(int index);


private:
    int outtest=-1;
    int start=0;
    int out=2;
    //double camRatio=0.005;
    double diff=0.0;
    const int StationMax = 3;
    const int Depth = 512;
    std::queue<cameraResult> resultQueue;
    std::mutex queueMutex;
    QVector<int> camerConnect={0,0,0};
    QVector<int> cams={0,0,0};   //相机计数
    QString pathFile="C:/vpconfig/Config.ini";

    QVector<int> materialCount1={0,0,0,0,0,0,0,0,0,0,0,0,0};
    QVector<int> materialCount2={0,0,0,0,0,0,0,0,0,0,0,0,0};
    QVector<int> materialCount3={0,0,0,0,0,0,0,0,0,0,0,0,0};

    QVector<int> camcons={0,0,0};   //相机计数--data
    QVector<int> camnum={0,0,0};   //相机计数--吹气
    QVector<int> materials={0,0,0}; //入料数
    QVector<double> camRatio={0,0,0};

    QVector<int>  boards_number= {0, 0, 0};
    QVector<int> material_number={0,0,0};
    QVector<int> photograph_number={0,0,0};
    QVector<int> Comprehensive_1_Count_Dly={0,0,0};
    static uint32_t Visual_Camera_Tri_Count_Result[3][512];

    QThread* visionThread = nullptr;

    bool threadRunning = false;


    void startVisionThread(); // 启动线程
    void stopVisionThread();  // 停止线程
    void visionLoop();//吹气
    void visionLoop(short stationNumber, int photoCount, int photoResult);
    void visionLoop2(short stationNumber, int photoCount, int photoResult, QString timestamp,int cam);
    QStringList Get_Trig_Blow();

    QStringList Get_Trig_Cam();

    void closeEvent(QCloseEvent *event) override;

    void onTaskFinished(QString task, int elapsed, QVariantMap output);

    QGroupBox* createStatusGroup(const QString& title, int lightCount,int index);

    QTableWidget* createDetectionDataTable();

    void initWindow(int nums);

    void createImgWindow(int index);

    void deleteImgWindow(int index);

    void relayoutAllImgWindows();

    void updateLight();
    void updatedata();
    void updataTab1(bool isStart=false);
    void updataTab2(bool isStart=false);
    void updataTabValue(int station,int index,int value);
    void updataTabProportion();
    void savepData();
    void loadpData();
    void saveToCSV(QTableWidget *table);    //save csv
    void saveDataToIni2(QTableWidget *middleTab);   //save txt
    void loadCSVToTable(QTableWidget *middleTab);//read csv
    void loadIniToTable(QTableWidget *middleTab); //read txt
    void loadTableToData(QTableWidget *middleTab);
    void loadCSVToData();
    void writeLogToFile(const QString &logText);
    void writeLog(const QString &logText,int cam=0);
    void onClearMaterials();
    void saveAppConfig();
    void loadAppConfig();
    void issueInstructions(int outtest,double camRatio,QString boardsNumber,int camNumber,int camCount,QString timestamp,QVector<VpAiInferResult> vpresult);
    AiResult inferenceResult(QVector<VpAiInferResult> vpresult,double camRatio,int cam);

    QSqlDatabase db;
private:


    VpUser m_User = VpUser::Visitor();
    Ui::MainWindow *ui;
    QTimer *m_1STimer = nullptr;
    QLabel *m_LabVer = nullptr;
    QLabel *m_PlcState = nullptr;
    QLabel *m_PlcInfo = nullptr;
    QLabel *m_CurrentDate = nullptr;
    QLabel *m_CurrentUser = nullptr;
    bool m_IsRun = false;
    QHBoxLayout *m_CameraLayout;
    QWidget *m_CameraWidget;
    QSet<QString> m_Cameras;

     // 界面
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *contentLayout;

    // 标题栏
    QLabel *titleLabel;
    QHBoxLayout *topLayout;

    //底部
    QHBoxLayout *downLayout;

    // 左导航栏
    QListWidget *navList;

    QWidget *leftPanel;
    QVBoxLayout *leftLayout;

    QLabel *m_LabVer1;
    QLabel *m_LabTime;
    QTimer *m_TimeTimer;




    // 中间
    int passage=1;
    QWidget *middleWidget;
    QHBoxLayout *middleLayout;//通道
    QLabel *passage1;
    QLabel *passage2;
    QLabel *passage3;

    //表格
    QTableWidget *middleTab;

    // 右状态栏
    QPushButton* buttonStatus;
    QWidget *rightWidget;
    QVBoxLayout *rightLayout;

    QLabel* labelEncoderPulse;
    QLabel* labelEncoderPulse2;
    QLabel* labelEncoderPulse3;
    QTimer* timer;

    QCheckBox* clearMaterials;
    QDateTime clearStartTime;

    QLabel* channelSpeed;
    QLabel* channelSpeed2;
    QLabel* channelSpeed3;

    QLabel* light1;
    QLabel* light2;
    QLabel* light3;
    QLabel* light4;
    QLabel* light5;
    QLabel* light6;
    QLabel* light7;
    QLabel* light8;
    QLabel* light9;



    QPixmap success = QPixmap(":/Assets/circle_green.svg");
    QPixmap failed = QPixmap(":/Assets/circle_red.svg");
    QPixmap none = QPixmap(":/Assets/circle_white.svg");

    QVector<QStringList> data1 = {    //拍照总数    //10-20                                          //长50                                  //笔杆             //破废
        (QStringList() << "0" << "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0"),
        (QStringList() << "0" << "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0")
    };
    QVector<QStringList> data2 = {
        (QStringList() << "0" << "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0"),
        (QStringList() << "0" << "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0")
    };
    QVector<QStringList> data3 = {
        (QStringList() << "0" << "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0"),
        (QStringList() << "0" << "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0" << "0" << "0" << "0"<< "0" << "0" << "0")
    };

private slots:
    void handleLog(const QString& msg);

};


#endif //FQI_MAINWINDOW_H
