//
// Created by Adminstrator on 25-7-23.
//

// You may need to build the project (run Qt uic code generator) to get "ui_BoardCard.h" resolved

#include "boardcard.h"
#include "ui_BoardCard.h"
#include <QVBoxLayout>
#include <QTimer>


BoardCard::BoardCard(QWidget *parent) :
    VpMainWindow(parent), ui(new Ui::BoardCard) {
    ui->setupUi(this);
    InitWindow();
    InitConnect();
    LoadConfig();
    LoadConfigToPage();

    this->setWindowTitle("板卡设置");
    QTimer *encoderTimer;
    QTimer *channelTimer;
    rightLayout = new QVBoxLayout(ui->groupBox_8);
    rightLayout->addWidget(createStatusGroup("通道一", 3,1));
    rightLayout->addWidget(createStatusGroup("通道二", 3,2));
    rightLayout->addWidget(createStatusGroup("通道三", 3,3));
    lastTime = QVector<QTime>(3, QTime::currentTime());
    lastCamcons=camcons;
    lastMaterials=materials;
    //startMonitoring();//监控相机和入料总数
    //获取板卡数据
    encoderTimer = new QTimer(this);
    connect(encoderTimer, &QTimer::timeout, this, &BoardCard::updateEncoderValue);
    encoderTimer->start(10);


    // 相机计数器
    QTimer *camnum = new QTimer(this);
    connect(camnum, &QTimer::timeout, this, [this]() {
        Get_Camera_Counts();
    });
    camnum->start(5);

    //ui更新
    encoderTimer = new QTimer(this);
    connect(encoderTimer, &QTimer::timeout, this, &BoardCard::updateUiValue);
    encoderTimer->start(500);

    /*QTimer* material23 = new QTimer(this);
    connect(material23, &QTimer::timeout, this, &BoardCard::Get_Trig_Blow);
    material23->start(5000);*/

    //监听状态，数据刷新
    channelTimer = new QTimer(this);
    connect(channelTimer, &QTimer::timeout, this, &BoardCard::updateChannelValue);
    channelTimer->start(5);


    /*//监听状态，数据刷新
    channelTimer = new QTimer(this);
    connect(channelTimer, &QTimer::timeout, this, &BoardCard::updateChannelValue);
    channelTimer->start(5);*/


    /*// 叠料
    QTimer *stacking1 = new QTimer(this);
    connect(stacking1, &QTimer::timeout, this, [this]() {
        if (BoarState==0)
        {listeningStacking1();
        listeningStacking2();
        listeningStacking3();
        }

    });
    stacking1->start(5);*/




    //计算通道速度
    QTimer* materialseep = new QTimer(this);
    connect(materialseep, &QTimer::timeout, this, &BoardCard::Get_channel_Speed);
    materialseep->start(3000);


    this->setFixedSize(1350, 520);

    connect(ui->btnChannel_4, &QPushButton::clicked, this, &BoardCard::savepParameter);

    connect(ui->btnChannel_1, &QPushButton::clicked, this, [=]() {
        channel=1;
        LoadConfigToPage();
        ui->btnChannel_1->setStyleSheet("border: 2px solid #00AAFF; border-radius: 5px;");
        ui->btnChannel_2->setStyleSheet("");
        ui->btnChannel_3->setStyleSheet("");

});
    connect(ui->btnChannel_2, &QPushButton::clicked, this, [=]() {
        channel=2;
        LoadConfigToPage();
        ui->btnChannel_2->setStyleSheet("border: 2px solid #00AAFF; border-radius: 5px;");
        ui->btnChannel_1->setStyleSheet("");
        ui->btnChannel_3->setStyleSheet("");
});
    connect(ui->btnChannel_3, &QPushButton::clicked, this, [=]() {
        channel=3;
        LoadConfigToPage();
        ui->btnChannel_3->setStyleSheet("border: 2px solid #00AAFF; border-radius: 5px;");
        ui->btnChannel_1->setStyleSheet("");
        ui->btnChannel_2->setStyleSheet("");
    });

    connect(ui->btnOpenCard, &QPushButton::clicked, this, &BoardCard::openSift);
    connect(ui->btnCloseCard, &QPushButton::clicked, this, &BoardCard::stopSift);
    connect(ui->pushButton, &QPushButton::clicked, this, &BoardCard::clearcount);

    Stacking[0]=valve_out_action[0][0];
    Stacking[1]=valve_out_action[1][0];
    Stacking[2]=valve_out_action[2][0];


    //灯光
    /*QTimer *timer2 = new QTimer(this);
    connect(timer2, &QTimer::timeout, this, &BoardCard::updateLight);
    timer2->start(300);*/


    //获取板卡状态
    QTimer *timer3 = new QTimer(this);
    connect(timer3, &QTimer::timeout, this, &BoardCard::GEt_Board_state);
    timer3->start(5000);

    loadAppConfig();

    //setValveParameter();
    ui->groupBox_8->hide();
}

void BoardCard::updateEncoderValue() {

    Get_Rotary_Encoders();

    Get_Rotary_Encoder(boards_number[channel-1].toShort());
    Get_Camera_Count(camera_name[channel-1].mid(2).toShort(),boards_number[channel-1].toShort());

    Get_material_Count(boards_number[channel-1].toShort());

    MCF_Screen_Get_Distance_Encoder(boards_number[channel-1].toShort());

}


void BoardCard::updateUiValue() {

    ui->label_8->setText(QString::number(Encoder));//编码器位置
    ui->label_9->setText(QString::number(camcon));//拍照次数
    ui->label_19->setText(QString::number(Distance));//吹气位置
    ui->label_18->setText(QString::number(material));//来料总数
}


void BoardCard::updateChannelValue() {
    //获取入料数
    Get_material_Counts();
   // Get_channel_Speed();
    //获取3个板卡DI0状态
    Get_DI0_Status();

    //获取3个板卡DO0状态
    Get_DO0_Status();

    //监听叠料 in2，out1
    //listeningStacking();

    //状态更新
    updateEncoderValue();
}


void BoardCard::listeningStacking() {
    short ret;
    unsigned short level=-0;
    unsigned short inPort=2;
    int pulse=0;
    int action=1000;
    for (int i=0;i<boards_counts.toInt();i++) {
        action=valve_out_action[boards_number[i].toShort()][0].split("：")[1].replace("ms", "").toInt();
        pulse=extractRawTimeValue(valve_out_action[i][0]).toInt();
        ret= MCF_Get_Input_Bit_Net(inPort,&level,boards_number[i].toShort());
        if (ret==0&level==0) {
           ret= MCF_Set_Output_Bit_Net(1,0,boards_number[i].toShort());
            Sleep(action);
           //QTimer::singleShot(pulse, this, [this,i]() {
           short ret= MCF_Set_Output_Bit_Net(1,1,boards_number[i].toShort());
            // });
        }
    }

}

// 监控第一个板卡
void BoardCard::listeningStacking1() {
    short ret;
    unsigned short level = 0;
    unsigned short inPort = 2;
    int boardIndex = 0;
    if (boardIndex >= boards_number.size()) return;

    //int pulse = extractRawTimeValue(valve_out_action[boardIndex][0]).toInt();
    int action = valve_out_action[boards_number[boardIndex].toShort()][0].split("：")[1].replace("ms", "").toInt();

    ret = MCF_Get_Input_Bit_Net(inPort, &level, boards_number[boardIndex].toShort());
    if (ret == 0 && level == 0) {
        int boardId = boards_number[boardIndex].toShort();


        QTimer::singleShot(action, this, [this, boardId]() {
            MCF_Set_Output_Bit_Net(1, 0, boardId);

        QTimer::singleShot(100, this, [this, boardId]() {
                MCF_Set_Output_Bit_Net(1, 1, boardId);
            });
        });
    }

}

