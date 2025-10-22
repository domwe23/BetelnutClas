//
// Createdr on 25-8-1.
////
//
//// You may need to build the project (run Qt uic code generator) to get "ui_CalibrationWindow.h" resolved
//
//#include "calibrationwindow.h"
//#include "ui_CalibrationWindow.h"
//
//
//CalibrationWindow::CalibrationWindow(QWidget *parent) :
//    VpMainWindow(parent), ui(new Ui::CalibrationWindow) {
//    ui->setupUi(this);
//    adjustSize(); by Adminstrator on 25-8-1.
//

// You may need to build the project (run Qt uic code generator) to get "ui_CalibrationWindow.h" resolved

#include "calibrationwindow.h"
#include "ui_CalibrationWindow.h"


CalibrationWindow::CalibrationWindow(QWidget* parent) :
    VpMainWindow(parent), ui(new Ui::CalibrationWindow)
{
    ui->setupUi(this);
    adjustSize();

    this->setFixedSize(930, 670);
    writeLogToFile("设置窗口");
    //气阀触发次数
    this->setWindowTitle("标定设置");

    QTimer* staticEncoders = new QTimer(this);
    connect(staticEncoders, &QTimer::timeout, this, &CalibrationWindow::updaEncoders);
    staticEncoders->start(5);

    QString imgPath = QCoreApplication::applicationDirPath() + "/image/mark.jpg";
    QPixmap pixmap(imgPath);
    ui->label_5->setPixmap(pixmap);
    ui->label_5->setScaledContents(true);  // 图片自动缩放以适应控件大小
    ui->label_5->resize(350,270);

    QDoubleValidator *validator = new QDoubleValidator(0.0, 9999.0, 2, ui->lineEdit_2);
    validator->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEdit_3->setValidator(validator);
    writeLogToFile("限制数字");



    connect(ui->pushButton, &QPushButton::clicked, this, [=]() {
        calibration1=1;
    });
    connect(ui->pushButton_3, &QPushButton::clicked, this, [=]() {
       calibration2=1;
   });
    connect(ui->pushButton_2, &QPushButton::clicked, this, [=]() {

        if (calibration1==1&&calibration2==1&&!ui->lineEdit_3->text().isEmpty()) {
            if (Encoder!=0) {
                 k= Encoder/ui->lineEdit_3->text().toDouble();
                ui->lineEdit_4->setText(QString::number(k));
            }
        }
              writeLog(QString::number(calibration1));
              writeLog(QString::number(calibration2));
              writeLog(ui->lineEdit_3->text());
              writeLog(QString::number(Encoder));
              writeLog(QString::number(k));
   });

    connect(ui->pushButton_4, &QPushButton::clicked, this, [=]() {
        calibration1=0;
        calibration2=0;
        Encoder=0;
        ui->lineEdit_2->setText("");
        ui->lineEdit_3->setText("");
        ui->lineEdit_4->setText("");
   });

    connect(ui->pushButton_5, &QPushButton::clicked, this, &CalibrationWindow::disposition);
    writeLogToFile("绑定按钮");


    connect(ui->lineEdit_4, &QLineEdit::textChanged, this, [this](const QString &text){
          k = text.toDouble();
        disposition2();
      });

    connect(ui->pushButton_6, &QPushButton::clicked, this, [=]() {
    channel=1;
    ui->pushButton_6->setStyleSheet("border: 2px solid #00AAFF; border-radius: 2px;");
    ui->pushButton_7->setStyleSheet("");
    ui->pushButton_8->setStyleSheet("");
        disposition2();
    });
    connect(ui->pushButton_7, &QPushButton::clicked, this, [=]() {
        channel=2;
        ui->pushButton_7->setStyleSheet("border: 2px solid #00AAFF; border-radius: 2px;");
        ui->pushButton_6->setStyleSheet("");
        ui->pushButton_8->setStyleSheet("");
        disposition2();
    });
    connect(ui->pushButton_8, &QPushButton::clicked, this, [=]() {
        channel=3;
        ui->pushButton_8->setStyleSheet("border: 2px solid #00AAFF; border-radius: 2px;");
        ui->pushButton_6->setStyleSheet("");
        ui->pushButton_7->setStyleSheet("");
        disposition2();
    });
    connect(ui->pushButton_9, &QPushButton::clicked, this, [=]() {
        myBoardCard->savepParameter();
        saveConfigCamera();
    });


    InitWindow();
    writeLogToFile("InitWindow()");
    LoadConfig();
    writeLogToFile("LoadConfig()");
    LoadLocationRowToTable();
    writeLogToFile("LoadLocationRowToTable()");
    LoadConfigCamera();
}

CalibrationWindow::~CalibrationWindow() {
    delete ui;
}




