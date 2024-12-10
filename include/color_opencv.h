#ifndef __XUAN_COLOR_OPENCV_H
#define __XUAN_COLOR_OPENCV_H

#include <opencv2/opencv.hpp>

class color_opencv
{
public:
    /* data */
    cv::VideoCapture cap;
    cv::Mat imgOriginal, imgHSV, imgBGR, imgClone;
    std::vector<cv::Mat> hsvSplit;   //创建向量容器，存放HSV的三通道数据
public:
    color_opencv(/* args */);
    color_opencv(int dev);
    bool get_cap(int dev);
    bool get_imgOriginal();
    void imgOriginal_Clone();
    void get_();
    void color_detect();
    ~color_opencv();
};

#endif