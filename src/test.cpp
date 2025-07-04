#include "Read_JsonFile.h"

Read_JsonFile::Read_JsonFile(QObject *parent)
    : QObject{parent}
{}

void Read_JsonFile::read_ROIJson()
{
    std::vector<cv::Rect> roi_container;

    QFile file("../JsonFiles/ROI.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开文件";
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON解析错误:" << parseError.errorString();
        return;
    }

    QJsonObject root = doc.object();
    QRegularExpression re("roi(\\d+)");
    QList<QPair<int, QString>> sortedKeys;

    for (const QString &key : root.keys()) {
        QRegularExpressionMatch match = re.match(key);
        if (match.hasMatch()) {
            bool ok;
            int num = match.captured(1).toInt(&ok);
            if (ok) {
                sortedKeys.append(qMakePair(num, key));
            } else {
                qWarning() << "无效的数字序号:" << key;
            }
        } else {
            qWarning() << "跳过未命名的键:" << key;
        }
    }

    // 按自然顺序排序
    std::sort(sortedKeys.begin(),
              sortedKeys.end(),
              [](const QPair<int, QString> &a, const QPair<int, QString> &b) {
                  return a.first < b.first;
              });

    // 处理排序后的ROI数据
    for (const auto &keyPair : sortedKeys) {
        const QString &key = keyPair.second;
        QJsonValue value = root.value(key);

        if (!value.isArray()) {
            qWarning() << "键" << key << "的值不是数组";
            continue;
        }

        QJsonArray arr = value.toArray();
        if (arr.size() != 4) {
            qWarning() << "键" << key << "的数组长度应为4, 实际为" << arr.size();
            continue;
        }

        // 提取坐标参数
        int x = arr[0].toInt(-1);
        int y = arr[1].toInt(-1);
        int width = arr[2].toInt(-1);
        int height = arr[3].toInt(-1);

        // 验证参数有效性
        if (x < 0 || y < 0 || width <= 0 || height <= 0) {
            qWarning() << "无效的ROI参数:" << key << x << y << width << height;
            continue;
        }
        // 创建并验证ROI范围
        cv::Rect roi(x, y, width, height);
        roi_container.push_back(roi);
    }
    emit this->signal_Model_SentRoiData(roi_container);
    qDebug() << "成功读取ROI_Json的数据";
}

void Read_JsonFile::read_CameraCalibrationsJson()
{
    QFile file("../JsonFiles/Camera_Calibration.json");

    cv::Mat K1, D1, R1, P1, Mapx, Mapy;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Failed to open JSON file:";
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        qCritical() << "Failed to parse JSON file";
        return;
    }

    QJsonObject jsonObj = doc.object();
    QJsonObject calibrationObj = jsonObj["camera_calibration"].toObject();

    // 读取内参矩阵 (3x3)
    QJsonArray intrinsicArray = calibrationObj["intrinsic_matrix"].toArray();
    K1 = cv::Mat(3, 3, CV_64F); // double 类型
    for (int i = 0; i < 3; ++i) {
        QJsonArray row = intrinsicArray[i].toArray();
        for (int j = 0; j < 3; ++j) {
            K1.at<double>(i, j) = row[j].toDouble();
        }
    }

    // 读取畸变系数 (1x5)
    QJsonArray distortionArray = calibrationObj["distortion_coefficients"].toArray();
    D1 = cv::Mat(1, 5, CV_64F); // double 类型
    for (int i = 0; i < 5; ++i) {
        D1.at<double>(0, i) = distortionArray[i].toDouble();
    }

    cv::initUndistortRectifyMap(K1, D1, R1, P1, cv::Size(5496, 3672), CV_32F, Mapx, Mapy);
    emit this->signal_Mdoel_SentMapData(Mapx, Mapy);
    qDebug() << "成功读取Camera_Calibration_Json的数据";
}

void Read_JsonFile::read_AlgorithmParameterJson()
{
    QJsonObject jsonObj = readJsonFile("../JsonFiles/Algorithm_Parameter.json");
    emit this->signal_Model_SentAlgorithmParameter(jsonObj);
    qDebug() << "成功读取Algorithm_Parameter_Json的数据";
}

void Read_JsonFile::read_OtherConfigJson()
{
    QJsonObject jsonObj = readJsonFile("../JsonFiles/OtherConfig.json");
    int Camera_Exposure = jsonObj["Camera_Exposure"].toInt();
    int TCP_Service = jsonObj["TCP_Service"].toInt();
    emit this->signal_Model_SentOtherConfig_CameraExposure(Camera_Exposure);
    emit this->signal_Model_SentOtherConfig_TCPService(TCP_Service);
    qDebug() << "成功读取OtheConfig_Json的数据";
}

QJsonObject Read_JsonFile::readJsonFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件:" << file.errorString();
        return QJsonObject();
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

    if (jsonDoc.isNull()) {
        qWarning() << "JSON 解析失败:" << parseError.errorString();
        return QJsonObject();
    }
    if (!jsonDoc.isObject()) {
        qWarning() << "JSON 文档不是一个对象";
        return QJsonObject();
    }
    return jsonDoc.object();
}
