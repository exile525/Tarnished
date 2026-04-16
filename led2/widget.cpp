#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->horizontalSlider->setRange(0,9999);
    connect(&event,&fsmpEvents::keyPressed,this,&Widget::button);
    //event 第一个人物，事件：按压，this,触发事件：自创槽函数

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

