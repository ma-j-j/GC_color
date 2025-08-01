#include "Read_JsonFile.h"

Read_JsonFile::Read_JsonFile(QObject *parent)
    : QObject{parent}
{}

void Read_JsonFile::read_ROIJson()
{
    QString dir_path = "../../JsonFiles/"; // JSON 文件目录
    QString prefix = "ROI_";               // 文件前缀
    QString suffix = ".json";              // 文件后缀

    QDir json_dir(dir_path);
    if (!json_dir.exists()) {
        qDebug() << "JSON 文件目录不存在:" << dir_path;
        return;
    }

    // 设置文件名过滤规则（匹配 "ROI_*.json"）
    QStringList filters;
    filters << prefix + "*" + suffix;
    json_dir.setNameFilters(filters);
    QFileInfoList file_list = json_dir.entryInfoList();

    if (file_list.isEmpty()) {
        qDebug() << "未找到匹配的 ROI JSON 文件:" << dir_path + prefix + "*" + suffix;
        return;
    }

    // 定义存储结构：类型名（如 "BGA63"） -> ROI 数据
    std::map<QString, std::vector<cv::Rect>> roi_data_per_file;

    // 遍历所有匹配的文件
    for (const QFileInfo &file_info : file_list) {
        QString file_name = file_info.fileName(); // 获取文件名

        // 从文件名中提取类型名（去掉 "ROI_" 前缀和 ".json" 后缀）
        QString type_name = file_name;
        type_name.remove(0, prefix.length()); // 去掉 "ROI_"
        type_name.chop(suffix.length());      // 去掉 ".json"

        // 调用 Extract_RoiRegion 提取 ROI 数据
        std::vector<cv::Rect> current_roi_data;
        try {
            this->Extract_RoiRegion(file_info.absoluteFilePath(), current_roi_data);
        } catch (const std::exception &e) {
            std::string error = "解析 ROI 文件失败:" + file_name.toStdString() + "错误:" + e.what();
            My_LOG(QLog4cplus::Level::l_ERROR, error.c_str());
            continue; // 跳过当前文件
        }

        // 将 ROI 数据存入映射表（键为类型名）
        roi_data_per_file[type_name] = current_roi_data;
    }
    My_LOG(QLog4cplus::Level::l_INFO, u8"成功读取所有ROI_JSON文件的数据");

    // 发送映射表（类型名 -> ROI 数据）
    emit this->signals_Model_Sent_RoiData(roi_data_per_file);
}

void Read_JsonFile::read_AlgorithmParameterJson()
{
    try {
        QString dir_path = "../../JsonFiles/";  // JSON 文件目录
        QString prefix = "AlgorithmParameter_"; // 文件前缀
        QString suffix = ".json";               // 文件后缀

        QDir json_dir(dir_path);
        if (!json_dir.exists()) {
            qDebug() << "JSON 文件目录不存在:" << dir_path;
            return;
        }

        // 设置文件名过滤规则（匹配 "Algorithm_Parameter_*.json"）
        QStringList filters;
        filters << prefix + "*" + suffix;
        json_dir.setNameFilters(filters);
        QFileInfoList file_list = json_dir.entryInfoList();

        if (file_list.isEmpty()) {
            qDebug() << "未找到匹配的 Algorithm_Parameter JSON 文件:"
                     << dir_path + prefix + "*" + suffix;
            return;
        }

        // 定义存储结构：类型名（如 "BGA63"） -> ROI 数据
        std::map<QString, QJsonObject> algorithm_parameter;

        // 遍历所有匹配的文件
        for (const QFileInfo &file_info : file_list) {
            QString file_name = file_info.fileName(); // 获取文件名
            QJsonObject jsonObj;
            // 从文件名中提取类型名（去掉 "Algorithm_Parameter_" 前缀和 ".json" 后缀）
            QString type_name = file_name;
            type_name.remove(0, prefix.length()); // 去掉 "Algorithm_Parameter_"
            type_name.chop(suffix.length());      // 去掉 ".json"

            // 将 ROI 数据存入映射表（键为类型名）
            algorithm_parameter[type_name] = jsonObj;
        }
        My_LOG(QLog4cplus::Level::l_INFO, u8"成功读取所有ROI_JSON文件的数据");

        // emit this->signals_Model_Sent_AlgorithmParameter(jsonObj);
        My_LOG(QLog4cplus::Level::l_INFO, u8"成功读取算法参数Json文件的数据");
    } catch (const std::exception &e) {
        My_LOG(QLog4cplus::Level::l_ERROR, u8"解析算法参数Json文件错误");
    }
}

void Read_JsonFile::read_OtherConfigJson()
{
    try {
        QJsonObject jsonObj = readJsonFile("../../JsonFiles/OtherConfig.json");
        My_LOG(QLog4cplus::Level::l_INFO, u8"成功读取其他参数Json文件的数据");
        emit this->signals_Model_Sent_OtherConfigParameter(jsonObj);
    } catch (const std::exception &e) {
        My_LOG(QLog4cplus::Level::l_ERROR, u8"解析其他参数Json文件错误");
    }
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

void Read_JsonFile::Extract_RoiRegion(const QString &filePath, std::vector<cv::Rect> &roi_container)
{
    QFile file(filePath);
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
}