// 监控第二个板卡
void BoardCard::listeningStacking2() {
    short ret;
    unsigned short level = 0;
    unsigned short inPort = 2;
    int boardIndex = 1;
    if (boardIndex >= boards_number.size()) return;

    //int pulse = extractRawTimeValue(valve_out_action[boardIndex][0]).toInt();
    int action = valve_out_action[boards_number[boardIndex].toShort()][0].split("：")[1].replace("ms", "").toInt();

    ret = MCF_Get_Input_Bit_Net(inPort, &level, boards_number[boardIndex].toShort());
    if (ret == 0 && level == 0) {
        int boardId = boards_number[boardIndex].toShort();

        QTimer::singleShot(action, this, [this, boardId]() {
            MCF_Set_Output_Bit_Net(1, 0, boardId);

        QTimer::singleShot(100, this, [this, boardId]() {
                MCF_Set_Output_Bit_Net(1, 1, boardId);
            });
        });
    }
}

// 监控第三个板卡
void BoardCard::listeningStacking3() {
    short ret;
    unsigned short level = 0;
    unsigned short inPort = 2;
    int boardIndex = 2;
    if (boardIndex >= boards_number.size()) return;

    //int pulse = extractRawTimeValue(valve_out_action[boardIndex][0]).toInt();
    int action = valve_out_action[boards_number[boardIndex].toShort()][0].split("：")[1].replace("ms", "").toInt();

    ret = MCF_Get_Input_Bit_Net(inPort, &level, boards_number[boardIndex].toShort());
    if (ret == 0 && level == 0) {
        int boardId = boards_number[boardIndex].toShort();

        QTimer::singleShot(action, this, [this, boardId]() {
            MCF_Set_Output_Bit_Net(1, 0, boardId);

        QTimer::singleShot(100, this, [this, boardId]() {
                MCF_Set_Output_Bit_Net(1, 1, boardId);
            });
        });
    }
}



void BoardCard::listeningStartMotor()
{
    short ret;
    unsigned short level = 0;
    unsigned short inPort = 3;
    int boardIndex = 0;

    ret = MCF_Get_Input_Bit_Net(inPort, &level, boards_number[boardIndex].toShort());
    if (ret == 0 && level == 0) {
        emit plcStartMotor(0);
    }
}

void BoardCard::listeningStopMotor()
{
    short ret;
    unsigned short level = 0;
    unsigned short inPort = 4;
    int boardIndex = 0;

    ret = MCF_Get_Input_Bit_Net(inPort, &level, boards_number[boardIndex].toShort());
    if (ret == 0 && level == 0) {
        emit plcStopMotor(0);
    }
}


BoardCard::~BoardCard() {
    closeBoard();
    delete ui;
}

void BoardCard::InitWindow() {

    model = new QStandardItemModel(4, 15, this);


    QStringList headers = {"设备", "相机"};
    for (int i = 1; i <= 13; ++i)
        headers << QString("气阀%1").arg(i);
    model->setHorizontalHeaderLabels(headers);

    QStringList rowLabels = {"位置", "IO位号", "输出动作", "测试"};

    for (int row = 0; row < rowLabels.size(); ++row) {
        model->setItem(row, 0, new QStandardItem(rowLabels[row])); // 注意从第0行开始，覆盖第0列
    }

    for (int col = 1; col < 15; ++col) {
        model->setItem(0, col, new QStandardItem("")); // 空白
    }

    ui->tableView->setModel(model);

    // 第1行
    for (int col = 1; col < 15; ++col) {
        QLineEdit *editor = new QLineEdit(ui->tableView);
        editor->setText("0");
        QModelIndex index = model->index(0, col);
        ui->tableView->setIndexWidget(index, editor);

        if (col == 1) {
            connect(editor, &QLineEdit::editingFinished, this, [=]() {
                camera_location[channel - 1] = editor->text();
            });
        } else {
            connect(editor, &QLineEdit::editingFinished, this, [=]() {
                valve_location[channel - 1][col - 2] = editor->text();
            });
        }
    }

    // 第2行
    for (int col = 1; col < 15; ++col) {
        QComboBox *combo = new QComboBox(ui->tableView);
        combo->addItems(QStringList() << "DO00"<< "DO01" << "DO02" << "DO03" << "DO04"
                                     << "DO05" << "DO06" << "DO07" << "DO08"
                                     << "DO09" << "DO10" << "DO11" << "DO12"
                                     << "DO13" << "DO14" << "DO15");
        ui->tableView->setIndexWidget(model->index(1, col), combo);

        if (col == 1) {
            connect(combo, &QComboBox::currentTextChanged, this, [=]() {
           camera_out_tags[channel-1]=combo->currentText();
        });
        } else {
            connect(combo, &QComboBox::currentTextChanged, this, [=]() {
           valve_out_tags[channel - 1][col - 2]=combo->currentText();
        });
        }
    }



    // 第3行
    for (int col = 1; col < 15; ++col) {
    if (col == 2) {
        // 对于第2列，使用combo1
        QComboBox *combo1 = new QComboBox(ui->tableView);
        combo1->addItems(QStringList() << "0：100ms" << "1：200ms" << "2：300ms" << "3：400ms" << "5：500ms"
                               << "6：600ms" << "7：700ms" << "8：900ms" << "9：1000ms" << "10：1100ms"
                               << "11：1200ms" << "12：1300ms" << "13：1400ms" << "14：1500ms" << "15：1600ms"
                               << "16：1700ms" << "17：1800ms" << "18：1900ms" << "19：2000ms" << "20：2100ms"
                               << "21：2200ms" << "22：2300ms" << "23：2400ms" << "24：2500ms" << "25：2600ms"
                               << "26：2700ms" << "27：2800ms" << "28：2900ms" << "29：3000ms" << "30：3100ms"
                               << "31：3200ms" << "32：3300ms" << "33：3400ms" << "34：3500ms" << "35：3600ms");

        ui->tableView->setIndexWidget(model->index(2, col), combo1);

        // 连接combo1的信号
        connect(combo1, &QComboBox::currentTextChanged, this, [=]() {
            // 这里根据您的需求设置对应的数据存储
            // 例如：valve_out_action[channel - 1][col - 2] = combo1->currentText();
            valve_out_action[channel - 1][col - 2] = combo1->currentText();
        });
    } else {
        // 对于其他列，使用combo2
        QComboBox *combo2 = new QComboBox(ui->tableView);
        combo2->addItems(QStringList() << "0：禁用输出" << "1：IO断路输出" << "2：IO开漏输出低电平" << "3：IO取反" << "5：500us低脉冲"
                               << "6：1ms低脉冲" << "7：保留" << "8：保留" << "9：2ms脉冲" << "10：3ms脉冲"
                               << "11：4ms脉冲" << "12：5ms脉冲" << "13：6ms脉冲" << "14：7ms脉冲" << "15：8ms脉冲"
                               << "16：9ms脉冲" << "17：10ms脉冲" << "18：20ms脉冲" << "19：30ms脉冲" << "20：40ms脉冲"
                               << "21：50ms脉冲" << "22：60ms脉冲" << "23：70ms脉冲" << "24：80ms脉冲" << "25：90ms脉冲"
                               << "26：100ms脉冲" << "27：200ms脉冲" << "28：300ms脉冲" << "29：400ms脉冲" << "30：500ms脉冲");

        ui->tableView->setIndexWidget(model->index(2, col), combo2);

        // 连接combo2的信号
        if (col == 1) {
            connect(combo2, &QComboBox::currentTextChanged, this, [=]() {
                camera_out_action[channel - 1] = combo2->currentText();
            });
        } else {
            connect(combo2, &QComboBox::currentTextChanged, this, [=]() {
                valve_out_action[channel - 1][col - 2] = combo2->currentText();
            });
        }
    }
}
    /*for (int col = 1; col < 15; ++col) {
        QComboBox *combo2 = new QComboBox(ui->tableView);
        /*QComboBox *combo1 = new QComboBox(ui->tableView);
        combo1->addItems(QStringList() << "0：100ms" << "1：200ms" << "2：300ms" << "3：400ms" << "5：500ms"
                               << "6：600ms" << "7：700ms" << "8：900ms" << "9：1000ms" << "10：1100ms"
                               << "11：1200ms" << "12：1300ms" << "13：1400ms" << "14：1500ms" << "15：1600ms"
                               << "16：1700ms" << "17：1800ms" << "18：1900ms" << "19：2000ms" << "20：2100ms"
                               << "21：2200ms" << "22：2300ms" << "23：2400ms" << "24：2500ms" << "25：2600ms"
                               << "26：2700ms" << "27：2800ms" << "28：2900ms" << "29：3000ms" << "30：3100ms"
                               << "31：3200ms" << "32：3300ms" << "33：3400ms" << "34：3500ms" << "35：3600ms");#1#
        combo2->addItems(QStringList() << "0：禁用输出" << "1：IO断路输出" << "2：IO开漏输出低电平" << "3：IO取反" << "5：500us低脉冲"
                               << "6：1ms低脉冲" << "7：保留" << "8：保留" << "9：2ms脉冲" << "10：3ms脉冲"
                               << "11：4ms脉冲" << "12：5ms脉冲" << "13：6ms脉冲" << "14：7ms脉冲" << "15：8ms脉冲"
                               << "16：9ms脉冲" << "17：10ms脉冲" << "18：20ms脉冲" << "19：30ms脉冲" << "20：40ms脉冲"
                               << "21：50ms脉冲" << "22：60ms脉冲" << "23：70ms脉冲" << "24：80ms脉冲" << "25：90ms脉冲"
                               << "26：100ms脉冲" << "27：200ms脉冲" << "28：300ms脉冲" << "29：400ms脉冲" << "30：500ms脉冲");

        /*if (col==2)
        {
            ui->tableView->setIndexWidget(model->index(2, col), combo1);
        }else
        {#1#
            ui->tableView->setIndexWidget(model->index(2, col), combo2);
        //}


        if (col == 1) {
            connect(combo2, &QComboBox::currentTextChanged, this, [=]() {
           camera_out_action[channel-1]=combo2->currentText();
        });
        }else if (col == 2)
        {
            connect(combo2, &QComboBox::currentTextChanged, this, [=]() {
          valve_out_action[channel - 1][col - 2]=combo2->currentText();
       });
        }
        else {
            connect(combo2, &QComboBox::currentTextChanged, this, [=]() {
           valve_out_action[channel - 1][col - 2]=combo2->currentText();
        });
        }
    }*/




    // 第4行：输出动作（QLineEdit）
    for (int col = 1; col < 15; ++col) {
        QPushButton *button = new QPushButton("输出", ui->tableView);
        QModelIndex index = model->index(3, col);
        ui->tableView->setIndexWidget(index, button);


        if (col == 1) {
            connect(button, &QPushButton::clicked, this, [=]() {
                unsigned short Dio = camera_out_tags[channel - 1].mid(2).toUShort();
                unsigned short board = boards_number[channel - 1].toUShort();
                unsigned short camname = camera_name[channel - 1].mid(2).toUShort();
                unsigned short action =extractCode(camera_out_action[channel - 1]).toUShort();


                    //testOutput(Dio, 1, board);
                    testOutput2(camname,1,action,Dio,board);


            });
        } else {
            connect(button, &QPushButton::clicked, this, [=]() {
                unsigned short Dio = valve_out_tags[channel - 1][col - 2].mid(2).toUShort();
                unsigned short board = boards_number[channel - 1].toUShort();
                unsigned short valvename = valve_name[channel - 1][col - 2].mid(2).toUShort();
                unsigned short valveaction = extractCode(valve_out_action[channel - 1][col - 2]).toUShort();


                // if (!toggled) {
                //     button->setText("复位");
                //     testOutput(Dio, 0, board);
                //     //testOutput1(valvename,1,board);
                // } else {
                //     button->setText("输出");
                //     testOutput(Dio, 1, board);
                //     ///testOutput1(valvename,1,board);
                // }
                testOutput2(valvename,1,valveaction,Dio,board);

            });
        }
    }



    // 样式调整（根据需要）
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setVisible(false);
}

