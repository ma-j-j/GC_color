#ifndef RECEIVE_SIGNAL_H
#define RECEIVE_SIGNAL_H
#include <iostream>

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include "QX-800_Model/Img_detect.h"
#include "QX-800_Model/daheng_camera.h"

class receive : public QObject
{    Q_OBJECT
private:
    cv::Mat result;
    std::vector<cv::Rect> m_ROI;
    cv::Mat m_img;
    bool *SavepictutePtr;

    chip_detect *m_Detect;
    DaHeng_camera *m_Picutre;

public:
    explicit receive(QObject *parent = nullptr);// 默认构造函数

public slots:
    void slotReciveParamete(std::vector<int> &Para);         // 接收算法参数
    void slotReciveFile(QString, QString);                   // 接收图片路径和ROI.Json，用于离线测试
    void slotWriteJson(QJsonObject JsonObj);                 // 接收修改信息并写入json文件
    void slotReceiveROIJson(std::vector<cv::Rect> roi_data); // 接收ROI.Json信号用于算法检测
    void slotReceiveTakePhoto(cv::Mat img);                  // 接收拍照图片用于算法检测
    void slotReceiveSavePicture(cv::Mat img);                // 接收拍照图片并保存

signals:
    void signal_sent_detect_Image(QImage img);                    // 发送检测后的图片(离线测试)
    void signal_sentROI_ToDetect(std::vector<cv::Rect> roi_data); // 发送ROI区域信息到后端检测
    // void signal_sentCameraData_toMainWindow(QJsonObject JsonObj); // 发送曝光时间
    void signal_sent_Detect_Img(QImage);                          // 发送检测后的图片（算法检测）
};

#endif // RECEIVE_SIGNAL_H
