#ifndef DAHENG_CAMERA_H
#define DAHENG_CAMERA_H

#include <GalaxyIncludes.h>

#include <QDebug>
#include <QObject>

#include "opencv2/opencv.hpp"

#include "QX-800_Model/Img_detect.h"

// 用户继承采集事件处理类
class CSampleCaptureEventHandler : public QObject, public ICaptureEventHandler
{
    Q_OBJECT
private:
public:
    cv::Mat img;
    explicit CSampleCaptureEventHandler(QObject *parent = nullptr);

    void DoOnImageCaptured(CImageDataPointer &objImageDataPointer,
                           void *pUserParam); //回调函数采集图像
signals:
    void signals_TakePhoto(cv::Mat img); //发图片
    // void signals_test(int);
};

class DaHeng_camera : public QObject
{
    Q_OBJECT
public:
    explicit DaHeng_camera(QObject *parent = nullptr);
    void open_camera(); //初始化相机
    bool *getisSavePicutePtr() { return &isSavePicture; }            //获取是否保存图片的指针
    void setSavePicture(bool isSave) { *isSavePicturePtr = isSave; } //更改是否保存图片的指针

public slots:
    void slotTakePhoto(); //软触发拍照
    void slotSavePictuer(); //离线检测的保存图片按钮触发

signals:
    void signals_TakePhoto(cv::Mat img);  // 发图片用于算法检测
    void signal_SavePicture(cv::Mat img); // 发图片用于保存

private:
    CGXFeatureControlPointer m_ObjFeatureControlPtr;
    CSampleCaptureEventHandler *m_Photo;
    bool isSavePicture;                      //是否保存图片;
    bool *isSavePicturePtr = &isSavePicture; //是否保存图片的指针
};

#endif // DAHENG_CAMERA_H