void BoardCard::InitConnect() {



    connect(ui->cmbMaterialPort_2, &QComboBox::currentTextChanged, this, [=]() {
           boards_counts=ui->cmbMaterialPort_2->currentText();
    });
    connect(ui->lineEdit_6, &QLineEdit::editingFinished, this, [=]() {
        boards_number[channel-1]=ui->lineEdit_6->text();
    });
    connect(ui->cmbMaterialPort_3, &QComboBox::currentTextChanged, this, [=]() {
      // boards_type[channel-1]=ui->cmbMaterialPort_3->currentText();
    });


    connect(ui->comboBox, &QComboBox::currentTextChanged, this, [=]() {
       material_detect[channel-1]=ui->comboBox->currentText();
    });
    connect(ui->cmbMaterialPort, &QComboBox::currentTextChanged, this, [=]() {
       material_Inlet[channel-1]=ui->cmbMaterialPort->currentText();
    });
    connect(ui->cmbMaterialLevel, &QComboBox::currentTextChanged, this, [=]() {
       material_level[channel-1]=ui->cmbMaterialLevel->currentText();
    });
    connect(ui->lineEdit, &QLineEdit::editingFinished, this, [=]() {
        material_filtering[channel-1]=ui->lineEdit->text();
    });
    connect(ui->cmbTriggerPos, &QComboBox::currentTextChanged, this, [=]() {
       material_photospot[channel-1]=ui->cmbTriggerPos->currentText();
    });



    connect(ui->comboBox_5, &QComboBox::currentTextChanged, this, [=]() {
           camera_direction[channel-1]=ui->comboBox_5->currentText();
        });
    connect(ui->lineEdit_5, &QLineEdit::editingFinished, this, [=]() {
           camera_delay[channel-1]=ui->lineEdit_5->text();
       });
    connect(ui->spinBox, &QSpinBox::editingFinished, this, [=]() {
          camera_offset[channel-1]=QString::number(ui->spinBox->value());
      });
    connect(ui->lineEdit_7, &QLineEdit::editingFinished, this, [=]() {
           camera_light_advance[channel-1]=ui->lineEdit_7->text();
       });


    connect(ui->lineEdit_2, &QLineEdit::editingFinished, this, [=]() {
          image_proctime[channel-1]=ui->lineEdit_2->text();
      });
    connect(ui->lineEdit_3, &QLineEdit::editingFinished, this, [=]() {
          image_timeout[channel-1]=ui->lineEdit_3->text();
      });
    connect(ui->lineEdit_4, &QLineEdit::editingFinished, this, [=]() {
          image_number[channel-1]=ui->lineEdit_4->text();
      });
}


