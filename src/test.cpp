#include "DetectionAlgorithm.h"

void DetectionAlgorithm::buckle_region(const cv::Mat &in_ROI,
                                       const int &buckle_half_width,
                                       const int &buckle_half_height,
                                       cv::Mat &bunckle1,
                                       cv::Mat &bunckle2,
                                       cv::Point &roi_offset1,
                                       cv::Point &roi_offset2,
                                       const bool &buckle_direction)
{
    if (buckle_direction) // 上下方向
    {
        // 计算图像的上底和下底的中点
        int mid_x = in_ROI.cols / 2;
        int top_y = 0;
        int bottom_y = in_ROI.rows - 1;

        // 向左右同时加 buckle_half_width 个像素值（x方向）
        int left_x = std::max(0, mid_x - buckle_half_width);
        int right_x = std::min(in_ROI.cols - 1, mid_x + buckle_half_width);

        // 扣出上底区域并向下加 buckle_half_height 个像素（y方向）
        int top_height = std::min(buckle_half_height, bottom_y - top_y);
        cv::Rect top_rect(left_x, top_y, right_x - left_x, top_height);
        bunckle1 = in_ROI(top_rect).clone();
        roi_offset1 = top_rect.tl();

        // 扣出下底区域并向上加 buckle_half_height 个像素（y方向）
        int bottom_height = std::min(buckle_half_height, bottom_y - top_y);
        cv::Rect bottom_rect(left_x, bottom_y - bottom_height + 1, right_x - left_x, bottom_height);
        bunckle2 = in_ROI(bottom_rect).clone();
        roi_offset2 = bottom_rect.tl();
    } else // 左右方向
    {
        // 计算图像的左边界和右边界的中点
        int mid_y = in_ROI.rows / 2;
        int left_x = 0;
        int right_x = in_ROI.cols - 1;

        // 向上下同时加 buckle_half_height 个像素值（y方向）
        int top_y = std::max(0, mid_y - buckle_half_width);
        int bottom_y = std::min(in_ROI.rows - 1, mid_y + buckle_half_width);

        // 扣出左区域并向右加 buckle_half_width 个像素（x方向）
        int left_width = std::min(buckle_half_height, right_x - left_x);
        cv::Rect left_rect(left_x, top_y, left_width, bottom_y - top_y + 1);
        bunckle1 = in_ROI(left_rect).clone();
        roi_offset1 = left_rect.tl();

        // 扣出右区域并向左加 buckle_half_height 个像素（x方向）
        int right_width = std::min(buckle_half_height, right_x - left_x);
        cv::Rect right_rect(right_x - right_width + 1, top_y, right_width, bottom_y - top_y + 1);
        bunckle2 = in_ROI(right_rect).clone();
        roi_offset2 = right_rect.tl();
    }
}

