//
// Created by Adminstrator on 25-7-31.
//

// You may need to build the project (run Qt uic code generator) to get "ui_UserWindow.h" resolved

#include "userwindow.h"
#include "ui_UserWindow.h"


UserWindow::UserWindow(QWidget *parent) :
    QWidget(parent), ui(new Ui::UserWindow) {
    ui->setupUi(this);
}

UserWindow::~UserWindow() {
    delete ui;
}