void CalibrationWindow::InitWindow() {

    QStandardItemModel *model = new QStandardItemModel(3, 14, this);

    // 设置列标题
    QStringList headers = {"设备", "相机"};
    for (int i = 2; i <= 13; ++i)
        headers << QString("气阀%1").arg(i);
    //headers << "气阀1";
    model->setHorizontalHeaderLabels(headers);

    // 设置行标题（去掉“IO位号”）
    QStringList rowLabels = {"位置", "偏移", "补正"};
    for (int row = 0; row < rowLabels.size(); ++row) {
        model->setItem(row, 0, new QStandardItem(rowLabels[row]));
    }

    // 设置模型
    ui->tableView->setModel(model);

    // 第1行：位置
    for (int col = 1; col < 14; ++col) {
        QLineEdit *editor = new QLineEdit(ui->tableView);
        editor->setText("0");
        QModelIndex index = model->index(0, col);
        ui->tableView->setIndexWidget(index, editor);
            connect(editor, &QLineEdit::editingFinished, this, [=]() {
                location[channel - 1][col - 1] = editor->text();
            });
    }






    /*// 第2行：模式
    for (int col = 1; col < 14; ++col) {
        QComboBox *combo2 = new QComboBox(ui->tableView);
        if (col==14)
        {
            combo2->addItems(QStringList()
            << "0：禁用输出" << "1：IO断路输出" << "2：IO开漏输出低电平" << "3：IO取反"
            << "5：500us低脉冲" << "6：1ms低脉冲" << "7：保留" << "8：保留"
            << "9：2ms脉冲" << "10：3ms脉冲" << "11：4ms脉冲" << "12：5ms脉冲"
            << "13：6ms脉冲" << "14：7ms脉冲" << "15：8ms脉冲" << "16：9ms脉冲"
            << "17：10ms脉冲" << "18：20ms脉冲" << "19：30ms脉冲" << "20：40ms脉冲"
            << "21：50ms脉冲" << "22：60ms脉冲" << "23：70ms脉冲" << "24：80ms脉冲"
            << "25：90ms脉冲" << "26：100ms脉冲" << "27：200ms脉冲" << "28：300ms脉冲"
            << "29：400ms脉冲" << "30：500ms脉冲");
        }
        else
        {
            combo2->addItems(QStringList()
            << "正向" << "反向");
        }


        ui->tableView->setIndexWidget(model->index(1, col), combo2);  // 行号改为1

        if (col == 1) {
            connect(combo2, &QComboBox::currentTextChanged, this, [=]() {
                camera_out_action[channel - 1] = combo2->currentText();
            });
        } else {
            connect(combo2, &QComboBox::currentTextChanged, this, [=]() {
                valve_out_action[channel - 1][col - 2] = combo2->currentText();
            });
        }
    }*/





    // 第2行：模式（改为简单的 QLineEdit，无连接，无逻辑）
    for (int col = 1; col < 14; ++col) {
        QLineEdit *editor = new QLineEdit(ui->tableView);
        editor->setText("");  // 初始内容为空，可改为默认值
        ui->tableView->setIndexWidget(model->index(1, col), editor);
    }





    /*// 第3行：按钮
    for (int col = 1; col < 14; ++col) {
        QPushButton *button = new QPushButton("补正", ui->tableView);
        QModelIndex index = model->index(2, col);
        ui->tableView->setIndexWidget(index, button);


            connect(button, &QPushButton::clicked, this, [=]() {
        QModelIndex posIndex = model->index(0, col);  // 位置行
        QModelIndex modeIndex = model->index(1, col); // 模式行


        QWidget *posWidget = ui->tableView->indexWidget(posIndex);

        QWidget *modeWidget = ui->tableView->indexWidget(modeIndex);

if (QLineEdit *edit = qobject_cast<QLineEdit*>(posWidget)) {
    if (QComboBox *combo = qobject_cast<QComboBox*>(modeWidget)) {
        bool ok = false;
        int val = edit->text().toInt(&ok);
        if (ok) {
            if (combo->currentText() == "正向") {
                val += 5;
            } else if (combo->currentText() == "反向") {
                val -= 5;
            }
            edit->setText(QString::number(val));
            location[channel - 1][col - 1] = QString::number(val);
        }
    }
}
});

    }*/



    // 第3行：按钮（点击后让第1行的值 += 第2行的值）
    for (int col = 1; col < 14; ++col) {
        QPushButton *button = new QPushButton("补正", ui->tableView);
        QModelIndex index = model->index(2, col);
        ui->tableView->setIndexWidget(index, button);

        connect(button, &QPushButton::clicked, this, [=]() {
            QModelIndex posIndex = model->index(0, col);  // 第1行：位置
            QModelIndex modeIndex = model->index(1, col); // 第2行：模式

            QWidget *posWidget = ui->tableView->indexWidget(posIndex);
            QWidget *modeWidget = ui->tableView->indexWidget(modeIndex);

            QLineEdit *posEdit = qobject_cast<QLineEdit *>(posWidget);
            QLineEdit *modeEdit = qobject_cast<QLineEdit *>(modeWidget);

            if (posEdit && modeEdit) {
                bool ok1 = false, ok2 = false;
                int posVal = posEdit->text().toInt(&ok1);
                int deltaVal = modeEdit->text().toInt(&ok2);
                if (ok1 && ok2) {
                    int newVal = posVal + deltaVal;
                    posEdit->setText(QString::number(newVal));
                    // 若仍需更新到 location 数组，请取消注释下一行：
                     location[channel - 1][col - 1] = QString::number(newVal);
                }
            }
        });
    }




    // 样式调整
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setVisible(false);
}