bool DetectionAlgorithm::buckle_exist_detect(const cv::Mat &in_ROI,
                                             cv::Mat &origin_img,
                                             const cv::Rect &roiRect,
                                             const std::vector<int> &binary_threshold,
                                             const std::vector<int> &buckle_threshold,
                                             const bool &transform,
                                             const int &detect_mode)
{
    // 拿到上下两个卡扣的区域
    cv::Mat up_bunckle, down_bunckle;
    cv::Point up_roi_offset, down_roi_offset; // 记录 ROI 的偏移量
    std::cout << m_Algorithm_Parameter.buckle_half_width << m_Algorithm_Parameter.buckle_half_height
              << std::endl;
    this->buckle_region(in_ROI,
                        m_Algorithm_Parameter.buckle_half_width,
                        m_Algorithm_Parameter.buckle_half_height,
                        up_bunckle,
                        down_bunckle,
                        up_roi_offset,
                        down_roi_offset,
                        m_Algorithm_Parameter.buckle_direction);

    if (transform == true) // 反转二值图像
    {
        up_bunckle = this->image_binary(up_bunckle, binary_threshold[0], binary_threshold[1]);
        down_bunckle = this->image_binary(down_bunckle, binary_threshold[0], binary_threshold[1]);
        cv::bitwise_not(up_bunckle, up_bunckle);
        cv::bitwise_not(down_bunckle, down_bunckle);
    } else if (transform == false) // 不反转二值图像
    {
        up_bunckle = this->image_binary(up_bunckle, binary_threshold[0], binary_threshold[1]);
        down_bunckle = this->image_binary(down_bunckle, binary_threshold[0], binary_threshold[1]);
    }

    // 对卡扣区域进行形态学操作
    cv::Mat element_1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(20, 20));
    cv::Mat element_2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(50, 50));
    cv::erode(up_bunckle, up_bunckle, element_1);      // 腐蚀
    cv::dilate(up_bunckle, up_bunckle, element_2);     // 膨胀
    cv::erode(down_bunckle, down_bunckle, element_1);  // 腐蚀
    cv::dilate(down_bunckle, down_bunckle, element_2); // 膨胀

    // 检测 up_bunckle 的卡扣
    std::vector<std::vector<cv::Point>> contours_1;
    std::vector<cv::Vec4i> hierarchy_1;
    std::vector<cv::Rect> buckle_rect1; // 存储有卡扣的容器
    findContours(up_bunckle, contours_1, hierarchy_1, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < contours_1.size(); i++) {
        cv::Rect rect = cv::boundingRect(cv::Mat(contours_1[i])); // 获取轮廓外接正矩形（局部坐标）
        std::cout << "Buckle:socket_" << i << "rect: x=" << rect.x << ", y=" << rect.y
                  << ", w=" << rect.width << ", h=" << rect.height << std::endl; // Debug用

        if (rect.width > buckle_threshold[0] && rect.height > buckle_threshold[1]) // 过滤掉小的矩形
        {
            cv::Rect up_bunckle_rect(rect.x + up_roi_offset.x,
                                     rect.y + up_roi_offset.y,
                                     rect.width,
                                     rect.height);
            // 在 in_ROI 中画出卡扣矩形（局部坐标，红色）
            rectangle(in_ROI, up_bunckle_rect, cv::Scalar(0, 0, 255), m_LineWidth, 8, 0);

            // 将局部坐标转换为全局坐标
            // 1. up_bunckle 在 in_ROI 中的起始位置是 up_roi_offset
            // 2. in_ROI 在 origin_img 中的起始位置是 roiRect.x 和 roiRect.y
            // 因此，全局坐标 = rect + up_roi_offset + roiRect
            cv::Rect global_rect(rect.x + up_roi_offset.x + roiRect.x,
                                 rect.y + up_roi_offset.y + roiRect.y,
                                 rect.width,
                                 rect.height);
            buckle_rect1.push_back(global_rect); // 存储有效矩形
            rectangle(origin_img,
                      global_rect,
                      cv::Scalar(0, 255, 0),
                      m_LineWidth,
                      8,
                      0); // 画出卡扣矩形（全局坐标，绿色）
            // std::cout << "rect1.x" << rect.x << "rect1.y" << rect.y
            //           << "rect1.width" << rect.width << "rect1.height" << rect.height << std::endl;
        }
    }
    std::cout << "buckle_rect1:" << buckle_rect1.size() << std::endl;

    // 检测 down_bunckle 的卡扣
    std::vector<std::vector<cv::Point>> contours_2;
    std::vector<cv::Vec4i> hierarchy_2;
    std::vector<cv::Rect> buckle_rect2; // 存储有卡扣的容器
    findContours(down_bunckle, contours_2, hierarchy_2, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < contours_2.size(); i++) {
        cv::Rect rect = cv::boundingRect(cv::Mat(contours_2[i])); // 获取轮廓外接正矩形（局部坐标）
        cv::Rect up_bunckle_rect(rect.x + down_roi_offset.x,
                                 rect.y + down_roi_offset.y,
                                 rect.width,
                                 rect.height);
        if (rect.width > buckle_threshold[0] && rect.height > buckle_threshold[1]) // 过滤掉小的矩形
        {
            // 在 in_ROI 中画出卡扣矩形（局部坐标，红色）
            rectangle(in_ROI, up_bunckle_rect, cv::Scalar(0, 0, 255), m_LineWidth, 8, 0);

            // 将局部坐标转换为全局坐标
            // 1. down_bunckle 在 in_ROI 中的起始位置是 down_roi_offset
            // 2. in_ROI 在 origin_img 中的起始位置是 roiRect.x 和 roiRect.y
            // 因此，全局坐标 = rect + down_roi_offset + roiRect
            cv::Rect global_rect(rect.x + down_roi_offset.x + roiRect.x,
                                 rect.y + down_roi_offset.y + roiRect.y,
                                 rect.width,
                                 rect.height);
            buckle_rect2.push_back(global_rect); // 存储有效矩形
            rectangle(origin_img,
                      global_rect,
                      cv::Scalar(0, 255, 0),
                      m_LineWidth,
                      8,
                      0); // 画出卡扣矩形（全局坐标，绿色）
            // std::cout << "rect2.x" << rect.x << "rect2.y" << rect.y
            //           << "rect2.width" << rect.width << "rect2.height" << rect.height << std::endl;
        }
    }
    std::cout << "buckle_rect2:" << buckle_rect2.size() << std::endl;

    // 根据检测模式来判断卡扣状态
    switch (detect_mode) {
    case 1: // LoaderMode_Before
        if (buckle_rect1.size() == 1 && buckle_rect2.size() == 1) {
            // std::cout << "OK: Socket Have Buckle" << std::endl;
            return true;
        } else {
            // std::cout << "NG: Socket Haven't Buckle" << std::endl;
            return false;
        }
        break;

    case 2: // LoaderMode_Middle
        if (buckle_rect1.size() == 0 && buckle_rect2.size() == 0) {
            // std::cout << "OK: Socket Haven't Buckle" << std::endl;
            return true;
        } else {
            // std::cout << "NG: Socket Have Buckle" << std::endl;
            return false;
        }
        break;

    case 5: // CuttingMode_Middle
        if (buckle_rect1.size() == 0 && buckle_rect2.size() == 0) {
            // std::cout << "OK: Socket Haven't Buckle" << std::endl;
            return true;
        } else {
            // std::cout << "NG: Socket Have Buckle" << std::endl;
            return false;
        }
        break;

    case 6: // CuttingMode_After
        if (buckle_rect1.size() == 1 && buckle_rect2.size() == 1) {
            // std::cout << "OK: Socket Have Buckle" << std::endl;
            return true;
        } else {
            // std::cout << "NG: Socket Haven't Buckle" << std::endl;
            return false;
        }
        break;

    default:
        std::cerr << "Error: Unknown detect_mode: " << detect_mode << std::endl;
        return false; // 默认返回失败
    }
}

