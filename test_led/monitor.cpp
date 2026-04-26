#include "monitor.h"
#include "ui_monitor.h"

Monitor::Monitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Monitor)
  , main_ui(nullptr)
  , time(new QTimer)
{
    ui->setupUi(this);
    //初始化usb摄像头
    camera = new fsmpCamera("/dev/video1", 640, 480,this);
    //摄像头准备完毕
    connect(camera,&fsmpCamera::pixReady,this,&Monitor::open_camera);
    camera->start(); 
    //定时器

    connect(time,&QTimer::timeout,this,&Monitor::timeout);
    time->start(1000);
}

Monitor::~Monitor()
{
    delete ui;
}

void Monitor::on_pushButton_clicked()
{
    {
        
        this->hide();
        if(main_ui){
            main_ui->show();
        }else{
           
            QWidgetList topWidgets = QApplication::topLevelWidgets();
            for(QWidget *w : topWidgets){
                if (Widget *mainWidget = qobject_cast<Widget*>(w)) {
                    mainWidget->show();
                    break;
                }
            }
        }
    }


}

void Monitor:: open_camera(const QImage &pix){
      
    ui->label->setPixmap(QPixmap::fromImage(pix));
    ui->label->setPixmap(QPixmap::fromImage(pix).scaled(ui->label->size(),Qt::KeepAspectRatio,Qt::FastTransformation));

    }
void Monitor::timeout(){

    }
