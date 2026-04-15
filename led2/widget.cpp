#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
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


void Widget::on_pushButton_2_clicked()
{
    qDebug() << "I GET RING" << endl;
}

void Widget::on_pushButton_3_clicked()
{
    qDebug() << "下游戏" << endl;
}


void Widget::on_pushButton_4_clicked()
{
    qDebug() << "直接就钻进了VIP" << endl;
}

