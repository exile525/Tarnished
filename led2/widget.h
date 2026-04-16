#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <fsmpLed.h>
#include <fsmpBeeper.h>
#include <fsmpFan.h>
#include <fsmpEvents.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_checkBox_2_stateChanged(int arg1);

    void on_horizontalSlider_valueChanged(int value);

    void on_checkBox_3_stateChanged(int arg1);

    void button(int num);//自创槽函数（按钮）
    void on_checkBox_4_stateChanged(int arg1);

    void on_checkBox_5_stateChanged(int arg1);

private:
    Ui::Widget *ui;
    fsmpLeds led;
    fsmpBeeper beeper;
    fsmpFan fan;
    fsmpEvents event;








};
#endif // WIDGET_H