void BoardCard::savepParameter() {

    QFileInfo fileInfo(pathFile);
    QDir dir = fileInfo.dir();

    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            return;
        }
    }

    QSettings settings(pathFile, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");

    for (int i = 0; i < 3; i++) {
        QString groupName = QString("channel%1").arg(i + 1);
        settings.beginGroup(groupName);
        if (i==0) {
            settings.setValue("boards_counts", boards_counts);
        }
        settings.setValue("boards_number", boards_number[i]);
        settings.setValue("boards_type", boards_type[i]);

        settings.setValue("material_detect", material_detect[i]);
        settings.setValue("material_Inlet", material_Inlet[i]);
        settings.setValue("material_level", material_level[i]);
        settings.setValue("material_filtering", material_filtering[i]);
        settings.setValue("material_photospot", material_photospot[i]);

        settings.setValue("camera_name", camera_name[i]);
        settings.setValue("camera_direction", camera_direction[i]);
        settings.setValue("camera_delay", camera_delay[i]);
        settings.setValue("camera_offset", camera_offset[i]);
        settings.setValue("camera_light_advance", camera_light_advance[i]);
        settings.setValue("camera_location", camera_location[i]);
        settings.setValue("camera_out_tags", camera_out_tags[i]);
        settings.setValue("camera_out_action", camera_out_action[i]);

        settings.setValue("image_proctime", image_proctime[i]);
        settings.setValue("image_timeout", image_timeout[i]);
        settings.setValue("image_number", image_number[i]);

        settings.setValue("valve_name", valve_name[i].join(","));
        settings.setValue("valve_location", valve_location[i].join(","));
        settings.setValue("valve_out_tags", valve_out_tags[i].join(","));
        settings.setValue("valve_out_action", valve_out_action[i].join(","));

        settings.endGroup();  // 对应每次 beginGroup
    }

    settings.sync();  // 可选，立即写入文件


    //InitBoardConfig();
    setCameraParameter();
    setValveParameter();

}




