#include "daheng_camera.h"

DaHeng_camera::DaHeng_camera(QObject *parent)
{
    this->m_Photo = new CSampleCaptureEventHandler(); // 采集回调对象
}
CSampleCaptureEventHandler::CSampleCaptureEventHandler(QObject *parent)
{
}

// 初始化相机
void DaHeng_camera::open_camera()
{
    //初始化
    IGXFactory::GetInstance().Init();

    gxdeviceinfo_vector vectorDeviceInfo;
    IGXFactory::GetInstance().UpdateDeviceList(1000, vectorDeviceInfo);
    if (0 == vectorDeviceInfo.size()) {
    }
    //打开第一台设备以及设备下面第一个流
    CGXDevicePointer ObjDevicePtr
        = IGXFactory::GetInstance().OpenDeviceBySN(vectorDeviceInfo[0].GetSN(), GX_ACCESS_EXCLUSIVE);
    CGXStreamPointer ObjStreamPtr = ObjDevicePtr->OpenStream(0);

    // 获取远端设备属性控制器
    this->m_ObjFeatureControlPtr = ObjDevicePtr->GetRemoteFeatureControl();
    //注册远端设备事件:曝光结束事件【目前只有千兆网系列相机支持曝光结束事件】
    // 选择事件源
    this->m_ObjFeatureControlPtr->GetEnumFeature("EventSelector")->SetValue("ExposureEnd");
    // 使能事件
    this->m_ObjFeatureControlPtr->GetEnumFeature("EventNotification")->SetValue("On");

    // 设置触发模式为On
    this->m_ObjFeatureControlPtr->GetEnumFeature("TriggerMode")->SetValue("On");
    // 设置触发源为软触发
    this->m_ObjFeatureControlPtr->GetEnumFeature("TriggerSource")->SetValue("Software");

    // 注册回调采集
    if (nullptr == this->m_Photo) {
        this->m_Photo = new CSampleCaptureEventHandler(); // 采集回调对象
    }
    ObjStreamPtr->RegisterCaptureCallback(this->m_Photo, NULL);

    connect(this->m_Photo,
            &CSampleCaptureEventHandler::signals_TakePhoto,
            this,
            &DaHeng_camera::signals_TakePhoto);
    connect(this->m_Photo,
            &CSampleCaptureEventHandler::signals_TakePhoto,
            this,
            &DaHeng_camera::signal_SavePicture); // 保存图片

    //发送开采命令
    ObjStreamPtr->StartGrab();
    this->m_ObjFeatureControlPtr->GetCommandFeature("AcquisitionStart")->Execute();

    qDebug() << "相机初始化成功";
}

void CSampleCaptureEventHandler::DoOnImageCaptured(CImageDataPointer &objImageDataPointer,
                                                   void *pUserParam)
{
    img.create(objImageDataPointer->GetHeight(), objImageDataPointer->GetWidth(), CV_8UC3);
    void *pRGB24Buffer = NULL;
    // 假设原始数据是BayerRG8图像
    pRGB24Buffer = objImageDataPointer->ConvertToRGB24(GX_BIT_0_7, GX_RAW2RGB_NEIGHBOUR, true);
    memcpy(img.data,
           pRGB24Buffer,
           (objImageDataPointer->GetHeight()) * (objImageDataPointer->GetWidth()) * 3);
    if (!img.empty()) {
        // cv::imwrite("photo.png", img);
        emit signals_TakePhoto(img);
        qDebug() << "拍照成功";
    } else {
        qDebug() << "拍照失败";
    }
}

void DaHeng_camera::slotTakePhoto()
{
    qDebug() << "收到拍照信号，开始拍照";
    this->setSavePicture(false);
    // 发送软触发命令
    this->m_ObjFeatureControlPtr->GetCommandFeature("TriggerSoftware")->Execute();
    qDebug() << "slotTakePhoto" << this->isSavePicture;
}

void DaHeng_camera::slotSavePictuer()
{
    qDebug() << "收到保存图片信号";
    this->setSavePicture(true);
    // 发送软触发命令
    this->m_ObjFeatureControlPtr->GetCommandFeature("TriggerSoftware")->Execute();
    qDebug() << "slotSavePictuer" << this->isSavePicture;
}
