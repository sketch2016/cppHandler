#include "handler.h"

#include <QDateTime>
#include <QDebug>

Handler::Handler()
{
    looper = new Looper();
    this->messageQueue = looper->messageQueue;
}

Handler::~Handler()
{
    if (messageQueue != nullptr) {
        delete messageQueue;
        messageQueue = nullptr;
    }
}

void Handler::sendMessage(Message *msg) {
    sendMessageDelayed(msg, 0);
}

void Handler::sendMessageDelayed(Message *msg, long delay /*ms*/) {
    long when = QDateTime::currentMSecsSinceEpoch();
    long time = when + delay;

    sendMessageAtTime(msg, time);
}

void Handler::sendMessageAtTime(Message *msg, long time) {
    qDebug()<<"sendMessageAtTime: msg.what="<<QString::number(msg->what)<<" msg.when="<<time<< " in thread " << QThread::currentThreadId();;
    msg->when = time;
    if (msg->target != nullptr) {
        msg->target = this;
    }

    if (messageQueue != nullptr) {
        messageQueue->enqueueMessage(msg);
    }
}

void Handler::handleMessage(Message *msg) {
    long now = QDateTime::currentMSecsSinceEpoch();
    qDebug()<<"handleMessage: "<<"msg: what="<<QString::number(msg->what)<< " now="<<QString::number(now);
}

Looper* Handler::getLooper() {
    return looper;
}

Message::Message(Handler *target, int what) {
    this->what = what;
    when = 0;
    next = nullptr;

    this->target = target;
}

Message::~Message() {
    if (next != nullptr) {
        delete next;
        next = nullptr;
    }
}

MessageQueue::MessageQueue() {
    messages = nullptr;
    shouldWakeByNewMsg = false;
    isWaiting = false;
}

MessageQueue::~MessageQueue() {
    if (messages != nullptr) {
        delete messages;
        messages = nullptr;
    }
}

void MessageQueue::enqueueMessage(Message *msg) {
    mutex.lock();

    if (messages == nullptr) {
        messages = msg;
    } else {
        Message *toInset = messages;
        Message *pre = nullptr;

        while (toInset != nullptr && msg->when >= toInset->when) {
            pre = toInset;
            toInset = toInset->next;
        }

        if (pre != nullptr) {
            pre->next = msg;
        }
        msg->next = toInset;
        if (toInset == messages) {
            messages = msg;
        }
    }

    if (isWaiting) {
        shouldWakeByNewMsg = true;
        //qDebug()<<"wakeByNewMsg = true";
    }

    //qDebug()<<"enqueueMessage:msg.what = "<<QString::number(msg->what);
    //qDebug()<<"all messages:" << toString();

    mWaitCond.wakeAll();
    mutex.unlock();
}

Message* MessageQueue::next() {
    mutex.lock();

    Message *msg;
    if (messages == nullptr) {
        mWaitCond.wait(&mutex);
    }
    long now = QDateTime::currentMSecsSinceEpoch();
    if (messages->when > now) {
        //wait messages->when - now to take message
        //qDebug()<<"waiting to take message";
        isWaiting = true;
        mWaitCond.wait(&mutex, messages->when - now);
        isWaiting = false;

        //interrupted by new message, so retake message
        if (shouldWakeByNewMsg) {
            //qDebug()<<"wakeByNewMsg wake mWaitCond";
            shouldWakeByNewMsg = false;
            mutex.unlock();
            return next();
        }
    }

    //qDebug()<<"all messages:" << toString();

    msg = messages;
    messages = msg->next;
    msg->next = nullptr;

    mutex.unlock();

    return msg;
}

QString MessageQueue::toString() {
    QString result;
    if (messages == nullptr) {
        result = "no messages";
    }

    Message *msg = messages;
    while (msg != nullptr) {
        QString msgInfo = "msg:{what = " + QString::number(msg->what) + ", when = " + QString::number(msg->when) + "}";
        result.append(msgInfo).append("\n");

        msg = msg->next;
    }

    return result;
}

Looper::Looper() {
    messageQueue = new MessageQueue();
}

Looper::~Looper() {
    if (messageQueue != nullptr) {
        delete messageQueue;
        messageQueue = nullptr;
    }
}

void Looper::loop() {
    while (true) {
        Message *msg = messageQueue->next();

        if (msg->target != nullptr) {
            msg->target->handleMessage(msg);
        }
    }
}

HandlerThread::HandlerThread(Handler *handler) {
    this->handler = handler;
}

HandlerThread::~HandlerThread() {

}

void HandlerThread::run() {
    handler->getLooper()->loop();
}
