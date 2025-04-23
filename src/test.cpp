#include "receive_signal.h"

receive::receive(QObject *parent)
    : QObject{parent}
{
    this->m_Detect = new chip_detect();
    this->m_Picutre = new DaHeng_camera();
}

// 接收ROI区域
void receive::slotReceiveROIJson(std::vector<cv::Rect> roi_data)
{
    m_ROI = roi_data;
    if (m_ROI.size() != 0) {
        qDebug() << "ROI数量:" << m_ROI.size();
    }
}

// 接收拍照图片，并进行芯片卡扣检测处理
void receive::slotReceiveTakePhoto(cv::Mat img)
{
    qDebug() << "算法检测" << *this->m_Picutre->getisSavePicutePtr();
    if (*this->m_Picutre->getisSavePicutePtr() == false) {
        m_img = img;
        if (!m_img.empty()) {
            qDebug() << "接收的图像不为空";
        }
        qDebug() << "检测算法开始";

        this->m_Detect->calibration(m_img);                     // 图像校正
        auto start = std::chrono::high_resolution_clock::now(); // 开始计时

        for (int i = 0; i < m_ROI.size(); i++) {
            cv::Mat m_ROI_IMG = m_img(m_ROI[i]);
            this->m_Detect->TF_chip(m_ROI_IMG, m_img, m_ROI[i], i + 1); // 每个ROI进行检测
            std::cout << "ROI" << i + 1 << ":finish" << std::endl;
        }
        auto end = std::chrono::high_resolution_clock::now(); // 结束计时
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Algorithm execution time: " << elapsed.count() << " seconds" << std::endl;

        QImage qimg = this->m_Detect->MatToQImage(m_img);
        emit this->signal_sent_Detect_Img(qimg);
    }
}

void receive::slotReceiveSavePicture(cv::Mat img)
{
    qDebug() << "图片保存" << *this->m_Picutre->getisSavePicutePtr();
    // 标志位为true时保存图片
    if (*this->m_Picutre->getisSavePicutePtr() == true) {
        if (!img.empty()) {
            cv::imwrite("img.bmp", img);
            qDebug() << "保存图片成功";
        }
    }
}

// 接收算法参数
void receive::slotReciveParamete(std::vector<int> &Para)
{
    this->m_Detect->get_parameter(Para);
}

//离线测试测试：测试文件图片
void receive::slotReciveFile(QString Img_file, QString Json_file)
{
    qDebug()<<"receive_fileName:"<<Img_file;
    // std::cout<<"2"<<std::endl;
    result = this->m_Detect->offline_ROItest(Img_file,Json_file);
    qDebug()<<"detect finish";
    cv::Mat img_1;
    QImage qimg_1;
    img_1 = result;
    qimg_1 = this->m_Detect->MatToQImage(img_1);
    emit this->signal_sent_detect_Image(qimg_1); //发图片
}

//设置：接收修改信息并写入json文件
void receive::slotWriteJson(QJsonObject JsonObj)
{
    // 创建 QJsonDocument 并设置 JSON 对象
    QJsonDocument jsonDoc(JsonObj);

    // 将 JSON 文档转换为格式化的字符串（可选）
    QString jsonString = jsonDoc.toJson(QJsonDocument::Indented);

    // 打开文件以写入 JSON 数据
    QFile file("test_data.json");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件进行写入:" << file.errorString();
        return;
    }

    // 写入 JSON 字符串到文件
    qint64 bytesWritten = file.write(jsonString.toUtf8());
    if (bytesWritten == -1) {
        qWarning() << "写入文件失败:" << file.errorString();
        My_LOG(QLOG_ERROR, u8"写入文件失败");
    } else if (bytesWritten != jsonString.toUtf8().size()) {
        qWarning() << "部分数据写入文件失败";
        My_LOG(QLOG_ERROR, u8"部分数据写入文件失败");
    } else {
        My_LOG(QLOG_INFO, u8"成功将 JSON 数据写入文件");
        qDebug() << "成功将 JSON 数据写入文件:" << file.fileName();
    }
    // 关闭文件
    file.close();
}