void DetectionAlgorithm::chip_region(const cv::Mat &in_ROI,
                                     const int &half_width,
                                     const int &half_height,
                                     cv::Mat &chip,
                                     cv::Point &roi_offset)
{
    // 计算输入图像的中心坐标
    int center_x = in_ROI.cols / 2;
    int center_y = in_ROI.rows / 2;

    // 计算矩形的左上角和右下角坐标
    int rect_x1 = center_x - half_width;
    int rect_y1 = center_y - half_height;
    int rect_x2 = center_x + half_width;
    int rect_y2 = center_y + half_height;

    // 确保矩形不超出图像边界
    rect_x1 = std::max(0, rect_x1);
    rect_y1 = std::max(0, rect_y1);
    rect_x2 = std::min(in_ROI.cols - 1, rect_x2);
    rect_y2 = std::min(in_ROI.rows - 1, rect_y2);

    // 创建矩形区域
    cv::Rect chip_rect(rect_x1, rect_y1, rect_x2 - rect_x1, rect_y2 - rect_y1);

    // 提取矩形区域到输出图像
    chip = in_ROI(chip_rect).clone();

    roi_offset = chip_rect.tl(); // 左上角坐标即为偏移量
}

bool DetectionAlgorithm::buckle_edging_detect(const cv::Mat &in_ROI,
                                              cv::Mat &origin_img,
                                              const cv::Rect &roiRect,
                                              const std::vector<int> &binary_threshold,
                                              const std::vector<int> &buckle_edging_threshold,
                                              const bool &transform,
                                              const int &detect_mode)
{
    // 拿到芯片位置的区域
    cv::Mat chip_ROI;
    cv::Point roi_offset; // 记录 ROI 的偏移量
    this->chip_region(in_ROI,
                      m_Algorithm_Parameter.chip_half_width,
                      m_Algorithm_Parameter.chip_half_height,
                      chip_ROI,
                      roi_offset);

    if (transform == true) // 反转二值图像
    {
        chip_ROI = this->image_binary(chip_ROI, binary_threshold[0], binary_threshold[1]);
        cv::bitwise_not(chip_ROI, chip_ROI);
    } else if (transform == false) // 不反转二值图像
    {
        chip_ROI = this->image_binary(chip_ROI, binary_threshold[0], binary_threshold[1]);
    }

    // 对芯片区域进行形态学操作
    cv::Mat element_1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15)); // 20
    cv::Mat element_2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(45, 45)); // 50
    cv::erode(chip_ROI, chip_ROI, element_1);  // 腐蚀
    cv::dilate(chip_ROI, chip_ROI, element_2); // 膨胀

    // 检测芯片中的卡扣
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<cv::Rect> buckle_rect; // 存储有卡扣的容器
    findContours(chip_ROI, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::cout << "contours:" << contours.size() << std::endl;

    for (size_t i = 0; i < contours.size(); i++) {
        cv::Rect rect = cv::boundingRect(cv::Mat(contours[i])); // 获取轮廓外接正矩形（局部坐标）
        std::cout << "EdgingBuckle:socket_" << i << "rect: x=" << rect.x << ", y=" << rect.y
                  << ", w=" << rect.width << ", h=" << rect.height << std::endl; // Debug用

        if (rect.width > buckle_edging_threshold[0]
            && rect.height > buckle_edging_threshold[1]) // 过滤掉小的矩形
        {
            cv::Rect up_bunckle_rect(rect.x + roi_offset.x,
                                     rect.y + roi_offset.y,
                                     rect.width,
                                     rect.height);

            // 在 in_ROI 中画出卡扣矩形（局部坐标，红色）
            rectangle(in_ROI, up_bunckle_rect, cv::Scalar(0, 0, 255), m_LineWidth, 8, 0);

            // 将局部坐标转换为全局坐标
            // 1. up_bunckle 在 in_ROI 中的起始位置是 roi_offset
            // 2. in_ROI 在 origin_img 中的起始位置是 roiRect.x 和 roiRect.y
            // 因此，全局坐标 = rect + roi_offset + roiRect
            cv::Rect global_rect(rect.x + roi_offset.x + roiRect.x,
                                 rect.y + roi_offset.y + roiRect.y,
                                 rect.width,
                                 rect.height);
            buckle_rect.push_back(global_rect); // 存储有效矩形
            rectangle(origin_img,
                      global_rect,
                      cv::Scalar(0, 255, 0),
                      m_LineWidth,
                      8,
                      0); // 画出卡扣矩形（全局坐标，绿色）
            std::cout << "global_rect.x" << global_rect.x << "global_rect.y" << global_rect.y
                      << "global_rect.width" << global_rect.width << "global_rect.height"
                      << global_rect.height << std::endl;
        }
    }
    std::cout << "buckle_rect:" << buckle_rect.size() << std::endl;

    switch (detect_mode) {
    case 3: // LoaderMode_After
        if (buckle_rect.size() == 2) {
            std::cout << "OK: Socket Have Chip And Buckle" << std::endl;
            return true; // 检测到芯片和卡扣
        } else {
            std::cout << "NG: Socket Chip Is Edging Buckle" << std::endl;
            return false; // 没有检测到芯片或卡扣不完整
        }
        break;
    case 4: // CuttingMode_Before
        if (buckle_rect.size() == 2) {
            std::cout << "OK: Socket Have Chip And Buckle" << std::endl;
            return true; // 检测到芯片和卡扣
        } else {
            std::cout << "NG: Socket Chip Is Edging Buckle" << std::endl;
            return false; // 没有检测到芯片或卡扣不完整
        }
        break;

    default:
        std::cerr << "Error: Unknown detect_mode: " << detect_mode << std::endl;
        return false;
    }
}

