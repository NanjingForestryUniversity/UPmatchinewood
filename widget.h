#ifndef WIDGET_H
#define WIDGET_H

#define WIN32_LEAN_AND_MEAN
#include <QWidget>
#include "camera.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/types_c.h"
#include <QPixmap>
#include "correct.h"
#include <QTimer>
#include <QDateTime>
#include <winsock2.h>
#include <iostream>
#include <QVector>
#include "showpic.h"

#pragma comment(lib,"ws2_32.lib")

class QModbusClient;
class QModbusReply;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void plc_connect();

private slots:
    //button
    //Tab_1
    void btn_Tab1_1_click();
    void btn_Tab1_2_click();

    //Tab_2
    void showlabel(cv::Mat);
    void btn_Tab2_start_click();
    void btn_Tab2_stop_click();
    void btn_Tab2_mainMenu_click();
    void showlabelSorting(char*);
    void btn_Tab2_autoLearning_click();
    void btn_Tab2_light_click();
    void btn_Tab2_middle_click();
    void btn_Tab2_dark_click();
    void on_btn_Tab2_Preprocessing_clicked();
    void on_btn_Tab2_show_clicked();
    void btn_Tab2_train_click();
    void showPic(char*);
    //Tab_3
    void btn_Tab3_cameraParasettings_click();
    void btn_Tab3_mainMenu_click();
    void showimage_test(cv::Mat);
    void btn_Tab3_saveImg_click();
    void btn_Tab3_changeModel_click();
    //Tab_4
    void btn_Tab4_return_click();
    void btn_Tab4_getCorrect_click();
    void btn_Tab4_black_click();
    void btn_Tab4_white_click();
    void btn_Tab4_wbcorrect_click();
    void on_btn_Tab2_start_2_clicked();
    void on_btn_Tab2_start_3_clicked();
    void on_btn_Tab2_start_4_clicked();

    void dafei_slot();

    //Tab_5
    void btn_Tab5_skip_click();
    void skip_Notime();

    void on_btn_Tab4_exit_clicked();

    //picture
    void PictureProcess();



private:
    Ui::Widget *ui;

    Adjust_para * adjust_para;
    QPixmap pix;
    QPixmap pix_test;
    SendThread *send_thread = nullptr;
    RecvThread *recv_thread = nullptr;

    QTimer *skip;

    QTimer* dafei = new QTimer;
    void connect_signals();
    void init_window();

    RestartThread *restart_thread = nullptr;

    showpic *pic = nullptr;

signals:
    void correct_signal();
};


#endif // WIDGET_H
