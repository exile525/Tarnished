#ifndef MONITOR_H
#define MONITOR_H

#include  <fsmpCamera.h>

#include <QWidget>
#include <widget.h>
#include <QTimer>
class Widget;
namespace Ui {
class Monitor;
}

class Monitor : public QWidget
{
    Q_OBJECT

public:
    explicit Monitor(QWidget *parent = nullptr);
    ~Monitor();

private slots:
    void on_pushButton_clicked();

    void open_camera(const QImage &pix);

    void timeout();
private:
    Ui::Monitor *ui;
    Widget *main_ui;

    QTimer *time;

    fsmpCamera *camera;
};

#endif // MONITOR_H
