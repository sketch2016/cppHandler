#ifndef HANDLER_H
#define HANDLER_H

#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>

class Handler;

class Message {
public:
    Message(Handler *target = nullptr, int what = -1);
    ~Message();

public:
    int what;
    long when;
    Message *next;
    Handler *target;
};

class MessageQueue {

public:
    MessageQueue();
    ~MessageQueue();

    void enqueueMessage(Message *msg);
    Message* next();

    QString toString();

private:
    Message *messages;
    QMutex mutex;
    QWaitCondition mWaitCond;

    bool shouldWakeByNewMsg;
    bool isWaiting;
};

class Looper {
public:
    Looper();
    ~Looper();

    void loop();

    MessageQueue *messageQueue;
};

class Handler
{
public:

    Handler();
    ~Handler();

    void sendMessage(Message *msg);

    void sendMessageDelayed(Message *msg, long delay);

    virtual void handleMessage(Message *msg) = 0;

    Looper* getLooper();

private:
    void sendMessageAtTime(Message *msg, long time);

    Looper *looper;
    MessageQueue *messageQueue;
};

class HandlerThread : public QThread {
public:
    HandlerThread(Handler *handler);
    ~HandlerThread();

    void run();

    Handler *handler;
};

#endif // HANDLER_H
