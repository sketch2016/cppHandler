#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "handler.h"

namespace Ui {
class MainWindow;
}

class MyHandler : public Handler {
public:
    MyHandler();
    ~MyHandler();
    void handleMessage(Message *msg);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btn1_clicked();

    void on_btn2_clicked();

    void on_btn3_clicked();

private:
    Ui::MainWindow *ui;

    Looper *looper;
    MyHandler *handler;
    HandlerThread *handlerThread;
};

#endif // MAINWINDOW_H
