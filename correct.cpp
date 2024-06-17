#include "correct.h"

correct::correct()
{
    is_corrected = false;
    correction_ratio_r = 1;
    correction_ratio_g = 1;
    correction_ratio_b = 1;
}

void correct::get_rgb(cv::Mat img)
{
    int b_temp = 0;
    int g_temp = 0;
    int r_temp = 0;
    for(int i=0; i<img.rows; i++)
    {
        for(int j=0; j<img.cols; j++)
        {
            b_temp += (int)img.at<cv::Vec3b>(i, j)[0];
            g_temp += (int)img.at<cv::Vec3b>(i, j)[1];
            r_temp += (int)img.at<cv::Vec3b>(i, j)[2];
        }
    }
    b = b_temp / (img.cols * img.rows);
    g = g_temp / (img.cols * img.rows);
    r = r_temp / (img.cols * img.rows);
}

void correct::cal_correction_ratio()
{
    correction_ratio_b = b / b_base;
    correction_ratio_g = g / g_base;
    correction_ratio_r = r / r_base;
    qDebug() << "correction ratio" << correction_ratio_b << correction_ratio_g << correction_ratio_r;
}

void correct::correct_img(cv::Mat img)
{
    for(int i=0; i<img.cols; i++)
    {
        for(int j=0; j<img.rows; j++)
        {
            (img.at<cv::Vec3b>(j, i)[0] * correction_ratio_b) > 255 ? img.at<cv::Vec3b>(j, i)[0] = 255 : img.at<cv::Vec3b>(j, i)[0] *= correction_ratio_b;
            (img.at<cv::Vec3b>(j, i)[1] * correction_ratio_b) > 255 ? img.at<cv::Vec3b>(j, i)[1] = 255 : img.at<cv::Vec3b>(j, i)[1] *= correction_ratio_b;
            (img.at<cv::Vec3b>(j, i)[2] * correction_ratio_b) > 255 ? img.at<cv::Vec3b>(j, i)[2] = 255 : img.at<cv::Vec3b>(j, i)[2] *= correction_ratio_b;
        }
    }
}
