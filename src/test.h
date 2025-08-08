#ifndef CAMERA_H
#define CAMERA_H

#include <QDebug>
#include <QJsonObject>
#include <QMessageBox>
#include <QObject>
#include "15000_BackEnd/log4cplus/qlog4cplus.h"
#include <GalaxyIncludes.h>
#include <iostream>
#include <opencv2/opencv.hpp>

// 图片标志位
enum class PictureType {
    img_save,                      // 模式类型0
    img_show,                      // 模式类型1
    img_detect_LoaderMode_Before,  // 模式类型2
    img_detect_LoaderMode_Middle,  // 模式类型3
    img_detect_LoaderMode_After,   // 模式类型4
    img_detect_CuttingMode_Before, // 模式类型5
    img_detect_CuttingMode_Middle, // 模式类型6
    img_detect_CuttingMode_After   // 模式类型7
};

// 定义字符串到 PictureType 的映射
static QMap<QString, PictureType> createPictureTypeMapping()
{
    QMap<QString, PictureType> mapping;
    mapping["OffLineImageSave"] = PictureType::img_save;
    mapping["OffLineImageShow"] = PictureType::img_show;
    mapping["LoaderMode_Before"] = PictureType::img_detect_LoaderMode_Before;
    mapping["LoaderMode_Middle"] = PictureType::img_detect_LoaderMode_Middle;
    mapping["LoaderMode_After"] = PictureType::img_detect_LoaderMode_After;
    mapping["CuttingMode_Before"] = PictureType::img_detect_CuttingMode_Before;
    mapping["CuttingMode_Middle"] = PictureType::img_detect_CuttingMode_Middle;
    mapping["CuttingMode_After"] = PictureType::img_detect_CuttingMode_After;
    return mapping;
}

// 用于检测模式的全局映射表
static const QMap<QString, PictureType> g_pictureTypeMapping = createPictureTypeMapping();

// 相机拍照信息
struct CameraImage_information
{
    int CamerID;
    PictureType m_PictureType;
    std::string CameraSN;
    cv::Mat img;
};

// 用户继承采集事件处理类
class CSampleCaptureEventHandler : public QObject, public ICaptureEventHandler
{
    Q_OBJECT
private:
    std::shared_ptr<CameraImage_information> m_imgInformation;

public:
    explicit CSampleCaptureEventHandler(const std::string &sn, int camerID)
    {
        m_imgInformation->CameraSN = sn;
        m_imgInformation->CamerID = camerID;
    }

    //回调函数采集图像
    void DoOnImageCaptured(CImageDataPointer &objImageDataPointer, void *pUserParam);

    // 相机用于设置图片模式
    void setPictureType(PictureType type) { m_imgInformation->m_PictureType = type; }

signals:
    void signals_Camer_Sent_TakePhoto(std::shared_ptr<CameraImage_information>); //发图片
};

class Camera : public QObject
{
    Q_OBJECT
public:
    explicit Camera(QObject *parent = nullptr);
    ~Camera();

    //初始化相机
    void Camera_Init();

    // 使用map来建立SN与编号的映射关系，更清晰易维护
    std::unordered_map<std::string, int> m_CameraMapTable = {
        {"LKR25070001", 0}, // 1号相机
        {"LKR25070002", 1}, // 2号相机
        {"LKR25070003", 2}  // 3号相机
    };

public slots:
    void slots_Camera_Receive_Exposure(QJsonObject);   // 接收曝光时间
    void slots_Camera_Receive_TakePhoto_Mode(QString); // 接收检测模式

signals:
    void signals_Camera_Sent_SavePicture(std::shared_ptr<CameraImage_information>); // 发图片用于保存
    void singals_Camera_Sent_ShowPicture(
        std::shared_ptr<CameraImage_information>); // 发图片用于离线检测显示
    void signals_Camera_Sent_DetectThread(
        std::shared_ptr<CameraImage_information>); // 发图片用于算法检测

private:
    CSampleCaptureEventHandler *m_CSampleCapture;
    ICaptureEventHandler *m_ICapture;
    // 相机的成员变量
    std::vector<CGXDevicePointer> m_devices;
    std::vector<CGXStreamPointer> m_streams;
    std::vector<CGXFeatureControlPointer> m_featureControls;
    std::vector<CSampleCaptureEventHandler *> m_eventHandlers;
    int m_Camera_Exposure; // 相机曝光时间
};

#endif // CAMERA_H
