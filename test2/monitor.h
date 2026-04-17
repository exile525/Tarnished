#ifndef MONITOR_H
#define MONITOR_H

#include <QWidget>
#include "widget.h"
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

private:
    Ui::Monitor *ui;

    Widget *main_ui;
};

#endif // MONITOR_H
