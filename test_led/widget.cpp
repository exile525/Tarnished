#include "widget.h"
#include "ui_widget.h"
#include "aichat.h"
#include "monitor.h"
#include <QDebug>
#include <QPixmap>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , time(new QTimer(this))
    , proximityTimer(new QTimer(this))
    , curtainTimer(new QTimer(this))
    , m_ui(nullptr)
    , aiWidget(nullptr)
{
    ui->setupUi(this);

    QPixmap pixmap(":/tu/222.jpg");
    QSize labelSize = ui->label->size();
    QPixmap scaledPixmap = pixmap.scaled(labelSize, Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->label->setPixmap(scaledPixmap);

    connect(&event,&fsmpEvents::keyPressed,this,&Widget::pushbuton);
    connect(time, &QTimer::timeout, this, &Widget::timeout);
    time->start(1000);

    connect(proximityTimer, &QTimer::timeout, this, &Widget::checkProximity);
    connect(curtainTimer, &QTimer::timeout, this, &Widget::updateProgressBar);

    
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);

    connect(time, &QTimer::timeout,
            this, &Widget::updateProgressBar);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_clicked()
{
    if(ui->pushButton->text()=="开灯"){
        myled.on(fsmpLeds::LED1);
        ui->pushButton->setText("关灯");
    }else{
        myled.off(fsmpLeds::LED1);
        ui->pushButton->setText("开灯");
    }
}


void Widget::on_pushButton_2_clicked()
{
    if(ui->pushButton_2->text()=="开灯"){
        myled.on(fsmpLeds::LED2);
        ui->pushButton_2->setText("关灯");
    }else{
        myled.off(fsmpLeds::LED2);
        ui->pushButton_2->setText("开灯");
    }
}


void Widget::on_pushButton_3_clicked()
{
    if(ui->pushButton_3->text()=="开灯"){
        myled.on(fsmpLeds::LED3);
        ui->pushButton_3->setText("关灯");
    }else{
        myled.off(fsmpLeds::LED3);
        ui->pushButton_3->setText("开灯");
    }

}


void Widget::on_checkBox_stateChanged(int arg1)
{

    if(arg1){
        ui->progressBar->setValue(0);   
        curtainTimer->start(1000);
    }else{
        curtainTimer->stop();                  
        ui->progressBar->setValue(0);
    }
}


void Widget::on_checkBox_2_stateChanged(int arg1)
{
    if (arg1){
        beeper.setRate(5000);
        beeper.start();
    }else{
        beeper.stop();
    }
}





void Widget::on_checkBox_3_stateChanged(int arg1)
{

    if (arg1){
        fan.setSpeed(100);
        fan.start();
    }else{
        fan.stop();
    }
}



void Widget::pushbuton(int num){
    static bool led1_flag = false;
    static bool led2_flag = false;
    static bool led3_flag = false;

    switch (num) {
        case 1:
            if(led1_flag){
                myled.off(fsmpLeds::LED1);
            }else{
                myled.on(fsmpLeds::LED1);
            }
            led1_flag = !led1_flag;
        break;

        case 2:
            if(led2_flag){
                myled.off(fsmpLeds::LED2);
            }else{
                myled.on(fsmpLeds::LED2);
            }
            led2_flag = !led2_flag;
        break;

        case 3:
            if(led3_flag){
                myled.off(fsmpLeds::LED3);
            }else{
                myled.on(fsmpLeds::LED3);
            }
            led3_flag = !led3_flag;
        break;

        default:
        break;
    }
}

void Widget::timeout(){

    ui->hum->setValue(th.humidity());
    ui->tem->setValue(th.temperature());
}






void Widget::on_pushButton_4_clicked()
{
    //跳转界面
      if(!m_ui){
          m_ui = new Monitor(nullptr);

      }
      this->hide();
      m_ui->show();
}


//窗帘进度条
void Widget::updateProgressBar()
{
    if (!curtainTimer->isActive())
        return;

    int value = ui->progressBar->value();
    if (value < 100) {
        value += 10;
        ui->progressBar->setValue(value);
    } else {
        curtainTimer->stop();
    }
}
void Widget::on_checkBox_4_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {  // 勾选时启动红外检测
        proximityTimer->start(500);  // 每500毫秒检测一次
        ui->label_9->setText("检测中...");
    } else {  // 取消勾选时停止检测
        proximityTimer->stop();
        ui->label_9->setText("————");
    }
}
void Widget::checkProximity()
{
    int proximityValue = proximity.getValue();

    // 根据传感器数据判断是否有人
    // 接近值越大表示越接近，可以根据实际测试调整阈值
    if (proximityValue > 500) {  // 阈值可以根据实际测试调整
        ui->label_9->setText("有人");
    } else {
        ui->label_9->setText("无人");
    }

}

void Widget::on_pushButton_5_clicked()
{
    if (!aiWidget) {
        aiWidget = new AiChatWidget(nullptr);
        aiWidget->setAttribute(Qt::WA_DeleteOnClose);
        connect(aiWidget, &QObject::destroyed, this, [this]() {
            qDebug() << "AI window destroyed, restoring main window";
            aiWidget = nullptr;
            // 确保主窗口可见并提升到最前
            this->showNormal();      // 如果是最大化或最小化状态，恢复正常
            this->raise();
            this->activateWindow();
            // 强制刷新
            this->repaint();
        });
    }
    this->hide();
    aiWidget->show();
    aiWidget->raise();
    aiWidget->activateWindow();
}