void CalibrationWindow::disposition() {
    saveConfig();
    for (int col = 0; col < 13; ++col)
    {
        if (k==0)
            return;
        if (col==0)
        {
            int loaction=location[channel-1][0].toInt()*k;
            myBoardCard->camera_location[channel-1]=QString::number(loaction);
            //LOG_INFO(myBoardCard->camera_location[channel-1]);
        }else
        {
            int loaction=location[channel-1][col].toInt()*k;
            myBoardCard->valve_location[channel-1][col]=QString::number(loaction);
            //LOG_INFO(myBoardCard->valve_location[channel-1][col]);
        }
    }
    myBoardCard->LoadConfigToPage();
}

void CalibrationWindow::disposition2() {
    for (int col = 0; col < 13; ++col)
    {
        if (col==0)
        {
            int loaction=myBoardCard->camera_location[channel-1].toInt()/k;
            location[channel-1][0]=QString::number(loaction);
        }else
        {
            int loaction=myBoardCard->valve_location[channel-1][col].toInt()/k;
            location[channel-1][col]=QString::number(loaction);
        }
    }
    LoadLocationRowToTable();
}


/*void CalibrationWindow::correction() {
    myBoardCard->camera_location[0]=camera_location[0].toInt()*k;
    for (int i = 1; i < 13; ++i) {
        myBoardCard->valve_location[0][i]=valve_location[0][i].toInt()*k;
    }

}*/


void CalibrationWindow::saveConfig()
{
    QSettings settings(pathFile, QSettings::IniFormat);

    settings.beginGroup("mark");
    for (int i = 0; i < location.size(); ++i) {
        QString key = QString("location%1").arg(i + 1);
        settings.setValue(key, location[i].join(","));
    }
    settings.endGroup();
}

void CalibrationWindow::saveConfigCamera()
{
    QSettings settings(pathFile, QSettings::IniFormat);

    if (ui->lineEdit_5->text().isEmpty()) return;
    else ratio = ui->lineEdit_5->text().toDouble();
    settings.beginGroup("config");
    settings.setValue("markraito",k);
    settings.setValue("camraito",ratio);
    settings.endGroup();
}


void CalibrationWindow::LoadConfig()
{
    if (!QFile::exists(pathFile)) {
        QMessageBox::warning(nullptr, "配置文件不存在",
                             QString("配置文件 \"%1\" 不存在，请检查文件路径或先保存配置。").arg(pathFile));
        return;
    }

    QSettings settings(pathFile, QSettings::IniFormat);

    location.clear();

    settings.beginGroup("mark");
    for (int i = 0; i < 3; ++i) {
        QString key = QString("location%1").arg(i + 1);
        QString value = settings.value(key).toString();
        QStringList list = value.split(",");
        location.append(list);
    }
    settings.endGroup();
}


void CalibrationWindow::LoadConfigCamera()
{
    if (!QFile::exists(pathFile)) {
        QMessageBox::warning(nullptr, "配置文件不存在",
                             QString("配置文件 \"%1\" 不存在，请检查文件路径或先保存配置。").arg(pathFile));
        return;
    }

    QSettings settings(pathFile, QSettings::IniFormat);

    settings.beginGroup("config");

    double kValue = settings.value("markraito", 24).toDouble();
    double ratioValue = settings.value("camraito", 0.005).toDouble();
    settings.endGroup();

    ratio=ratioValue;
    ui->lineEdit_5->setText(QString::number(ratio));
    k = kValue;
    //ui->lineEdit_4->setText(QString::number(k));
}


void CalibrationWindow::LoadLocationRowToTable()
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->tableView->model());
    if (!model) return;

    for (int col = 1; col < 14; ++col) {
        QModelIndex index = model->index(0, col);
        QWidget *widget = ui->tableView->indexWidget(index);
        if (QLineEdit *edit = qobject_cast<QLineEdit*>(widget)) {
            edit->setText(location[channel-1][col - 1]);
        }
    }
}



void CalibrationWindow::writeLogToFile(const QString &logText) {

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

void CalibrationWindow::updaEncoders()
{
    if (calibration1==0)
    {
        ui->lineEdit->setText(QString::number(myBoardCard->Encoder));
    }
    else if (calibration2==0&&calibration1==1)
    {
        ui->lineEdit_2->setText(QString::number(myBoardCard->Encoder));
    }
    else if (Encoder==0)
    {
        Encoder=qAbs(ui->lineEdit->text().toInt()-ui->lineEdit_2->text().toInt());
    }
   QStringList Trig= myBoardCard->Get_Trig_Blow();
    //ui->textBrowser->setText(Trig.join("\n"));
}


void CalibrationWindow::writeLog(const QString &logText) {

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
