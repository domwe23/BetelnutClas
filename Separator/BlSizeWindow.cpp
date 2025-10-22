//
// Created by adminstrator on 25-9-12.
//

// You may need to build the project (run Qt uic code generator) to get "ui_BlSizeWindow.h" resolved

#include "BlSizeWindow.h"
#include "ui_BlSizeWindow.h"


BlSizeWindow::BlSizeWindow(QWidget *parent) :
    VpMainWindow(parent), ui(new Ui::BlSizeWindow) {
    ui->setupUi(this);

    this->setFixedSize(685, 907);

    connect(ui->pushButton, &QPushButton::clicked, [=]() {
        saveConfig();
        camRatioCheck(0,camRatio1.toDouble());
        camRatioCheck(1,camRatio2.toDouble());
        camRatioCheck(2,camRatio3.toDouble());
    });
    loadConfig();
    initConnect();
    this->setWindowTitle("产品设置");


}

BlSizeWindow::~BlSizeWindow() {
    delete ui;
}


void BlSizeWindow::initConnect()
{
    connect(ui->lineEdit, &QLineEdit::textChanged, this, [=]() {
           A_W[0]=ui->lineEdit->text();
       });
    connect(ui->lineEdit_22, &QLineEdit::editingFinished, this, [=]() {
           A_W[1]=ui->lineEdit_22->text();
       });
    connect(ui->lineEdit_2, &QLineEdit::editingFinished, this, [=]() {
           A_H[0]=ui->lineEdit_2->text();
       });
    connect(ui->lineEdit_23, &QLineEdit::editingFinished, this, [=]() {
          A_H[1]=ui->lineEdit_23->text();
      });

    connect(ui->lineEdit_3, &QLineEdit::editingFinished, this, [=]() {
           B_W[0]=ui->lineEdit_3->text();
       });
    connect(ui->lineEdit_24, &QLineEdit::editingFinished, this, [=]() {
           B_W[1]=ui->lineEdit_4->text();
       });
    connect(ui->lineEdit_4, &QLineEdit::editingFinished, this, [=]() {
          B_H[0]=ui->lineEdit_4->text();
      });
    connect(ui->lineEdit_25, &QLineEdit::editingFinished, this, [=]() {
           B_H[1]=ui->lineEdit_25->text();
       });


    connect(ui->lineEdit_6, &QLineEdit::editingFinished, this, [=]() {
           C_W[0]=ui->lineEdit_6->text();
       });
    connect(ui->lineEdit_26, &QLineEdit::editingFinished, this, [=]() {
           C_W[1]=ui->lineEdit_26->text();
       });
    connect(ui->lineEdit_5, &QLineEdit::editingFinished, this, [=]() {
           C_H[0]=ui->lineEdit_5->text();
       });
    connect(ui->lineEdit_27, &QLineEdit::editingFinished, this, [=]() {
           C_H[1]=ui->lineEdit_27->text();
       });


    connect(ui->lineEdit_8, &QLineEdit::editingFinished, this, [=]() {
           D_W[0]=ui->lineEdit_8->text();
       });
    connect(ui->lineEdit_28, &QLineEdit::editingFinished, this, [=]() {
           D_W[1]=ui->lineEdit_28->text();
       });
    connect(ui->lineEdit_7, &QLineEdit::editingFinished, this, [=]() {
          D_H[0]=ui->lineEdit_7->text();
      });
    connect(ui->lineEdit_29, &QLineEdit::editingFinished, this, [=]() {
           D_H[1]=ui->lineEdit_29->text();
       });


    connect(ui->lineEdit_10, &QLineEdit::editingFinished, this, [=]() {
           E1_W[0]=ui->lineEdit_10->text();
       });
    connect(ui->lineEdit_30, &QLineEdit::editingFinished, this, [=]() {
           E1_W[1]=ui->lineEdit_30->text();
       });
    connect(ui->lineEdit_9, &QLineEdit::editingFinished, this, [=]() {
         E1_H[0]=ui->lineEdit_9->text();
     });
    connect(ui->lineEdit_31, &QLineEdit::editingFinished, this, [=]() {
           E1_H[1]=ui->lineEdit_31->text();
       });


    connect(ui->lineEdit_12, &QLineEdit::editingFinished, this, [=]() {
           E2_W[0]=ui->lineEdit_12->text();
       });
    connect(ui->lineEdit_32, &QLineEdit::editingFinished, this, [=]() {
           E2_W[1]=ui->lineEdit_32->text();
       });
    connect(ui->lineEdit_11, &QLineEdit::editingFinished, this, [=]() {
          E2_H[0]=ui->lineEdit_11->text();
      });
    connect(ui->lineEdit_33, &QLineEdit::editingFinished, this, [=]() {
           E2_H[1]=ui->lineEdit_33->text();
       });



    connect(ui->lineEdit_14, &QLineEdit::editingFinished, this, [=]() {
           E3_W[0]=ui->lineEdit_14->text();
       });
    connect(ui->lineEdit_36, &QLineEdit::editingFinished, this, [=]() {
           E3_W[1]=ui->lineEdit_36->text();
       });
    connect(ui->lineEdit_13, &QLineEdit::editingFinished, this, [=]() {
               E3_H[0]=ui->lineEdit_13->text();
           });
    connect(ui->lineEdit_37, &QLineEdit::editingFinished, this, [=]() {
           E3_H[1]=ui->lineEdit_37->text();
       });


    connect(ui->lineEdit_16, &QLineEdit::editingFinished, this, [=]() {
           E4_W[0]=ui->lineEdit_16->text();
       });
    connect(ui->lineEdit_38, &QLineEdit::editingFinished, this, [=]() {
           E4_W[1]=ui->lineEdit_38->text();
       });
    connect(ui->lineEdit_15, &QLineEdit::editingFinished, this, [=]() {
           E4_H[0]=ui->lineEdit_15->text();
       });
    connect(ui->lineEdit_39, &QLineEdit::editingFinished, this, [=]() {
           E4_H[1]=ui->lineEdit_39->text();
       });



    connect(ui->lineEdit_18, &QLineEdit::editingFinished, this, [=]() {
           E5_W[0]=ui->lineEdit_18->text();
       });
    connect(ui->lineEdit_34, &QLineEdit::editingFinished, this, [=]() {
           E5_W[1]=ui->lineEdit_34 ->text();
       });
    connect(ui->lineEdit_17, &QLineEdit::editingFinished, this, [=]() {
           E5_H[0]=ui->lineEdit_17->text();
       });
    connect(ui->lineEdit_35, &QLineEdit::editingFinished, this, [=]() {
           E5_H[1]=ui->lineEdit_35 ->text();
       });

    connect(ui->lineEdit_20, &QLineEdit::editingFinished, this, [=]() {
         camRatio1=ui->lineEdit_20->text();
     });
    connect(ui->lineEdit_21, &QLineEdit::editingFinished, this, [=]() {
        camRatio2=ui->lineEdit_21->text();
    });
    connect(ui->lineEdit_19, &QLineEdit::editingFinished, this, [=]() {
        camRatio3=ui->lineEdit_19 ->text();
       });
}