void BoardCard::LoadConfig()
{

    if (!QFile::exists(pathFile)) {
        QMessageBox::warning(nullptr, "配置文件不存在",
                             QString("配置文件 \"%1\" 不存在，请检查文件路径或先保存配置。").arg(pathFile));
        return;
    }

    QSettings settings(pathFile, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");

    boards_counts = settings.value("channel1/boards_counts", "1").toString();

    for (int i = 0; i < 3; ++i) {
        QString group = QString("channel%1").arg(i + 1);
        settings.beginGroup(group);

        boards_number[i]        = settings.value("boards_number", "0").toString();
        boards_type[i]          = settings.value("boards_type", "0").toString();

        material_detect[i]      = settings.value("material_detect", "0").toString();
        material_Inlet[i]       = settings.value("material_Inlet", "0").toString();
        material_level[i]       = settings.value("material_level", "0").toString();
        material_filtering[i]   = settings.value("material_filtering", "0").toString();
        material_photospot[i]   = settings.value("material_photospot", "0").toString();

        camera_name[i]          = settings.value("camera_name", "相机").toString();
        camera_direction[i]     = settings.value("camera_direction", "0").toString();
        camera_delay[i]         = settings.value("camera_delay", "0").toString();
        camera_offset[i]        = settings.value("camera_offset", "0").toString();
        camera_light_advance[i] = settings.value("camera_light_advance", "0").toString();
        camera_location[i]      = settings.value("camera_location", "0").toString();
        camera_out_tags[i]      = settings.value("camera_out_tags", "0").toString();
        camera_out_action[i]    = settings.value("camera_out_action", "0").toString();

        image_proctime[i]       = settings.value("image_proctime", "0").toString();
        image_timeout[i]        = settings.value("image_timeout", "0").toString();
        image_number[i]         = settings.value("image_number", "0").toString();

        // 逗号拆分
        QString valveNameStr = settings.value("valve_name", "").toString();
        valve_name[i] = valveNameStr.split(',');

        QString valveLocationStr = settings.value("valve_location", "").toString();
        valve_location[i] = valveLocationStr.split(',');

        QString valveOutTagsStr = settings.value("valve_out_tags", "").toString();
        valve_out_tags[i] = valveOutTagsStr.split(',');

        QString valveOutActionStr = settings.value("valve_out_action", "").toString();
        valve_out_action[i] = valveOutActionStr.split(',');

        settings.endGroup();
    }
}


void BoardCard::LoadConfigToPage() {

    ui->cmbMaterialPort_2->setCurrentText(boards_counts);
    ui->lineEdit_6->setText(boards_number[channel-1]);
    ui->cmbMaterialPort_3->setCurrentText(boards_type[channel-1]);

    ui->comboBox->setCurrentText(material_detect[channel-1]);
    ui->cmbMaterialPort->setCurrentText(material_Inlet[channel-1]);
    ui->cmbMaterialLevel->setCurrentText(material_level[channel-1]);
    ui->lineEdit->setText(material_filtering[channel-1]);
    ui->cmbTriggerPos->setCurrentText(material_photospot[channel-1]);

    ui->comboBox_5->setCurrentText(camera_direction[channel-1]);
    ui->lineEdit_5->setText(camera_delay[channel-1]);
    ui->spinBox->setValue(camera_offset[channel-1].toInt());
    ui->lineEdit_7->setText(camera_light_advance[channel-1]);

    ui->lineEdit_2->setText(image_proctime[channel-1]);
    ui->lineEdit_3->setText(image_timeout[channel-1]);
    ui->lineEdit_4->setText(image_number[channel-1]);

    //第一行
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
        QModelIndex idx = model->index(0, 1);
        QWidget* w = ui->tableView->indexWidget(idx);
        QLineEdit* editor = qobject_cast<QLineEdit*>(w);
        if (editor) {
            editor->setText(camera_location[channel-1]);
        }
    for (int col = 2; col <= 14; ++col) {
        QModelIndex idx = model->index(0, col);
        QWidget* w = ui->tableView->indexWidget(idx);
        QLineEdit* editor = qobject_cast<QLineEdit*>(w);
        if (editor) {
            int valveIndex = col - 2; // valve_location 列表索引对应
            editor->setText(valve_location[channel-1][valveIndex]);
        }
    }



  //第二行
    QModelIndex idx1 = model->index(1, 1);  // 第1行 第1列
    QWidget* w1 = ui->tableView->indexWidget(idx1);
    QComboBox* combo1 = qobject_cast<QComboBox*>(w1);
    if (combo1) {
            int index1= combo1->findText(camera_out_tags[channel-1]);
            if (index1 != -1) {
                combo1->setCurrentIndex(index1);
            } else {
                combo1->addItem(camera_out_tags[channel-1]);  // 如果没有，先添加再选中
                combo1->setCurrentText(camera_out_tags[channel-1]);
            }
        }

    // 设置 valve_out_tags[0] 到 第1行第2列 ~ 第14列
    for (int col = 2; col <= 14; ++col) {
        QModelIndex idx = model->index(1, col);  // 第1行 各列
        QWidget* w = ui->tableView->indexWidget(idx);
        QComboBox* combo = qobject_cast<QComboBox*>(w);
        if (combo) {
            QString value = valve_out_tags[channel-1][col - 2];  // 注意 col - 2 索引
            int index = combo->findText(value);
            if (index != -1) {
                combo->setCurrentIndex(index);
            } else {
                combo->addItem(value);
                combo->setCurrentText(value);
            }
        }
    }






    //第三行
    QModelIndex idx2 = model->index(2, 1);  // 第1行 第1列
    QWidget* w2 = ui->tableView->indexWidget(idx2);
    QComboBox* combo2 = qobject_cast<QComboBox*>(w2);
    if (combo2) {
        int index2= combo2->findText(camera_out_action[channel-1]);
        if (index2 != -1) {
            combo2->setCurrentIndex(index2);
        } else {
            combo2->addItem(camera_out_action[channel-1]);  // 如果没有，先添加再选中
            combo2->setCurrentText(camera_out_action[channel-1]);
        }
    }

    // 设置 valve_out_tags[0] 到 第1行第2列 ~ 第14列
    for (int col = 2; col <= 14; ++col) {
        QModelIndex idx = model->index(2, col);  // 第1行 各列
        QWidget* w = ui->tableView->indexWidget(idx);
        QComboBox* combo = qobject_cast<QComboBox*>(w);
        if (combo) {
            QString value = valve_out_action[channel-1][col - 2];  // 注意 col - 2 索引
            int index = combo->findText(value);
            if (index != -1) {
                combo->setCurrentIndex(index);
            } else {
                combo->addItem(value);
                combo->setCurrentText(value);
            }
        }
    }


}



int BoardCard::openBoard() {

    if (boards_counts=="1") {
        unsigned short connection_number = boards_counts.toShort();
        unsigned short station_number[1] = {
            static_cast<unsigned short>(boards_number[0].toUShort()),
        };
        unsigned short station_type[1]= {
            static_cast<unsigned short>(boards_type[0].toUShort()),
        };


        short ret=0;

        for (int i = 0; i < connection_number; i++) {
            ret=MCF_Sorting_Init_Net(station_number[i]);
            if (ret!=0) {
                QMessageBox::critical(this, "初始化失败",
                   QString("板卡 %1 初始化失败，错误码: %2").arg(station_number[i]).arg(ret));
                return -1;
            }
        }


        ret=MCF_Open_Net(connection_number,&station_number[0],&station_type[0]);
        if (ret!=0) {
            QMessageBox::critical(this, "打开失败",
               QString("板卡打开失败，错误码: %1").arg(ret));
            return -1;
        }

        /*QMessageBox::critical(this, "打开成功",
               QString("板卡打开成功"));*/
        return 0;

    }else if (boards_counts=="2") {
        unsigned short connection_number = boards_counts.toShort();
        unsigned short station_number[2] = {
            static_cast<unsigned short>(boards_number[0].toUShort()),
            static_cast<unsigned short>(boards_number[1].toUShort()),
        };
        unsigned short station_type[2]= {
            static_cast<unsigned short>(boards_type[0].toUShort()),
            static_cast<unsigned short>(boards_type[1].toUShort()),
        };



        short ret=0;

        for (int i = 0; i < connection_number; i++) {
            ret=MCF_Sorting_Init_Net(station_number[i]);
            if (ret!=0) {
                QMessageBox::critical(this, "初始化失败",
                   QString("板卡 %1 初始化失败，错误码: %2").arg(station_number[i]).arg(ret));
                return -1;
            }
        }


        ret=MCF_Open_Net(connection_number,&station_number[0],&station_type[0]);
        if (ret!=0) {
            QMessageBox::critical(this, "打开失败",
               QString("板卡打开失败，错误码: %1").arg(ret));
            return -1;
        }

        /*QMessageBox::critical(this, "打开成功",
               QString("板卡打开成功"));*/
        return 0;
    }
    else{
        unsigned short connection_number = boards_counts.toShort();
        unsigned short station_number[3] = {
            static_cast<unsigned short>(boards_number[0].toUShort()),
            static_cast<unsigned short>(boards_number[1].toUShort()),
            static_cast<unsigned short>(boards_number[2].toUShort())
        };
        unsigned short station_type[3]= {
            static_cast<unsigned short>(boards_type[0].toUShort()),
            static_cast<unsigned short>(boards_type[1].toUShort()),
            static_cast<unsigned short>(boards_type[2].toUShort())
        };



        short ret=0;

        for (int i = 0; i < connection_number; i++) {
            ret=MCF_Sorting_Init_Net(station_number[i]);
            if (ret!=0) {
                QMessageBox::critical(this, "初始化失败",
                   QString("板卡 %1 初始化失败，错误码: %2").arg(station_number[i]).arg(ret));
                return -1;
            }
        }


        ret=MCF_Open_Net(connection_number,&station_number[0],&station_type[0]);
        if (ret!=0) {
            QMessageBox::critical(this, "打开失败",
               QString("板卡打开失败，错误码: %1").arg(ret));
            return -1;
        }

        /*QMessageBox::critical(this, "打开成功",
               QString("板卡打开成功"));*/

        return 0;
    }



    //InitBoardConfig();


    }


int BoardCard::closeBoard() {
    short ret=0;
    for (int i = 0; i <boards_counts.toInt(); i++) {
        ret= MCF_Sorting_Close_Net(boards_number[i].toUShort());
        if (ret!=0) {
            QMessageBox::critical(this, "关闭失败",
               QString("板卡%1关闭筛选失败，错误码: %2").arg(boards_number[i]).arg(ret));
            return -1;
        }
    }

    ret = MCF_Close_Net();
    if (ret!=0) {
        QMessageBox::critical(this, "关闭失败",
           QString("板卡关闭失败，错误码: %1").arg(ret));
        return -1;
    }

    QMessageBox::critical(this, "关闭成功",
           QString("板卡关闭成功"));
    return 0;
}

int BoardCard::clearcount() {
    for (int i = 0; i <boards_counts.toInt(); i++) {
        short ret= MCF_Set_Encoder_Net(0,0,boards_number[i].toUShort());
        if (ret!=0) {
            QMessageBox::critical(this, "气阀设置失败",
               QString("板卡 %1编码器清0失败，错误码: %2").arg(boards_number[channel-1]).arg(ret));
            return -1;
        }

    }
    return 0;
}



int BoardCard::setCameraBlowParameter(){
    for (int i=0;i<boards_counts.toInt();i++) {
        short ret= MCF_Sorting_Camera_Blow_Config_Net(1,15,boards_number[i].toUShort());
        if (ret!=0) {
            QMessageBox::critical(this, "气阀设置失败",
               QString("板卡 %1 气阀设置失败，错误码: %2").arg(boards_number[i]).arg(ret));
            return -1;
        }
    }
    return 0;
}


int BoardCard::setCameraParameter(){
   unsigned short direction=0;
   unsigned short cameraaction=0;
   unsigned short outtags=0;
   unsigned short camname=0;
    QStringList log;
    for (int i=0;i<boards_counts.toInt();i++) {
        direction = (camera_direction[i].trimmed() == "逆方向") ? 1 : 0;
        cameraaction=extractCode(camera_out_action[i]).toShort();
        outtags = camera_out_tags[i].mid(2).toShort();
        camname=camera_name[i].mid(2).toShort();
        short ret= MCF_Sorting_Set_Camera_Net(camname,camera_location[i].toShort(),direction,
            cameraaction,outtags,boards_number[i].toUShort());

        if (ret!=0) {
            QMessageBox::critical(this, "",
               QString("板卡 %1 相机参数设置失败，错误码: %2").arg(boards_number[i]).arg(ret));
            return -1;
        }

        //ret=MCF_Sorting_Set_Trig_Camera_Delay_Count_Net();
         QString log2 = QString("相机号：%1 位置：%2 方向：%3 模式：%4 输出口：%5 站号：%6")
                             .arg(camname)
                             .arg(camera_location[i])
                             .arg(direction)
                             .arg(cameraaction)
                             .arg(outtags)
                             .arg(boards_number[i]);
        log.append(log2);
    }


    LOG_INFO(log.join("\n"));
    writeLog(log.join("\n"));
    return 0;
}

int BoardCard::setCameraDelay() {
    short camname=0;
    for (int i=0;i<boards_counts.toInt();i++) {
        camname=camera_name[i].mid(2).toUShort();
        short ret= MCF_Sorting_Set_Trig_Camera_Delay_Count_Net(camname,camera_delay[i].toUShort(),boards_number[i].toUShort());
        if (ret!=0) {
            QMessageBox::critical(this, "",
               QString("板卡 %1 相机延迟设置失败，错误码: %2").arg(boards_number[i]).arg(ret));
            return -1;
        }
    }
    return 0;
}

int BoardCard::setCameraOffset() {
    short camname=0;
    for (int i=0;i<boards_counts.toInt();i++) {
        camname=camera_name[i].mid(2).toUShort();
        short ret= MCF_Sorting_Set_Camera_Trig_Offset_Net(camname,camera_offset[i].toUShort(),boards_number[i].toUShort());
        if (ret!=0) {
            QMessageBox::critical(this, "",
               QString("板卡 %1 相机偏移设置失败，错误码: %2").arg(boards_number[i]).arg(ret));
            return -1;
        }
    }
    return 0;
}

int BoardCard::setValveParameter() {
   unsigned short direction=0;
   unsigned short cameraaction=0;
   unsigned short outtags=0;
   unsigned short valvename=0;
    long location=0;
    short boar=0;
    for (int i=0;i<boards_counts.toInt();i++) {
        for (int j=0;j<valve_name[i].size();j++) {
            direction = (camera_direction[i].trimmed() == "逆方向") ? 1 : 0;
            cameraaction=extractCode(valve_out_action[i][j]).toUShort();
            outtags = valve_out_tags[i][j].mid(2).toUShort();
            valvename=valve_name[i][j].mid(2).toUShort();
            location=valve_location[i][j].toLong();
            boar=boards_number[i].toUShort();
            short ret= MCF_Sorting_Set_Blow_Net(valvename,valve_location[i][j].toLong(),direction,
                cameraaction,outtags,boards_number[i].toUShort());

            if (ret!=0) {
                QMessageBox::critical(this, "",
                   QString("板卡 %1 气阀%2参数设置失败，错误码: %3").arg(boards_number[i]).arg(valvename).arg(ret));
                return -1;
            }
            QString log = QString("气阀号：%1 位置：%2 方向：%3 模式：%4 输出口：%5 站号：%6")
                      .arg(valvename)
                      .arg(location)
                      .arg(direction)
                      .arg(cameraaction)
                      .arg(outtags)
                      .arg(boar);

            LOG_INFO(log);
            writeLog(log);
        }
    }

    return 0;
}


void BoardCard::testOutput(unsigned short outionumber,unsigned short outlogic,unsigned outstation) {

    short ret=MCF_Set_Output_Bit_Net(outionumber,outlogic,outstation);
    if (ret!=0) {
        QMessageBox::critical(this, "",
               QString("DO %1 输出失败，错误码: %2").arg(outionumber).arg(ret));
        return;
    }else {

    }
}


void BoardCard::testOutput1(unsigned short valverumber,unsigned short piece,unsigned outstation) {

    short ret=MCF_Sorting_Set_Trig_Blow_Net(valverumber,piece,outstation);
    if (ret!=0) {
        QMessageBox::critical(this, "",
               QString("气阀 %1 输出失败，错误码: %2").arg(valverumber).arg(ret));
        return;
    }else {

    }
}



void BoardCard::testOutput2(
    unsigned short BlowChannal,
    unsigned short Motion_Dir,
    unsigned short Action_Mode,
    unsigned short Action_IO,
    unsigned short StationNumber)
{
    short rtn = 0;
    unsigned short Enable = 0;
    unsigned short Compare_Source = 0;
    long Pos = 0;



    rtn = MCF_Get_Compare_Config_Net(BlowChannal, &Enable, &Compare_Source, StationNumber);
    if (rtn!=0) {
        QMessageBox::critical(this, "",
               QString("获取启用配置失败，错误码: %1").arg(rtn));
        return;
    }
    if (Enable==0) {
         rtn = MCF_Set_Compare_Config_Net(BlowChannal, 1, 0, StationNumber);
    if (rtn!=0) {
        QMessageBox::critical(this, "",
               QString("启用比较失败，错误码: %1").arg(rtn));
        return;
    }
    }


    // 计算比较位置
    int Compare_Position = 10;
    // if (Motion_Dir == 0 || Motion_Dir == 2)
    //     Compare_Position = Pos + 10;
    // else if (Motion_Dir == 1 || Motion_Dir == 3)
    //     Compare_Position = Pos - 10;


    rtn = MCF_Add_Compare_Point_Net(
        BlowChannal,
        10,
        1,
        Action_Mode,
        Action_IO,
        StationNumber
    );
    if (rtn!=0) {
        QMessageBox::critical(this, "",
               QString("添加比较点失败，错误码: %1").arg(rtn));
        return;
    }

}


int BoardCard::InitBoardConfig() {
    int ret = setCameraBlowParameter();
    if (ret != 0) return ret;

    ret = setCameraParameter();
    if (ret != 0) return ret;

    ret = setCameraDelay();
    if (ret != 0) return ret;

    ret = setCameraOffset();
    if (ret != 0) return ret;

    ret = setValveParameter();
    if (ret != 0) return ret;

    return 0; // 全部成功

}


void BoardCard::Get_Rotary_Encoder(short StationNumber) {
    long in_Encoder;
   short rtn =MCF_Get_Encoder_Net(0, &in_Encoder, StationNumber);
    Encoder=in_Encoder;
}


void BoardCard::Get_Rotary_Encoders() {
    for (int i = 0; i < boards_counts.toInt(); i++) {
        long in_Encoder=0;
        short rtn = MCF_Get_Encoder_Net(0, &in_Encoder, boards_number[i].toUShort());
        if (rtn == 0) Encoders[i] = in_Encoder;
    }
}



void BoardCard::Get_Camera_Count(short cameraName ,short StationNumber) {
    unsigned long in_Encoder=0;
    short rtn =MCF_Sorting_Get_Trig_Camera_Count_Net(cameraName, &in_Encoder, StationNumber);
    //LOG_INFO(QString::number(rtn));
    camcon=in_Encoder;
}

void BoardCard::Get_Camera_Counts() {
    for (int i = 0; i < boards_counts.toInt(); i++) {
        unsigned long in_Encoder=0;
        short rtn =MCF_Sorting_Get_Trig_Camera_Count_Net(camera_name[i].mid(2).toShort(), &in_Encoder, boards_number[i].toUShort());
         if (rtn == 0)camcons[i]=in_Encoder;
    }

}


void BoardCard::Get_material_Count(short StationNumber) {

    short rtn =MCF_Sorting_Set_Input_Enable_Net(0,1,StationNumber);
    //LOG_INFO(QString::number(rtn));

    unsigned long Trigger_Piece_Pass[8][2];
    unsigned long* Piece_Pass = &Trigger_Piece_Pass[StationNumber][Trigger1];
     rtn = MCF_Sorting_Get_Piece_Pass_Net(Trigger1, Piece_Pass, StationNumber);
    //LOG_INFO(QString::number(rtn));
    Rate_Piece_Pass[Trigger1] = Trigger_Piece_Pass[StationNumber][Trigger1];


    unsigned long Trigger_Piece_Find[8][2];
    unsigned long Trigger_Piece_Cross_Camera[8];
    unsigned long Temp_Piece_Size[10] = {0};
    unsigned long Temp_Piece_Distance_To_next[10] = {0};

    rtn = MCF_Sorting_Get_Piece_State_Net(
    Trigger1,
    &Trigger_Piece_Find[StationNumber][Trigger1],
    Temp_Piece_Size,
    Temp_Piece_Distance_To_next,
    &Trigger_Piece_Cross_Camera[Trigger1],
    StationNumber
    );
    //LOG_INFO(QString::number(rtn));
    Trigger_Piece_Find4[Trigger1] = Trigger_Piece_Find[StationNumber][Trigger1];
    material=Trigger_Piece_Find4[Trigger1] +Rate_Piece_Pass[Trigger1];
}

void BoardCard::Get_material_Counts() {

    for (int i=0;i<boards_counts.toInt();i++) {

        short rtn =MCF_Sorting_Set_Input_Enable_Net(0,1,boards_number[i].toUShort());

        unsigned long Trigger_Piece_Pass[8][2];
        unsigned long* Piece_Pass = &Trigger_Piece_Pass[boards_number[i].toUShort()][Trigger1];
        rtn = MCF_Sorting_Get_Piece_Pass_Net(Trigger1, Piece_Pass, boards_number[i].toUShort());
         if (rtn==0) Rate_Piece_Pass2[Trigger1] = Trigger_Piece_Pass[boards_number[i].toUShort()][Trigger1];


        unsigned long Trigger_Piece_Find[8][2];
        unsigned long Trigger_Piece_Cross_Camera[8];
        unsigned long Temp_Piece_Size[10] = {0};
        unsigned long Temp_Piece_Distance_To_next[10] = {0};

        rtn = MCF_Sorting_Get_Piece_State_Net(
        Trigger1,
        &Trigger_Piece_Find[boards_number[i].toUShort()][Trigger1],
        Temp_Piece_Size,
        Temp_Piece_Distance_To_next,
        &Trigger_Piece_Cross_Camera[Trigger1],
        boards_number[i].toUShort()
        );
        if (i==0&&rtn==0) {
            Trigger_Piece_Find1[Trigger1] = Trigger_Piece_Find[boards_number[i].toUShort()][Trigger1];
            materials[i]=Trigger_Piece_Find1[Trigger1] +Rate_Piece_Pass2[Trigger1];
        }
        if (i==1&&rtn==0) {
            Trigger_Piece_Find2[Trigger1] = Trigger_Piece_Find[boards_number[i].toUShort()][Trigger1];
            materials[i]=Trigger_Piece_Find2[Trigger1] +Rate_Piece_Pass2[Trigger1];
        }
        if (i==2&&rtn==0) {
            Trigger_Piece_Find3[Trigger1] = Trigger_Piece_Find[boards_number[i].toUShort()][Trigger1];
            materials[i]=Trigger_Piece_Find3[Trigger1] +Rate_Piece_Pass2[Trigger1];
        }

    }

}


/*void BoardCard::Get_channel_Speed() {
    for (int i = 0; i < boards_counts.toInt(); i++) {
        int currentCount = materials[i];
        QTime currentTime = QTime::currentTime();

        int elapsedMS = lastTime[i].msecsTo(currentTime);
        if (elapsedMS == 0) continue;  // 避免除以0

        double minutes = elapsedMS / 60000.0;
        int delta = currentCount - lastMaterialCount[i];

        double speed = delta / minutes;

        channelSpeed[i] = speed;

        lastMaterialCount[i] = currentCount;
        lastTime[i] = currentTime;
    }
}*/


/*void BoardCard::Get_channel_Speed() {
    for (int i = 0; i < 3; i++) {
        int currentCount = materials[i];
        QTime currentTime = QTime::currentTime();

        int elapsedMS = lastTime[i].msecsTo(currentTime);
        if (elapsedMS == 0) continue; // 避免除以0

        double minutes = elapsedMS / 5000.0;
        int delta = currentCount - lastMaterialCount[i];
        double speed = delta / minutes;

        channelSpeed[i] = static_cast<int>(calculateAverageSpeed(channelSpeedHistory[i], speed, historySize));

        lastMaterialCount[i] = currentCount;
        lastTime[i] = currentTime;
    }
}*/

void BoardCard::Get_channel_Speed() {
    for (int i = 0; i < 3; i++) {
        int currentCount = materials[i];
        QTime currentTime = QTime::currentTime();

        int elapsedMS = lastTime[i].msecsTo(currentTime);
        if (elapsedMS == 0) continue;

        double minutes = elapsedMS / 60000.0;
        int delta = currentCount - lastMaterialCount[i];
        double speed = delta / minutes;

        channelSpeed[i] = static_cast<int>(speed);

        lastMaterialCount[i] = currentCount;
        lastTime[i] = currentTime;
    }
}






short BoardCard::MCF_Screen_Get_Distance_Encoder(unsigned short StationNumber)
{
    short rtn = 0;
    unsigned long Input_Count_Fall = 0;
    unsigned long Lock_Data_Buffer[10] = {0};
    rtn = MCF_Get_Input_Fall_Count_Bit_Net(
        0,
        &Input_Count_Fall,
        Lock_Data_Buffer,
        StationNumber
    );
    if (rtn != 0) return rtn;

    // 吹气位置
    Distance = std::abs(static_cast<int>(Encoder - Lock_Data_Buffer[0]));

    return rtn;
}



int BoardCard::openSift() {
    for (int i=0;i<boards_counts.toInt();i++) {

       short ret=MCF_Sorting_Start_Net(0,boards_number[i].toUShort());
        if (ret!=0) {
            QMessageBox::critical(this, "初始化失败",
               QString("板卡 %1 开启筛选失败，错误码: %2").arg(boards_number[i]).arg(ret));
            return -1;

        }
    }
    writeLog("开启筛选成功");
    return 0;
}

int BoardCard::stopSift() {
    for (int i=0;i<boards_counts.toInt();i++) {

        short ret=MCF_Sorting_Close_Net(boards_number[i].toUShort());
        if (ret!=0) {
            QMessageBox::critical(this, "初始化失败",
               QString("板卡 %1 关闭筛选失败，错误码: %2").arg(boards_number[i]).arg(ret));
            return -1;
        }
    }
    writeLog("关闭筛选成功");
    return 0;
}

int BoardCard::stopBoard() {

    short ret=MCF_Close_Net();
    if (ret!=0) {
        QMessageBox::critical(this, "初始化失败",
           QString("关闭板卡失败，错误码: %1").arg(ret));
        return -1;
    }
    return 0;
}

void BoardCard::Get_DI0_Status() {
    unsigned short ref = 0;
    short ret=-1;
    for (int i=0;i<boards_counts.toInt();i++) {
       ret= MCF_Get_Input_Bit_Net(0,&ref, boards_number[i].toUShort());
        if (ret==0) {
            DI0Status[i]=ref;
        }
    }

}


void BoardCard::Get_DO0_Status() {
    unsigned short ref = 0;
    short ret=-1;
    for (int i=0;i<boards_counts.toInt();i++) {
        ret= MCF_Get_Input_Bit_Net(0,&ref, boards_number[i].toUShort());
        if (ret==0) {
            if (i==0)
            DI03Status[0]=ref;
            else if (i==1)
            DI03Status[3]=ref;
            else if (i==2)
            DI03Status[6]=ref;
        }
        ret= MCF_Get_Input_Bit_Net(1,&ref, boards_number[i].toUShort());
        if (ret==0) {
            if (i==0)
                DI03Status[1]=ref;
            else if (i==1)
                DI03Status[4]=ref;
            else if (i==2)
                DI03Status[7]=ref;
        }
        ret= MCF_Get_Input_Bit_Net(2,&ref, boards_number[i].toUShort());
        if (ret==0) {
            if (i==0)
                DI03Status[2]=ref;
            else if (i==1)
                DI03Status[5]=ref;
            else if (i==2)
                DI03Status[8]=ref;
        }
    }

}



QString BoardCard::extractCode(const QString& text) {
    return text.section("：", 0, 0).trimmed();
}


QString BoardCard::extractRawTimeValue(const QString& text) {
    QString desc = text.section("：", 1).trimmed();

    QRegularExpression re(R"((\d+)(?=\s*[a-zA-Z]))");
    QRegularExpressionMatch match = re.match(desc);

    if (match.hasMatch()) {
        return match.captured(1);
    }

    return "";
}


QGroupBox* BoardCard::createStatusGroup(const QString& title, int lightCount,int index) {
    QGroupBox* groupBox = new QGroupBox();
    /*groupBox->setStyleSheet(R"(
        QGroupBox {
            border: 1px solid white;
            border-radius: 2px;
        }
    )");*/
    groupBox->setStyleSheet(R"(
    QGroupBox {
        border: none; /* 无边框 */
        margin: 0px;   /* 可选，配合布局控制边距 */
        padding: 0px;  /* 可选，减少内容填充区域 */
    }
)");
    QGridLayout* mainLayout = new QGridLayout(groupBox);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    //标题
    QLabel* titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("color: white; font-size: 8px;");
    mainLayout->addWidget(titleLabel, 0, 0, Qt::AlignLeft| Qt::AlignTop);

    //水平灯光
    QHBoxLayout* lightLayout = new QHBoxLayout();
    lightLayout->setSpacing(20);
    lightLayout->setContentsMargins(0, 0, 0, 0);

    if (index==1) {
        light1 = new QLabel();
        light1->setFixedSize(10, 10);
        light1->setStyleSheet("background-color: red; border-radius: 5px;");
        lightLayout->addWidget(light1);

        light2 = new QLabel();
        light2->setFixedSize(10, 10);
        light2->setStyleSheet("background-color: red; border-radius: 5px;");
        lightLayout->addWidget(light2);

        light3 = new QLabel();
        light3->setFixedSize(10, 10);
        light3->setStyleSheet("background-color: red; border-radius: 5px;");
        lightLayout->addWidget(light3);
    }
    else if (index==2) {
        light4 = new QLabel();
        light4->setFixedSize(10, 10);
        light4->setStyleSheet("background-color: red; border-radius: 5px;");
        lightLayout->addWidget(light4);

        light5 = new QLabel();
        light5->setFixedSize(10, 10);
        light5->setStyleSheet("background-color: red; border-radius: 5px;");
        lightLayout->addWidget(light5);

        light6 = new QLabel();
        light6->setFixedSize(10, 10);
        light6->setStyleSheet("background-color: red; border-radius: 5px;");
        lightLayout->addWidget(light6);
    }
    else{
        light7 = new QLabel();
        light7->setFixedSize(10, 10);
        light7->setStyleSheet("background-color: red; border-radius: 5px;");
        lightLayout->addWidget(light7);

        light8 = new QLabel();
        light8->setFixedSize(10, 10);
        light8->setStyleSheet("background-color: red; border-radius: 5px;");
        lightLayout->addWidget(light8);

        light9 = new QLabel();
        light9->setFixedSize(10, 10);
        light9->setStyleSheet("background-color: red; border-radius: 5px;");
        lightLayout->addWidget(light9);
    }



    QWidget* lightContainer = new QWidget();
    lightContainer->setLayout(lightLayout);
    mainLayout->addWidget(lightContainer, 1, 0);
    mainLayout->setRowStretch(0, 2);
    mainLayout->setRowStretch(1, 4);
    return groupBox;
}



void BoardCard::updateLight() {
      if (DI03Status[0]==0) {
     light1->setStyleSheet("background-color: green; border-radius: 5px;");
   }
    else light1->setStyleSheet("background-color: red; border-radius: 5px;");
    if (DI03Status[1]==0) {
        light2->setStyleSheet("background-color: green; border-radius: 5px;");
    }
    else light2->setStyleSheet("background-color: red; border-radius: 5px;");
    if (DI03Status[2]==0) {
        light3->setStyleSheet("background-color: green; border-radius: 5px;");
    }
    else light3->setStyleSheet("background-color: red; border-radius: 5px;");
    if (DI03Status[3]==0) {
        light4->setStyleSheet("background-color: green; border-radius: 5px;");
    }
    else light4->setStyleSheet("background-color: red; border-radius: 5px;");
    if (DI03Status[4]==0) {
        light5->setStyleSheet("background-color: green; border-radius: 5px;");
    }
    else light5->setStyleSheet("background-color: red; border-radius: 5px;");
    if (DI03Status[5]==0) {
        light6->setStyleSheet("background-color: green; border-radius: 5px;");
    }
    else light6->setStyleSheet("background-color: red; border-radius: 5px;");
    if (DI03Status[6]==0) {
        light7->setStyleSheet("background-color: green; border-radius: 5px;");
    }
    else light7->setStyleSheet("background-color: red; border-radius: 5px;");
    if (DI03Status[7]==0) {
        light8->setStyleSheet("background-color: green; border-radius: 5px;");
    }
    else light8->setStyleSheet("background-color: red; border-radius: 5px;");
    if (DI03Status[8]==0) {
        light9->setStyleSheet("background-color: green; border-radius: 5px;");
    }
    else light9->setStyleSheet("background-color: red; border-radius: 5px;");

}


void BoardCard::GEt_Board_state() {

   short ret= MCF_Get_Link_State_Net();
    BoarState=ret;

    /*for (int i = 0; i < boards_counts.toInt(); i++) {
         materials[i]++;
    }*/
    /*if (DI0Status[0]==0)
    {
        DI0Status[0]=1;
    }else if (DI0Status[0]==1)
    {
        DI0Status[0]=0;
    }*/

}

QStringList BoardCard::Get_Trig_Blow() {
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
    //writeLogToFile(trigList.join("\n"));
    //LOG_INFO(trigList.join("\n"));
return trigList;
}



// 计算滑动窗口平均速度
double BoardCard::calculateAverageSpeed(QQueue<double>& history, double newSpeed, int maxSize) {
    history.enqueue(newSpeed);
    if (history.size() > maxSize) {
        history.dequeue();
    }

    double sum = 0.0;
    for (double s : history) sum += s;

    return sum / history.size();
}




void BoardCard::writeLogToFile(const QString &logText) {

    QString exePath = QCoreApplication::applicationDirPath();

    QString filePath = exePath + "/log01.txt";

    QFile file(filePath);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);

        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        out << "[" << time << "] " << logText << "\n";

        file.close();
    }
}



