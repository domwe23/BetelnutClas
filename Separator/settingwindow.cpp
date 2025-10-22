//
// Created by Adminstrator on 25-7-22.
//

// You may need to build the project (run Qt uic code generator) to get "ui_settingWindow.h" resolved

#include "settingwindow.h"
#include "ui_settingWindow.h"


settingWindow::settingWindow(QWidget *parent) :
    VpMainWindow(parent), ui(new Ui::settingWindow) {
    ui->setupUi(this);
    initWindow();
    connect(ui->pushButton_4, &QPushButton::clicked, this, &settingWindow::onPushButton4Clicked);

    connect(ui->pushButton, &QPushButton::clicked, this, [=]() {
        channel=1;
        loadSettingPage();
});
    connect(ui->pushButton_3, &QPushButton::clicked, this, [=]() {
        channel=2;
        loadSettingPage();
});
    connect(ui->pushButton_2, &QPushButton::clicked, this, [=]() {
        channel=3;
        loadSettingPage();
});

}

// settingWindow::settingWindow(int index) {
//
// }



settingWindow::~settingWindow() {
    delete ui;
}

void settingWindow::onPushButton4Clicked() {
    QSettings settings(QCoreApplication::applicationDirPath() + "/../Config.ini", QSettings::IniFormat);

    QString groupName = QString("channel%1").arg(channel);
    settings.beginGroup(groupName);

    //settings.setValue("station_number", station_number);
    //settings.setValue("card_type", card_type);

    settings.setValue("is_enable", is_enable);
    settings.setValue("level", level);
    settings.setValue("filtering", filtering);

    settings.setValue("camera_name", camera_name);
    settings.setValue("camera_location", camera_location);
    settings.setValue("camera_direction", camera_direction);
    settings.setValue("camera_out_tags", camera_out_tags);
    settings.setValue("camera_out_action", camera_out_action);
    settings.setValue("camera_delay", camera_delay);
    settings.setValue("camera_offset", camera_offset);
    settings.setValue("camera_light_advance", camera_light_advance);

    settings.setValue("valve_name", valve_name.join(","));
    settings.setValue("valve_location", valve_location.join(","));
    settings.setValue("valve_direction", valve_direction.join(","));
    settings.setValue("valve_out_tags", valve_out_tags.join(","));
    settings.setValue("valve_out_action", valve_out_action.join(","));
    settings.setValue("valve_offset", valve_offset.join(","));

    settings.endGroup();

}

