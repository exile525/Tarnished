#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <fsmpLed.h>
#include<fsmpBeeper.h>
#include <fsmpFan.h>
#include <fsmpEvents.h>
#include <fsmpTempHum.h>
#include <fsmpProximity.h>
#include <QTimer>
class Monitor;
class AiChatWidget;
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

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_checkBox_stateChanged(int arg1);

    void updateProgressBar();

    void on_checkBox_2_stateChanged(int arg1);



    void on_checkBox_3_stateChanged(int arg1);


    void pushbuton(int num);

    void timeout();
    void on_pushButton_4_clicked();

    void on_checkBox_4_stateChanged(int arg1);
    void checkProximity();

    void on_pushButton_5_clicked();

private:
    Ui::Widget *ui;
    fsmpLeds myled;
    fsmpBeeper beeper;
    fsmpFan fan;
    fsmpEvents event;
    fsmpTempHum th;
    AiChatWidget *aiWidget;
    QTimer *time;
    QTimer *proximityTimer;
    QTimer *curtainTimer;
    Monitor *m_ui;
    fsmpProximity proximity;
};
#endif // WIDGET_H
