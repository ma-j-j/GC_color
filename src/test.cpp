#include "DetectionAlgorithm.h"

DetectionAlgorithm::DetectionAlgorithm(Algorithm_Parameter P)
{
    m_Algorithm_Parameter.chip_binary_high = P.chip_binary_high;
    m_Algorithm_Parameter.chip_binary_low = P.chip_binary_low;
    m_Algorithm_Parameter.buckle_binary_high = P.buckle_binary_high;
    m_Algorithm_Parameter.buckle_binary_low = P.buckle_binary_low;
    m_Algorithm_Parameter.buckle_height_high = P.buckle_height_high;
    m_Algorithm_Parameter.buckle_height_low = P.buckle_height_low;
    m_Algorithm_Parameter.buckle_left_high = P.buckle_left_high;
    m_Algorithm_Parameter.buckle_left_low = P.buckle_left_low;
    m_Algorithm_Parameter.buckle_right_high = P.buckle_right_high;
    m_Algorithm_Parameter.buckle_right_low = P.buckle_right_low;
    m_Algorithm_Parameter.buckle_width_high = P.buckle_width_high;
    m_Algorithm_Parameter.buckle_width_low = P.buckle_width_low;
}

DetectionAlgorithm::~DetectionAlgorithm() {}

cv::Mat DetectionAlgorithm::img_proce(cv::Mat in_img, int low, int high)
{
    cv::Mat gray, binary;
    if (in_img.channels() == 3 || in_img.channels() == 4) {
        cvtColor(in_img, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = in_img; // 如果已经是灰度图像，直接使用
    }
    threshold(gray, binary, low, high, cv::THRESH_BINARY); // cv::THRESH_BINARY | cv::THRESH_OTSU
    return binary;
}

// 通过传入的ROI区域，计算图的上、下底中心点，向左右同时增加left_right_pixel个像素，向上或向下增加up_down_pixel个像素，拿到卡扣的区域
void DetectionAlgorithm::bunckle_region(cv::Mat in_ROI,
                                        int left_right_pixel,
                                        int up_down_pixel,
                                        cv::Mat &up_bunckle,
                                        cv::Mat &down_bunckle)
{
    // 计算图像的上底和下底的中点
    int top_mid_x = in_ROI.cols / 2;
    int bottom_mid_x = in_ROI.cols / 2;
    int top_y = 0;
    int bottom_y = in_ROI.rows - 1;

    // 向左右同时加left_right_pixel个像素值
    int left_x = std::max(0, top_mid_x - left_right_pixel);
    int right_x = std::min(in_ROI.cols - 1, top_mid_x + left_right_pixel);

    // 扣出上底区域并向下加up_down_pixel个像素
    int top_height = std::min(up_down_pixel, bottom_y - top_y); // 确保不超出图像范围
    cv::Rect top_rect(left_x, top_y, right_x - left_x, top_height);
    up_bunckle = in_ROI(top_rect).clone();

    // 扣出下底区域并向上加up_down_pixel个像素
    int bottom_height = std::min(up_down_pixel, bottom_y - top_y); // 确保不超出图像范围
    cv::Rect bottom_rect(left_x, bottom_y - bottom_height + 1, right_x - left_x, bottom_height);
    down_bunckle = in_ROI(bottom_rect).clone();
}

void DetectionAlgorithm::Detect(cv::Mat &in_ROI,
                                cv::Mat &origin_img,
                                cv::Rect roiRect,
                                int imageIndex,
                                QString &ErrorInformation)
{
    qDebug() << "调用基类检测算法";
}

void LoaderMode_Before::Detect(cv::Mat &in_ROI,
                               cv::Mat &origin_img,
                               cv::Rect roiRect,
                               int imageIndex,
                               QString &ErrorInformation)
{
    qDebug() << "调用上料前检测算法";
}

void LoaderMode_After::Detect(cv::Mat &in_ROI,
                              cv::Mat &origin_img,
                              cv::Rect roiRect,
                              int imageIndex,
                              QString &ErrorInformation)
{
    qDebug() << "调用上料后检测算法";
}

void CuttingMode_Before::Detect(cv::Mat &in_ROI,
                                cv::Mat &origin_img,
                                cv::Rect roiRect,
                                int imageIndex,
                                QString &ErrorInformation)
{
    qDebug() << "调用下料前检测算法";
}

void CuttingMode_After::Detect(cv::Mat &in_ROI,
                               cv::Mat &origin_img,
                               cv::Rect roiRect,
                               int imageIndex,
                               QString &ErrorInformation)
{
    qDebug() << "调用下料后检测算法";
}
