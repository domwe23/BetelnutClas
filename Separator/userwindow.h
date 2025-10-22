//
// Created by Adminstrator on 25-7-31.
//

#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class UserWindow; }
QT_END_NAMESPACE

class UserWindow : public QWidget {
Q_OBJECT

public:
    explicit UserWindow(QWidget *parent = nullptr);
    ~UserWindow() override;

private:
    Ui::UserWindow *ui;
};


#endif //USERWINDOW_H
