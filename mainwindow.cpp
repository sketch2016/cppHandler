#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#define MSG_ONE 1
#define MSG_TWO 2
#define MSG_THREE 3

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    handler = new MyHandler();
    handlerThread = new HandlerThread(handler);
    handlerThread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn1_clicked()
{
    Message *message = new Message(handler, MSG_ONE);
    handler->sendMessageDelayed(message, 1000);
}

void MainWindow::on_btn2_clicked()
{
    Message *message = new Message(handler, MSG_TWO);
    handler->sendMessageDelayed(message, 3000);
}

void MainWindow::on_btn3_clicked()
{
    Message *message = new Message(handler, MSG_THREE);
    handler->sendMessageDelayed(message, 5000);
}

MyHandler::MyHandler() {

}

MyHandler::~MyHandler() {

}

void MyHandler::handleMessage(Message *msg) {
    qDebug()<<"MyHandler::handleMessage msg.what="<<QString::number(msg->what) << " in thread " << QThread::currentThreadId();
}
