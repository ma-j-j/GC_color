#ifndef DETECTIONALGORITHM_H
#define DETECTIONALGORITHM_H

#include <QDebug>
#include <QFile>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QPainter>
#include <QRegularExpression> // 新增正则表达式头文件
#include <QString>
#include <QWidget>
#include "15000_BackEnd/log4cplus/qlog4cplus.h"
#include <iostream>
#include <opencv2/opencv.hpp>

// 检测算法参数
struct Algorithm_Parameter
{
public:
    /**芯片**/
    int chip_binary_low, chip_binary_high; // 芯片二值化阈值
    /**卡扣**/
    int buckle_binary_low, buckle_binary_high; // 卡扣二值化阈值
    int buckle_width_low, buckle_width_high;   // 卡扣宽度阈值
    int buckle_height_low, buckle_height_high; // 卡扣高度阈值
    int buckle_left_low, buckle_left_high;     // 卡扣左边差值阈值
    int buckle_right_low, buckle_right_high;   // 卡扣右边差值阈值
};

// 一个相机拍一次照片的检测结果信息
struct Detect_Result
{
public:
    cv::Mat result_img;                // 结果图
    std::vector<QString> socket_error; // 报错信息
    /*
     * 检测结果图
     * 哪个相机的哪个卡扣出现了哪种问题
    */
};

class DetectionAlgorithm
{
public:
    DetectionAlgorithm(Algorithm_Parameter);
    ~DetectionAlgorithm();
    // 图像二值化预处理
    cv::Mat img_proce(cv::Mat in_ROI, int low, int hight);
    // 扣出ROI中的卡扣区域
    void bunckle_region(cv::Mat in_ROI,
                        int left_right_pixel,
                        int up_down_pixel,
                        cv::Mat &up_bunckle,
                        cv::Mat &down_bunckle);
    // 芯片、卡扣检测
    virtual void Detect(cv::Mat &in_ROI,
                        cv::Mat &origin_img,
                        cv::Rect roiRect,
                        int imageIndex,
                        QString &ErrorInformation)
        = 0;

private:
    Algorithm_Parameter m_Algorithm_Parameter;
};

// 上料模式(前):此时KIT压开Socket，需要检测是否有芯片残留和卡扣损坏，有芯片或有卡扣报NG
class LoaderMode_Before : public DetectionAlgorithm
{
public:
    LoaderMode_Before(Algorithm_Parameter param)
        : DetectionAlgorithm(param)
    {}

    void Detect(cv::Mat &in_ROI,
                cv::Mat &origin_img,
                cv::Rect roiRect,
                int imageIndex,
                QString &ErrorInformation) override;
};

// 上料模式(后):此时KIT移开，芯片已放如Socket中，需要检测芯片是否被卡扣卡好，若存在搭边则报NG
class LoaderMode_After : public DetectionAlgorithm
{
public:
    LoaderMode_After(Algorithm_Parameter param)
        : DetectionAlgorithm(param)
    {}
    void Detect(cv::Mat &in_ROI,
                cv::Mat &origin_img,
                cv::Rect roiRect,
                int imageIndex,
                QString &ErrorInformation) override;
};

// 下料模式(前):此时KIT压开Socket，需要检测芯片有无和卡扣损坏，若无芯片直接报NG，若有芯片则进一步检查是否有卡扣残留，有卡扣残留报NG
class CuttingMode_Before : public DetectionAlgorithm
{
public:
    CuttingMode_Before(Algorithm_Parameter param)
        : DetectionAlgorithm(param)
    {}
    void Detect(cv::Mat &in_ROI,
                cv::Mat &origin_img,
                cv::Rect roiRect,
                int imageIndex,
                QString &ErrorInformation) override;
};

// 下料模式(后):此时KIT移开，芯片已从Socket中拿出，需要检测Socket是否残留芯片，有芯片报NG
class CuttingMode_After : public DetectionAlgorithm
{
public:
    CuttingMode_After(Algorithm_Parameter param)
        : DetectionAlgorithm(param)
    {}
    void Detect(cv::Mat &in_ROI,
                cv::Mat &origin_img,
                cv::Rect roiRect,
                int imageIndex,
                QString &ErrorInformation) override;
};

#endif // DETECTIONALGORITHM_H