int DetectionAlgorithm::count_chip_points(const cv::Mat &in_ROI)
{
    // 芯片二值化阈值
    cv::Mat binary = this->image_binary(in_ROI,
                                        m_Algorithm_Parameter.chip_binary_low,
                                        m_Algorithm_Parameter.chip_binary_high);

    cv::bitwise_not(binary, binary); // 反转二值图像

    int white_pixel_count = cv::countNonZero(binary);
    std::cout << "White pixel count: " << white_pixel_count << std::endl; // Debug用

    return white_pixel_count;
}

cv::Mat DetectionAlgorithm::image_binary(const cv::Mat &in_img, const int &low, const int &high)
{
    cv::Mat gray, binary;
    if (in_img.channels() == 3 || in_img.channels() == 4) {
        cvtColor(in_img, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = in_img; // 如果已经是灰度图像，直接使用
    }
    threshold(gray, binary, low, high, cv::THRESH_BINARY);
    return binary;
}

/**
 * 芯片检测:对ROI进行二值化并反转,超过x个白色像素点则认为有芯片残留
 * 卡扣检测:抠出卡扣区域,对ROI进行二值化并反转,检测卡扣区域
 */
void LoaderMode_Before::Detect(const cv::Mat &in_ROI,
                               cv::Mat &origin_img,
                               const cv::Rect &roiRect,
                               const int &imageIndex,
                               QString &ErrorInformation)
{
    qDebug() << "调用上料前检测算法";
    int detect_mode = 1; // 模式1

    // 克隆输入图像以保留原始图像
    cv::Mat original_ROI = in_ROI.clone();

    // 检测芯片残留
    int white_pixel_count = this->count_chip_points(in_ROI);
    if (white_pixel_count > m_Algorithm_Parameter.chip_exist_low) {
        std::cout << "NG: Socket " << imageIndex << " Have Chip" << std::endl;
        ErrorInformation = QString("Socket_%1 NG: Have Chip").arg(imageIndex); // 芯片报错
        rectangle(origin_img,
                  roiRect,
                  cv::Scalar(0, 0, 255),
                  m_LineWidth,
                  8,
                  0); // 画出芯片矩形（红色）
        return;
    } else {
        std::vector<int> binary_threshold
            = {m_Algorithm_Parameter.buckle_binary_low,
               m_Algorithm_Parameter.buckle_binary_high}; // 卡扣二值化阈值
        std::vector<int> buckle_threshold = {m_Algorithm_Parameter.buckle_width_low,
                                             m_Algorithm_Parameter.buckle_height_low}; // 卡扣长宽
        bool is_transform = true;                       // 反转二值图
        bool tf_buckle = this->buckle_exist_detect(original_ROI,
                                                   origin_img,
                                                   roiRect,
                                                   binary_threshold,
                                                   buckle_threshold,
                                                   is_transform,
                                                   detect_mode); // 检测卡扣
        if (tf_buckle == true) {
            std::cout << "OK: Socket " << imageIndex << " No Chip And Buckle is OK" << std::endl;
        } else {
            std::cout << "NG: Socket " << imageIndex << " Buckle is Broken" << std::endl;
            ErrorInformation = QString("Socket_%1 NG: Buckle is Broken").arg(imageIndex); // 卡扣报错
            rectangle(origin_img,
                      roiRect,
                      cv::Scalar(0, 0, 255),
                      m_LineWidth,
                      8,
                      0); // 画出卡扣矩形（红色）
        }
    }
}

/**
 * 卡扣检测:抠出卡扣区域,对ROI进行二值化并反转,检测卡扣区域
 */
void LoaderMode_Middle::Detect(const cv::Mat &in_ROI,
                               cv::Mat &origin_img,
                               const cv::Rect &roiRect,
                               const int &imageIndex,
                               QString &ErrorInformation)
{
    qDebug() << "调用上料中检测算法";
    int detect_mode = 2; // 模式2

    // 克隆输入图像以保留原始图像
    cv::Mat original_ROI = in_ROI.clone();

    std::vector<int> binary_threshold = {m_Algorithm_Parameter.buckle_binary_low,
                                         m_Algorithm_Parameter.buckle_binary_high}; // 卡扣二值化阈值
    std::vector<int> buckle_threshold = {m_Algorithm_Parameter.buckle_width_low,
                                         m_Algorithm_Parameter.buckle_height_low}; // 卡扣长宽
    bool is_transform = true;
    bool tf_buckle = this->buckle_exist_detect(original_ROI,
                                               origin_img,
                                               roiRect,
                                               binary_threshold,
                                               buckle_threshold,
                                               is_transform,
                                               detect_mode); // 检测卡扣
    if (tf_buckle == true) {
        std::cout << "OK: Socket " << imageIndex << " No Chip And NO Buckle " << std::endl;
    } else {
        std::cout << "NG: Socket " << imageIndex << " Buckle is Broken" << std::endl;
        ErrorInformation = QString("Socket_%1 NG: Buckle is Broken").arg(imageIndex); // 卡扣报错
        rectangle(origin_img,
                  roiRect,
                  cv::Scalar(0, 0, 255),
                  m_LineWidth,
                  8,
                  0); // 画出卡扣矩形（红色）
    }
}

/**
 * 芯片检测:对ROI进行二值化并反转,超过x个白色像素点则认为有芯片,报OK,若无则NG
 * 卡扣检测:抠出卡扣区域,对ROI进行二值化,检测卡扣区域的反光点,給一个大的值,检测两个卡扣有没有卡好
 */
void LoaderMode_After::Detect(const cv::Mat &in_ROI,
                              cv::Mat &origin_img,
                              const cv::Rect &roiRect,
                              const int &imageIndex,
                              QString &ErrorInformation)
{
    qDebug() << "调用上料后检测算法";
    int detect_mode = 3; // 模式3

    // 克隆输入图像以保留原始图像
    cv::Mat original_ROI = in_ROI.clone();

    // 检测芯片有无
    int white_pixel_count = this->count_chip_points(original_ROI);
    if (white_pixel_count > m_Algorithm_Parameter.chip_exist_low) // 有芯片,进一步检测卡扣有无卡好
    {
        std::vector<int> binary_edging_threshold
            = {m_Algorithm_Parameter.buckle_edging_binary_low,
               m_Algorithm_Parameter.buckle_edging_binary_high}; // 搭边卡扣二值化阈值
        std::vector<int> buckle_edging_threshold
            = {m_Algorithm_Parameter.buckle_edging_width_low,
               m_Algorithm_Parameter.buckle_edging_height_low}; // 搭边卡扣长宽

        bool is_transform = true; // 反转二值图像
        bool tf_buckle = this->buckle_edging_detect(original_ROI,
                                                    origin_img,
                                                    roiRect,
                                                    binary_edging_threshold,
                                                    buckle_edging_threshold,
                                                    is_transform,
                                                    detect_mode); // 检测卡扣
        if (tf_buckle == true) {
            std::cout << "OK: Socket " << imageIndex << "Have Chip And Buckle is OK" << std::endl;
        } else {
            std::cout << "NG: Socket " << imageIndex << " Buckle is Edging" << std::endl;
            ErrorInformation = QString("Socket_%1 NG: Chip is Edging").arg(imageIndex); // 卡扣报错
            rectangle(origin_img,
                      roiRect,
                      cv::Scalar(0, 0, 255),
                      m_LineWidth,
                      8,
                      0); // 画出卡扣矩形（红色）
        }
    } else // 无芯片NG
    {
        std::cout << "NG: Socket " << imageIndex << " No Chip" << std::endl;
        ErrorInformation = QString("Socket_%1 NG: No Chip").arg(imageIndex); // 芯片报错
        rectangle(origin_img,
                  roiRect,
                  cv::Scalar(0, 0, 255),
                  m_LineWidth,
                  8,
                  0); // 画出芯片矩形（红色）
        return;
    }
}

/**
 * 芯片检测:对ROI进行二值化并反转,超过x个白色像素点则认为有芯片,报OK,若无则NG
 * 卡扣检测:抠出卡扣区域,对ROI进行二值化,检测卡扣区域的反光点,給一个大的值,检测两个卡扣有没有卡好
 */
void CuttingMode_Before::Detect(const cv::Mat &in_ROI,
                                cv::Mat &origin_img,
                                const cv::Rect &roiRect,
                                const int &imageIndex,
                                QString &ErrorInformation)
{
    qDebug() << "调用下料前检测算法";
    int detect_mode = 4; // 模式4

    // 克隆输入图像以保留原始图像
    cv::Mat original_ROI = in_ROI.clone();
    // 检测芯片缺失
    int white_pixel_count = this->count_chip_points(in_ROI);
    if (white_pixel_count > m_Algorithm_Parameter.chip_exist_low) // 有芯片,进一步检测卡扣有无卡好
    {
        std::vector<int> binary_edging_threshold
            = {m_Algorithm_Parameter.buckle_edging_binary_low,
               m_Algorithm_Parameter.buckle_edging_binary_high}; // 搭边卡扣二值化阈值
        std::vector<int> buckle_edging_threshold
            = {m_Algorithm_Parameter.buckle_edging_width_low,
               m_Algorithm_Parameter.buckle_edging_height_low}; // 搭边卡扣长宽

        bool is_transform = true; // 反转二值图像
        bool tf_buckle = this->buckle_edging_detect(original_ROI,
                                                    origin_img,
                                                    roiRect,
                                                    binary_edging_threshold,
                                                    buckle_edging_threshold,
                                                    is_transform,
                                                    detect_mode); // 检测卡扣
        if (tf_buckle == true) {
            std::cout << "OK: Socket " << imageIndex << " Have Chip And Buckle is OK" << std::endl;
        } else {
            std::cout << "NG: Socket " << imageIndex << " Chip is Edging" << std::endl;
            ErrorInformation = QString("Socket_%1 NG: Chip is Edging").arg(imageIndex); // 卡扣报错
            rectangle(origin_img,
                      roiRect,
                      cv::Scalar(0, 0, 255),
                      m_LineWidth,
                      8,
                      0); // 画出卡扣矩形（红色）
        }
    } else {
        std::cout << "NG: Socket " << imageIndex << " No Chip" << std::endl;
        ErrorInformation = QString("Socket_%1 NG: No Chip").arg(imageIndex); // 芯片报错
        rectangle(origin_img,
                  roiRect,
                  cv::Scalar(0, 0, 255),
                  m_LineWidth,
                  8,
                  0); // 画出芯片矩形（红色）
        return;
    }
}

/**
 * 卡扣检测:抠出卡扣区域,对ROI进行二值化,检测卡扣区域的反光点,給一个大的值,检测两个卡扣有没有缺损
 */
void CuttingMode_Middle::Detect(const cv::Mat &in_ROI,
                                cv::Mat &origin_img,
                                const cv::Rect &roiRect,
                                const int &imageIndex,
                                QString &ErrorInformation)
{
    qDebug() << "调用下料中检测算法";
    int detect_mode = 5; // 模式5

    // 克隆输入图像以保留原始图像
    cv::Mat original_ROI = in_ROI.clone();

    std::vector<int> binary_edging_threshold
        = {m_Algorithm_Parameter.buckle_edging_binary_low,
           m_Algorithm_Parameter.buckle_edging_binary_high}; // 搭边卡扣二值化阈值
    std::vector<int> buckle_edging_threshold
        = {m_Algorithm_Parameter.buckle_edging_width_low,
           m_Algorithm_Parameter.buckle_edging_height_low}; // 搭边卡扣长宽

    bool is_transform = true; // 反转二值图像
    bool tf_buckle = this->buckle_edging_detect(original_ROI,
                                                origin_img,
                                                roiRect,
                                                binary_edging_threshold,
                                                buckle_edging_threshold,
                                                is_transform,
                                                detect_mode); // 检测卡扣
    if (tf_buckle == true) {
        std::cout << "OK: Socket " << imageIndex << "Buckle is OK" << std::endl;
    } else {
        std::cout << "NG: Socket " << imageIndex << "Buckle is Broken" << std::endl;
        ErrorInformation = QString("Socket_%1 NG: Buckle is Broken").arg(imageIndex); // 卡扣报错
        rectangle(origin_img,
                  roiRect,
                  cv::Scalar(0, 0, 255),
                  m_LineWidth,
                  8,
                  0); // 画出卡扣矩形（红色）
    }
}

/**
 * 芯片检测:对ROI进行二值化并反转,超过x个白色像素点则认为有芯片,报NG,若无则OK
 * 卡扣检测:抠出卡扣区域,对ROI进行二值化,检测卡扣区域的反光点,給一个大的值,检测两个卡扣有没有缺损
 */
void CuttingMode_After::Detect(const cv::Mat &in_ROI,
                               cv::Mat &origin_img,
                               const cv::Rect &roiRect,
                               const int &imageIndex,
                               QString &ErrorInformation)
{
    qDebug() << "调用下料后检测算法";
    int detect_mode = 6; // 模式6
    // 克隆输入图像以保留原始图像
    cv::Mat original_ROI = in_ROI.clone();

    // 检测芯片有无
    int white_pixel_count = this->count_chip_points(original_ROI);
    if (white_pixel_count > m_Algorithm_Parameter.chip_exist_low) // 有芯片NG
    {
        std::cout << "NG: Socket " << imageIndex << " Have Chip" << std::endl;
        ErrorInformation = QString("Socket_%1 NG: Have Chip").arg(imageIndex); // 芯片报错
        rectangle(origin_img,
                  roiRect,
                  cv::Scalar(0, 0, 255),
                  m_LineWidth,
                  8,
                  0); // 画出芯片矩形（红色）
        return;
    } else // 无芯片OK
    {
        std::vector<int> binary_threshold
            = {m_Algorithm_Parameter.buckle_binary_low,
               m_Algorithm_Parameter.buckle_binary_high}; // 卡扣二值化阈值
        std::vector<int> buckle_threshold = {m_Algorithm_Parameter.buckle_width_low,
                                             m_Algorithm_Parameter.buckle_height_low}; // 卡扣长宽

        bool is_transform = true; // 反转二值图像
        bool tf_buckle = this->buckle_exist_detect(in_ROI,
                                                   origin_img,
                                                   roiRect,
                                                   binary_threshold,
                                                   buckle_threshold,
                                                   is_transform,
                                                   detect_mode); // 检测卡扣
        if (tf_buckle == true) {
            std::cout << "OK: Socket " << imageIndex << " No Chip And Buckle is OK" << std::endl;
        } else {
            std::cout << "NG: Socket " << imageIndex << " Buckle is Broken" << std::endl;
            ErrorInformation = QString("Socket_%1 NG: Buckle is Broken").arg(imageIndex); // 卡扣报错
            rectangle(origin_img,
                      roiRect,
                      cv::Scalar(0, 0, 255),
                      m_LineWidth,
                      8,
                      0); // 画出卡扣矩形（红色）
        }
    }
}