void BoardCard::loadAppConfig()
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
    ui->btnChannel_1->setText(QString("通道%1").arg(passage));
    ui->btnChannel_2->setText(QString("通道%1").arg(passage+1));
    ui->btnChannel_3->setText(QString("通道%1").arg(passage+2));
}


void BoardCard::writeLog(const QString &logText) {
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



void BoardCard::startMonitoring()
{
    // 启动 camcons 监控线程
    camThreadRunning = true;
    camThread = QThread::create([this]() { monitorCamcons(); });
    camThread->start();

    // 启动 materials 监控线程
    materialThreadRunning = true;
    materialThread = QThread::create([this]() { monitorMaterials(); });
    materialThread->start();
}

void BoardCard::stopMonitoring()
{
    // 停止 camcons 线程
    camThreadRunning = false;
    if (camThread) {
        camThread->wait();
        delete camThread;
        camThread = nullptr;
    }

    // 停止 materials 线程
    materialThreadRunning = true;
    if (materialThread) {
        materialThread->wait();
        delete materialThread;
        materialThread = nullptr;
    }
}

// 相机计数监控
void BoardCard::monitorCamcons()
{
    while (camThreadRunning) {
        {
            QMutexLocker locker(&camMutex);
            for (int i = 0; i < camcons.size(); ++i) {
                if (camcons[i] != lastCamcons[i]) {
                    emit camconChanged(i, camcons[i]);
                    lastCamcons[i] = camcons[i];
                }
            }
        }
        QThread::msleep(5);
    }
}

// 入料总数监控
void BoardCard::monitorMaterials()
{
    while (materialThreadRunning) {
        {
            QMutexLocker locker(&materialMutex);
            for (int i = 0; i < materials.size(); ++i) {
                if (materials[i] != lastMaterials[i]) {
                    emit materialChanged(i, materials[i]);
                    lastMaterials[i] = materials[i];
                }
            }
        }
        QThread::msleep(5);
    }
}