#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include "opencv2/opencv.hpp"
#include <QDebug>
#include "SapClassBasic.h"
#include "camera.h"
#include <QMutex>
#include <QSemaphore>
#include "correct.h"
#include <QTcpServer>
#include <QTcpSocket>

//#define REALWIDTH 500
//#define READHEIGHT real_height
//#define HEIGHT    500

class QModbusClient;
class QModbusReply;

static int count;

void onImageDataCallback(SapXferCallbackInfo *pInfo);
void onImageCorrectCallback(SapXferCallbackInfo *pInfo);

class Process_img : public QThread
{
    Q_OBJECT

private:
    QMutex stop_mutex;
    bool m_stop;

protected:
    void run();

public:
    explicit Process_img(QObject *parent = nullptr);
    void exitThread();

signals:
    void show_img(cv::Mat);

public slots:
};



class Adjust_para: public QThread
{
    Q_OBJECT

private:
    QMutex stop_mutex;
    bool is_stop;
    bool correct_flag;
    QMutex correct_mutex;

protected:
    void run();

public:
    explicit Adjust_para(QObject *parent = nullptr);
    void exitThread();

signals:
    void send_image_debug(cv::Mat);


public slots:
    void get_correct_signal();
};



class SendThread: public QThread
{
    Q_OBJECT

protected:
    void run();

public:
    explicit SendThread(QObject *parent = NULL);
    QTcpServer *send_server;
    QTcpSocket *send_socket;
};


class RecvThread: public QThread
{
    Q_OBJECT

protected:
    void run();

public:
    explicit RecvThread(QObject *parent = NULL);
    QTcpServer *recv_server;
    QTcpSocket *recv_socket;

public slots:

signals:
    void send_sortingResult(char*);
    void SendPicInfo(char*);
//    void read_plc();
};

class RestartThread: public QThread
{
    Q_OBJECT

private:
    QMutex restr_mutex;
    bool restr_flag;

protected:
    void run();

public:
    explicit RestartThread(QObject *parent = NULL);
    void restr_fun();

};


#endif // THREAD_H
