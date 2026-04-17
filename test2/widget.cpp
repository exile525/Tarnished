#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QPixmap>
#include "monitor.h"
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    ,time(new QTimer(this))
    ,m_ui(nullptr)
{
    ui->setupUi(this);
    ui->horizontalSlider->setRange(0,9999);
    connect(&event,&fsmpEvents::keyPressed,this,&Widget::button);
    //event 第一个人物，事件：按压，this:widget 触发事件：自创槽函数
    connect(time,&QTimer::timeout,this,&Widget::timout);
    time->start(1000);//1000毫秒n

    QPixmap pixmap(":/12.jpg");
    QSize labelSize = ui->label_3->size();
    QPixmap scaledPixmap = pixmap.scaled(labelSize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->label_3->setPixmap(scaledPixmap);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_clicked()
{
    if(ui->pushButton->text() == "开灯"){
        //led_on;
        led.on(fsmpLeds::LED1);
        ui->pushButton->setText("关灯");
    }else{
        led.off(fsmpLeds::LED1);
        ui->pushButton->setText("开灯");
    }
}


void Widget::on_checkBox_stateChanged(int arg1)
{
    if(arg1){
        led.on(fsmpLeds::LED3);
    }else{
        led.off(fsmpLeds::LED3);
    }
}


void Widget::on_checkBox_2_stateChanged(int arg1)
{
    if(arg1){

        beeper.stop();
    }else{
        beeper.start();
    }
}


void Widget::on_horizontalSlider_valueChanged(int value)
{
    qDebug() << ui->horizontalSlider->value();
    beeper.setRate(value);
    beeper.start();

    ui->progressBar->setValue((ui->horizontalSlider->value()+1)/100);

}


void Widget::on_checkBox_3_stateChanged(int arg1)
{
    if(arg1){
        fan.setSpeed(100);
        fan.start();
    }else{
        fan.stop();
    }
}

void Widget::button(int num){
    switch(num){
        case 1:
            led.on(fsmpLeds::LED1);
            ui->checkBox_4->toggle();
            break;
        case 2:
            led.on(fsmpLeds::LED2);
            ui->checkBox_5->toggle();
            break;
        case 3:
            led.on(fsmpLeds::LED3);
            ui->checkBox->toggle();
            break;
        default:
            break;
    }


}




void Widget::on_checkBox_4_stateChanged(int arg1)
{
    if(arg1){
        led.on(fsmpLeds::LED1);
    }else{
        led.off(fsmpLeds::LED1);
    }
}


void Widget::on_checkBox_5_stateChanged(int arg1)
{
    if(arg1){
        led.on(fsmpLeds::LED2);
    }else{
        led.off(fsmpLeds::LED2);
    }
}

void Widget::timout(){
    ui->doubleSpinBox->setValue(th.temperature());
    ui->doubleSpinBox_2->setValue(th.humidity());
}

void Widget::on_pushButton_2_clicked()
{
    //跳转界面
    if(!m_ui){
        m_ui = new Monitor(nullptr);

    }
    this->hide();
    m_ui->show();
}