void BlSizeWindow::saveConfig() {
    QSettings settings(pathFile, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    LOG_INFO(A_W[0]);
    settings.beginGroup("blsize");

    settings.setValue("A_W", A_W.join(","));
    settings.setValue("A_H", A_H.join(","));
    settings.setValue("B_W", B_W.join(","));
    settings.setValue("B_H", B_H.join(","));
    settings.setValue("C_W", C_W.join(","));
    settings.setValue("C_H", C_H.join(","));
    settings.setValue("D_W", D_W.join(","));
    settings.setValue("D_H", D_H.join(","));
    settings.setValue("E1_W", E1_W.join(","));
    settings.setValue("E1_H", E1_H.join(","));
    settings.setValue("E2_W", E2_W.join(","));
    settings.setValue("E2_H", E2_H.join(","));
    settings.setValue("E3_W", E3_W.join(","));
    settings.setValue("E3_H", E3_H.join(","));
    settings.setValue("E4_W", E4_W.join(","));
    settings.setValue("E4_H", E4_H.join(","));
    settings.setValue("E5_W", E5_W.join(","));
    settings.setValue("E5_H", E5_H.join(","));
    settings.setValue("camRatio1", camRatio1);
    settings.setValue("camRatio2", camRatio2);
    settings.setValue("camRatio3", camRatio3);

    settings.endGroup();
    settings.sync();
}




void BlSizeWindow::loadConfig() {
    QSettings settings(pathFile, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");

    settings.beginGroup("blsize");

    // 使用 split(",") 读取成 QStringList
    A_W = settings.value("A_W", "0,0").toString().split(",");
    A_H = settings.value("A_H", "0,0").toString().split(",");
    B_W = settings.value("B_W", "0,0").toString().split(",");
    B_H = settings.value("B_H", "0,0").toString().split(",");
    C_W = settings.value("C_W", "0,0").toString().split(",");
    C_H = settings.value("C_H", "0,0").toString().split(",");
    D_W = settings.value("D_W", "0,0").toString().split(",");
    D_H = settings.value("D_H", "0,0").toString().split(",");
    E1_W = settings.value("E1_W", "0,0").toString().split(",");
    E1_H = settings.value("E1_H", "0,0").toString().split(",");
    E2_W = settings.value("E2_W", "0,0").toString().split(",");
    E2_H = settings.value("E2_H", "0,0").toString().split(",");
    E3_W = settings.value("E3_W", "0,0").toString().split(",");
    E3_H = settings.value("E3_H", "0,0").toString().split(",");
    E4_W = settings.value("E4_W", "0,0").toString().split(",");
    E4_H = settings.value("E4_H", "0,0").toString().split(",");
    E5_W = settings.value("E5_W", "0,0").toString().split(",");
    E5_H = settings.value("E5_H", "0,0").toString().split(",");
    camRatio1 = settings.value("camRatio1", camRatio1).toString();
    camRatio2 = settings.value("camRatio2", camRatio2).toString();
    camRatio3 = settings.value("camRatio3", camRatio3).toString();

    settings.endGroup();

    updateUiFromVars();
}





void BlSizeWindow::updateUiFromVars()
{
    // ---- A ----
    ui->lineEdit->setText(A_W[0]);
    ui->lineEdit_22->setText(A_W[1]);
    ui->lineEdit_2->setText(A_H[0]);
    ui->lineEdit_23->setText(A_H[1]);

    // ---- B ----
    ui->lineEdit_3->setText(B_W[0]);
    ui->lineEdit_24->setText(B_W[1]);
    ui->lineEdit_4->setText(B_H[0]);
    ui->lineEdit_25->setText(B_H[1]);

    // ---- C ----
    ui->lineEdit_6->setText(C_W[0]);
    ui->lineEdit_26->setText(C_W[1]);
    ui->lineEdit_5->setText(C_H[0]);
    ui->lineEdit_27->setText(C_H[1]);

    // ---- D ----
    ui->lineEdit_8->setText(D_W[0]);
    ui->lineEdit_28->setText(D_W[1]);
    ui->lineEdit_7->setText(D_H[0]);
    ui->lineEdit_29->setText(D_H[1]);

    // ---- E1 ----
    ui->lineEdit_10->setText(E1_W[0]);
    ui->lineEdit_30->setText(E1_W[1]);
    ui->lineEdit_9->setText(E1_H[0]);
    ui->lineEdit_31->setText(E1_H[1]);

    // ---- E2 ----
    ui->lineEdit_12->setText(E2_W[0]);
    ui->lineEdit_32->setText(E2_W[1]);
    ui->lineEdit_11->setText(E2_H[0]);
    ui->lineEdit_33->setText(E2_H[1]);

    // ---- E3 ----
    ui->lineEdit_14->setText(E3_W[0]);
    ui->lineEdit_36->setText(E3_W[1]);
    ui->lineEdit_13->setText(E3_H[0]);
    ui->lineEdit_37->setText(E3_H[1]);

    // ---- E4 ----
    ui->lineEdit_16->setText(E4_W[0]);
    ui->lineEdit_38->setText(E4_W[1]);
    ui->lineEdit_15->setText(E4_H[0]);
    ui->lineEdit_39->setText(E4_H[1]);

    // ---- E5 ----
    ui->lineEdit_18->setText(E5_W[0]);
    ui->lineEdit_34->setText(E5_W[1]);
    ui->lineEdit_17->setText(E5_H[0]);
    ui->lineEdit_35->setText(E5_H[1]);

    // ---- camRatio ----
    ui->lineEdit_20->setText(camRatio1);
    ui->lineEdit_21->setText(camRatio2);
    ui->lineEdit_19->setText(camRatio3);
}





