#ifndef CORRECT_H
#define CORRECT_H

#include "opencv2/opencv.hpp"
#include <QDebug>

class correct
{
private:
    float correction_ratio;
    float r;
    float g;
    float b;
    float correction_ratio_r;
    float correction_ratio_g;
    float correction_ratio_b;
    int b_base = 125.0f;
    int g_base = 125.0f;
    int r_base = 125.0f;

public:
    correct();
    bool is_corrected;
    void get_rgb(cv::Mat img);
    void cal_correction_ratio();
    void correct_img(cv::Mat img);
};

#endif // CORRECT_H