void settingWindow::initWindow()
{
    ui->groupBox->setTitle("");
    // 左侧菜单项
    ui->listWidget->clear();
    ui->listWidget->addItem("物料设置");
    ui->listWidget->addItem("相机设置");
    ui->listWidget->addItem("气阀设置");

    // --------------------
    // page1 - 相机设置
    // --------------------
    QWidget *page1 = new QWidget();
    QGridLayout *layout1 = new QGridLayout(page1);

    Qcamera_name=new QLineEdit("相机");
    layout1->addWidget(new QLabel("名称"), 0, 0);
    layout1->addWidget(Qcamera_name, 0, 1);

    Qcamera_location=new QLineEdit("3000");
    layout1->addWidget(new QLabel("设备位置"), 1, 0);
    layout1->addWidget(Qcamera_location, 1, 1);
    layout1->addWidget(new QLabel("Plus"), 1, 2);


    layout1->addWidget(new QLabel("运动方向"), 2, 0);
    directionBox = new QComboBox();
    directionBox->addItems(QStringList() << "正方向" << "反方向");
    layout1->addWidget(directionBox, 2, 1);

    layout1->addWidget(new QLabel("输出位号"), 3, 0);
    outputPortBox = new QComboBox();
    outputPortBox->addItems(QStringList() << "D01" << "D02" << "D03"<< "D04" << "D05" << "D06"<< "D07" << "D08" << "D09"<< "D010" << "D011" << "D012"<< "D013");
    layout1->addWidget(outputPortBox, 3, 1);

    layout1->addWidget(new QLabel("输出动作"), 4, 0);
    actionBox = new QComboBox();
    actionBox->addItems(QStringList() << "0：关闭相机触发" << "1：打开相机触发");
    layout1->addWidget(actionBox, 4, 1);

    Qcamera_delay=new QLineEdit("0");
    layout1->addWidget(new QLabel("计数延时"), 5, 0);
    layout1->addWidget(Qcamera_delay, 5, 1);
    layout1->addWidget(new QLabel("MS"), 5, 2);

    layout1->addWidget(new QLabel("位置偏移"), 6, 0);
    offsetSpin = new QSpinBox();
    offsetSpin->setSuffix("%");
    layout1->addWidget(offsetSpin, 6, 1);

    Qlight_advance=new QLineEdit("0");
    layout1->addWidget(new QLabel("光源提前"), 7, 0);
    layout1->addWidget(Qlight_advance, 7, 1);
    layout1->addWidget(new QLabel("us"), 7, 2);

    // --------------------
    // page2 - 气阀设置
    // --------------------
    QWidget *page2 = new QWidget();
    QGridLayout *layout2 = new QGridLayout(page2);

    layout2->addWidget(new QLabel("名称"), 0, 0);
    ValveBox = new QComboBox();
    ValveBox->addItems(QStringList() << "气阀1" << "气阀2"<< "气阀3" << "气阀4"<< "气阀5" << "气阀6"<< "气阀7" << "气阀8"<< "气阀9" << "气阀10"<< "气阀11" << "气阀12"<< "气阀13");
    layout2->addWidget(ValveBox, 0, 1);

    Qvalve_location=new QLineEdit("9000");
    layout2->addWidget(new QLabel("位置"), 1, 0);
    layout2->addWidget(Qvalve_location, 1, 1);
    layout2->addWidget(new QLabel("Pluse"), 1, 2);

    layout2->addWidget(new QLabel("运动方向"), 2, 0);
    dirBox = new QComboBox();
    dirBox->addItems(QStringList() << "正方向" << "反方向");
    layout2->addWidget(dirBox, 2, 1);

    layout2->addWidget(new QLabel("输出位号"), 3, 0);
    outputBox = new QComboBox();
    outputBox->addItems(QStringList() << "D01" << "D02" << "D03"<< "D04" << "D05" << "D06"<< "D07" << "D08" << "D09"<< "D010" << "D011" << "D012"<< "D013");
    outputBox->setCurrentText("D08");
    layout2->addWidget(outputBox, 3, 1);

    layout2->addWidget(new QLabel("输出动作"), 4, 0);
    actionBox2 = new QComboBox();
    actionBox2->addItems(QStringList() << "0：关闭气阀触发" << "1：打开气阀触发");
    layout2->addWidget(actionBox2, 4, 1);

    layout2->addWidget(new QLabel("位置偏移"), 5, 0);
    offsetBox = new QSpinBox();
    offsetBox->setSuffix("%");
    layout2->addWidget(offsetBox, 5, 1);


    // --------------------
    // page3 - 物料设置
    // --------------------
    QWidget *page3 = new QWidget();
    QGridLayout *layout3 = new QGridLayout(page3);


    layout3->addWidget(new QLabel("启用状态"), 2, 0);
    dirBox3 = new QComboBox();
    dirBox3->addItems(QStringList() << "开启" << "关闭");
    layout3->addWidget(dirBox3, 2, 1);

    layout3->addWidget(new QLabel("触发器电平"), 3, 0);
    outputBox3 = new QComboBox();
    outputBox3->addItems(QStringList() << "低电平" << "高电平" );
    outputBox3->setCurrentText("低电平");
    layout3->addWidget(outputBox3, 3, 1);

    Qfiltering=new QLineEdit(" ");
    layout3->addWidget(new QLabel("触发器滤波"), 4, 0);;
    layout3->addWidget(Qfiltering, 4, 1);


    // 清除原有页面
    while (ui->stackedWidget->count() > 0) {
        QWidget *w = ui->stackedWidget->widget(0);
        ui->stackedWidget->removeWidget(w);
        delete w;  // 如果这些页面是 new 出来的
    }

    ui->stackedWidget->addWidget(page3);
    ui->stackedWidget->addWidget(page1);
    ui->stackedWidget->addWidget(page2);

     connect(ui->listWidget, &QListWidget::currentRowChanged,
                         ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    connect(ui->listWidget, &QListWidget::currentRowChanged, this, [=](int index) {
    loadSettingPage();
});

    //qDebug() << "连接是否成功:" << connected;

    // 默认显示第一个页面
    ui->listWidget->setCurrentRow(0);


    ui->horizontalLayout_4->setStretch(0,1);
    ui->horizontalLayout_4->setStretch(1,4);



    connect(dirBox3, &QComboBox::currentTextChanged, this, [=]() {
        is_enable=dirBox3->currentText();
    });
    connect(outputBox3, &QComboBox::currentTextChanged, this, [=]() {
        level=outputBox3->currentText();
    });
    connect(Qfiltering, &QLineEdit::editingFinished, this, [=]() {
        filtering=Qfiltering->text();
});



    //********************************************************
    connect(Qcamera_name, &QLineEdit::editingFinished, this, [=]() {
        camera_name=Qcamera_name->text();
});
    connect(Qcamera_location, &QLineEdit::editingFinished, this, [=]() {
        camera_location=Qcamera_location->text();
});
    connect(directionBox, &QComboBox::currentTextChanged, this, [=]() {
        camera_direction=directionBox->currentText();
 });
    connect(outputPortBox, &QComboBox::currentTextChanged, this, [=]() {
        camera_out_tags=outputPortBox->currentText();
 });
    connect(actionBox, &QComboBox::currentTextChanged, this, [=]() {
        camera_out_action=actionBox->currentText();
 });
    connect(Qcamera_delay, &QLineEdit::editingFinished, this, [=]() {
        camera_delay=Qcamera_delay->text();
    });
    connect(offsetSpin, &QSpinBox::editingFinished, this, [=]() {
        camera_offset=QString::number(offsetSpin->value());
    });
    connect(Qlight_advance, &QLineEdit::editingFinished, this, [=]() {
        camera_light_advance=Qlight_advance->text();
    });


    //**************************************************************************
    connect(ValveBox, &QComboBox::currentTextChanged, this, [=]() {
        //is_enable=ValveBox->currentText();
            loadSettingValve();
});
    connect(Qvalve_location	, &QLineEdit::editingFinished, this, [=]() {
        QString numStr=ValveBox->currentText().mid(2);
        valve_location[numStr.toInt()-1]=Qvalve_location->text();
      });
    connect(dirBox, &QComboBox::currentTextChanged, this, [=]() {
        QString numStr=ValveBox->currentText().mid(2);
        valve_direction[numStr.toInt()-1]=dirBox->currentText();
   });
    connect(outputBox, &QComboBox::currentTextChanged, this, [=]() {
        QString numStr=ValveBox->currentText().mid(2);
        valve_out_tags[numStr.toInt()-1]=outputBox->currentText();
});
    connect(actionBox2, &QComboBox::currentTextChanged, this, [=]() {
        QString numStr=ValveBox->currentText().mid(2);
        valve_out_action[numStr.toInt()-1]=actionBox2->currentText();
});
    connect(offsetBox, &QSpinBox::editingFinished, this, [=]() {
        QString numStr=ValveBox->currentText().mid(2);
        valve_offset[numStr.toInt()-1]=QString::number(offsetBox->value());
 });





}









void settingWindow::loadSettingPage() {

    int index = ui->listWidget->currentRow();
    if (index < 0) return;

    QSettings settings("../Config.ini", QSettings::IniFormat);
    QString section = QString("channel%1").arg(channel);

    if (index == 0) { // 物料设置页
        QString is_enable = settings.value(section + "/is_enable", "").toString();
        QString level = settings.value(section + "/level", "").toString();
        QString filtering = settings.value(section + "/filtering", "").toString();

        dirBox3->setCurrentText(is_enable);
        outputBox3->setCurrentText(level);
        Qfiltering->setText(filtering);

    } else if (index == 1) { // 相机设置页
        QString name = settings.value(section + "/camera_name").toString();
        QString location = settings.value(section + "/camera_location").toString();
        QString direction = settings.value(section + "/camera_direction").toString();
        QString tags = settings.value(section + "/camera_out_tags").toString();
        QString action = settings.value(section + "/camera_out_action").toString();
        QString delay = settings.value(section + "/camera_delay").toString();
        QString offset = settings.value(section + "/camera_offset").toString();
        QString light = settings.value(section + "/camera_light_advance").toString();

        Qcamera_name->setText(name);
        Qcamera_location->setText(location);
        directionBox->setCurrentText(direction);
        outputPortBox->setCurrentText(tags);
        actionBox->setCurrentText(action);
        Qcamera_delay->setText(delay);
        offsetSpin->setValue(offset.toInt());
        Qlight_advance->setText(light);

    } else if (index == 2) { // 气阀设置页
        QStringList valve_location = settings.value(section + "/valve_location").toString().split(",");
        QStringList valve_direction = settings.value(section + "/valve_direction").toString().split(",");
        QStringList valve_out_tags = settings.value(section + "/valve_out_tags").toString().split(",");
        QStringList valve_out_action = settings.value(section + "/valve_out_action").toString().split(",");
        QStringList valve_offset = settings.value(section + "/valve_offset").toString().split(",");

        int valve_index = ValveBox->currentIndex(); // 当前选中气阀

        Qvalve_location->setText(valve_location.value(valve_index, ""));
        dirBox->setCurrentText(valve_direction.value(valve_index, ""));
        outputBox->setCurrentText(valve_out_tags.value(valve_index, ""));
        actionBox2->setCurrentText(valve_out_action.value(valve_index, ""));
        offsetBox->setValue(valve_offset.value(valve_index, "").toInt());
    }

    ui->stackedWidget->setCurrentIndex(index);
}



void settingWindow::loadSettingValve() {
    QSettings settings("../Config.ini", QSettings::IniFormat);
    QString section = QString("channel%1").arg(channel);

    QStringList valve_location = settings.value(section + "/valve_location").toString().split(",");
    QStringList valve_direction = settings.value(section + "/valve_direction").toString().split(",");
    QStringList valve_out_tags = settings.value(section + "/valve_out_tags").toString().split(",");
    QStringList valve_out_action = settings.value(section + "/valve_out_action").toString().split(",");
    QStringList valve_offset = settings.value(section + "/valve_offset").toString().split(",");

    int valve_index = ValveBox->currentIndex(); // 当前选中气阀

    Qvalve_location->setText(valve_location.value(valve_index, ""));
    dirBox->setCurrentText(valve_direction.value(valve_index, ""));
    outputBox->setCurrentText(valve_out_tags.value(valve_index, ""));
    actionBox2->setCurrentText(valve_out_action.value(valve_index, ""));
    offsetBox->setValue(valve_offset.value(valve_index, "").toInt());
}
