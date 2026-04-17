#include "monitor.h"
#include "ui_monitor.h"

Monitor::Monitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Monitor)
  ,main_ui(nullptr)
{
    ui->setupUi(this);
}

Monitor::~Monitor()
{
    delete ui;
}

void Monitor::on_pushButton_clicked()
{
    //跳回主界面
    this->hide();
    if(main_ui){
        main_ui->show();
    }else{
        //如果没有设置，作为一种临时方案，查找顶级窗口中的 Widget
        //这不是最好的方法，但对于当前场景可以工作
        QWidgetList topWidgets = QApplication::topLevelWidgets();
        for(QWidget *w : topWidgets){
            if (Widget *mainWidget = qobject_cast<Widget*>(w)) {
                mainWidget->show();
                break;
            }
        }
    }
}

void Monitor:: newpicture(){
        //处理图片
    }
void Monitor::timout(){

    }
