#ifndef CAMERA_H
#define CAMERA_H

#include <QDebug>
#include "SapClassBasic.h"
#include <opencv2/opencv.hpp>
#include "thread.h"
class camera
{
public:
    camera();
    bool enumDevic(char* serverName);
    bool creatObjects(char* serverName);
    void destroyObjects();
    bool openCamera();
    void closeCamera();
    bool startCapture();
    bool stopCapture();
    bool interTricapture();

    bool startCorrect();
    bool stopCorrect();

    bool capture_black_flag = false;
    bool capture_white_flag = false;
    bool ret_SCaptrue = false;
    bool ret_SCorrect = false;
    cv::Mat white_mat;
    cv::Mat black_mat;
    cv::Mat eps;
    SapTransfer *c_trans;
    float* white_buf = nullptr;
    float* black_buf = nullptr;

    SapLocation *m_loc;
    SapAcquisition *m_pAcq;
    SapBuffer *m_buffer;
    SapTransfer *m_trans;

    SapTransfer *l_trans;
    std::queue<cv::Mat> imgqueue;
    cv::Mat calibrated_img;
    int rgb_height;
private:

};

#endif // CAMERA_H
