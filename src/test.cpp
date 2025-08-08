#include "Camera.h"

Camera::Camera(QObject *parent) {}

Camera::~Camera()
{
    delete m_CSampleCapture;
    delete m_ICapture;
}

void Camera::Camera_Init()
{
    //初始化
    IGXFactory::GetInstance().Init();

    gxdeviceinfo_vector vectorDeviceInfo;
    IGXFactory::GetInstance().UpdateDeviceList(1000, vectorDeviceInfo);
    qDebug() << "相机数量：" << vectorDeviceInfo.size();
    // if (vectorDeviceInfo.size() != 3) {
    //     qWarning() << "相机数量不匹配!";
    //     QMessageBox::information(nullptr, "警告", "相机数量不匹配！请检查相机数量是否为3");
    //     return;
    // }

    for (int i = 0; i < vectorDeviceInfo.size(); i++) {
        try {
            std::cout << vectorDeviceInfo[i].GetSN() << std::endl;

            CGXDevicePointer devicePtr = IGXFactory::GetInstance()
                                             .OpenDeviceBySN(vectorDeviceInfo[i].GetSN(),
                                                             GX_ACCESS_EXCLUSIVE);
            CGXStreamPointer streamPtr = devicePtr->OpenStream(0);
            CGXFeatureControlPointer featureControlPtr = devicePtr->GetRemoteFeatureControl();

            // 设置触发模式
            featureControlPtr->GetEnumFeature("TriggerMode")->SetValue("On");
            featureControlPtr->GetEnumFeature("TriggerSource")->SetValue("Software");
            featureControlPtr->GetEnumFeature("ExposureAuto")->SetValue("Off");

            std::string CamerSN = std::string(vectorDeviceInfo[i].GetSN());
            if (this->m_CameraMapTable.find(CamerSN) != this->m_CameraMapTable.end()) {
                qDebug()<<"1";
                this->m_CSampleCapture
                    = new CSampleCaptureEventHandler(CamerSN, this->m_CameraMapTable[CamerSN]);
                qDebug()<<"2";
            } else {
                qWarning() << "SN码:" << QString::fromStdString(CamerSN) << "不存在";
            }

            streamPtr->RegisterCaptureCallback(this->m_CSampleCapture, NULL);
            streamPtr->StartGrab();
            featureControlPtr->GetCommandFeature("AcquisitionStart")->Execute();

            // 存储对象
            m_devices.push_back(devicePtr);
            m_streams.push_back(streamPtr);
            m_featureControls.push_back(featureControlPtr);
            m_eventHandlers.push_back(this->m_CSampleCapture);

            connect(m_eventHandlers[i],
                    &CSampleCaptureEventHandler::signals_Camer_Sent_TakePhoto,
                    this,
                    &Camera::signals_Camera_Sent_SavePicture); // 保存图片
            connect(m_eventHandlers[i],
                    &CSampleCaptureEventHandler::signals_Camer_Sent_TakePhoto,
                    this,
                    &Camera::singals_Camera_Sent_ShowPicture); // 显示图片
            connect(m_eventHandlers[i],
                    &CSampleCaptureEventHandler::signals_Camer_Sent_TakePhoto,
                    this,
                    &Camera::signals_Camera_Sent_DetectThread); // 检测图片

        } catch (const std::exception &e) {
            std::cerr << "Failed to initialize camera " << vectorDeviceInfo[i].GetSN() << ": "
                      << e.what() << std::endl;
        }
    }
}

void CSampleCaptureEventHandler::DoOnImageCaptured(CImageDataPointer &objImageDataPointer,
                                                   void *pUserParam)
{
    m_imgInformation->img.create(objImageDataPointer->GetHeight(),
                                 objImageDataPointer->GetWidth(),
                                 CV_8UC3);
    void *pRGB24Buffer = NULL;
    pRGB24Buffer = objImageDataPointer->ConvertToRGB24(GX_BIT_0_7, GX_RAW2RGB_NEIGHBOUR, true);
    memcpy(m_imgInformation->img.data,
           pRGB24Buffer,
           (objImageDataPointer->GetHeight()) * (objImageDataPointer->GetWidth()) * 3);
    if (!m_imgInformation->img.empty()) {
        cv::flip(m_imgInformation->img, m_imgInformation->img, 0); // 反转图片使其转正
        emit this->signals_Camer_Sent_TakePhoto(m_imgInformation);
        qDebug() << "相机拍照成功";
    } else {
        qDebug() << "相机拍照失败";
    }
}

void Camera::slots_Camera_Receive_Exposure(QJsonObject jsonObj)
{
    m_Camera_Exposure = jsonObj["Camera_Exposure"].toInt();
    // qDebug() << "收到的曝光时间为：" << m_Camera_Exposure;
}

void Camera::slots_Camera_Receive_TakePhoto_Mode(QString receive_mode)
{
    // 查找 receive_mode 对应的 PictureType
    if (g_pictureTypeMapping.contains(receive_mode)) {
        PictureType pictureType = g_pictureTypeMapping[receive_mode];

        // 遍历所有特征控制器
        for (int i = 0; i < m_featureControls.size(); i++) {
            // 设置模式
            m_eventHandlers[i]->setPictureType(pictureType);
            qDebug() << "模式：" << static_cast<int>(pictureType);
            // 设置相机曝光时间
            m_featureControls[i]->GetFloatFeature("ExposureTime")->SetValue(m_Camera_Exposure);
            // 执行触发命令
            m_featureControls[i]->GetCommandFeature("TriggerSoftware")->Execute();
        }
    } else {
        // 如果 receive_mode 不在映射表中，输出警告信息
        qWarning() << "未知的模式:" << receive_mode;
        // My_LOG(QLog4cplus::Level::l_ERROR, "未知的模式:"+receive_mode);
    }
}
