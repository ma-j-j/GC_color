#ifndef __XUAN_COLOR_H
#define __XUAN_COLOR_H
#include <opencv2/opencv.hpp>

#define COLOR_THRESHOLD_DEBUG (1)
extern int detect_color;
extern int detect_centerX;
extern int detect_centerY;
class color
{
private:
    cv::Point center;
    cv::Mat grayimage;
    cv::Mat out_image;
    cv::Rect r;
public:
    void color_recognite(cv::Mat image, cv::Scalar High, cv::Scalar Low);
    cv::Point show_center();
    void show_grayimage(char const *WINDOWNAME);
    void show_image(char const *WINDOWNAME);
    cv::Rect getRect() const{return r;}
    void color_detect();
    void color_ring(cv::Mat in_image);
    void ma_color();
};

#endif
