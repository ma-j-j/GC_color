#include "Save_JsonFile.h"

Save_JsonFile::Save_JsonFile(QWidget *parent)
    : QWidget(parent)
{}

void Save_JsonFile::SaveAlgorithmParameterJsonFile(const QJsonObject &jsonObj,
                                                   const QString &type_name)
{
    QString file_name = "AlgorithmParameter_" + type_name + ".json";

    try {
        QString save_path = "../../JsonFiles/AlgorithmParameter/" + file_name;

        // 将QJsonObject转换为QJsonDocument
        QJsonDocument doc(jsonObj);

        // 写入文件
        QFile file(save_path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Could not open Algorithm file for writing:";
        }

        file.write(doc.toJson(QJsonDocument::Indented)); // 使用缩进格式美化JSON
        file.close();
        std::string info = file_name.toStdString() + "保存成功";
        My_LOG(QLog4cplus::Level::l_INFO, info.c_str());
        QMessageBox::information(nullptr, "提示", "算法参数保存成功！");
    } catch (const std::exception &e) {
        std::string error = file_name.toStdString() + "保存失败";
        My_LOG(QLog4cplus::Level::l_ERROR, error.c_str());
        QMessageBox::information(nullptr, "提示", QString::fromStdString(error));
    }
}

void Save_JsonFile::SaveOtherConfigJsonFiel(const QJsonObject &jsonObj)
{
    try {
        // 将QJsonObject转换为QJsonDocument
        QJsonDocument doc(jsonObj);

        // 写入文件
        QFile file("../../JsonFiles/OtherConfig.json");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Could not open Algorithm file for writing:";
        }

        file.write(doc.toJson(QJsonDocument::Indented)); // 使用缩进格式美化JSON
        file.close();
        My_LOG(QLog4cplus::Level::l_INFO, u8"OtherConfig.Json保存成功！");
        QMessageBox::information(nullptr, "提示", "其他信息保存成功！");
    } catch (const std::exception &e) {
        My_LOG(QLog4cplus::Level::l_ERROR, u8"保存其他参数Json文件失败");
        QMessageBox::information(nullptr, "提示", "保存其他参数Json文件失败");
    }
}

// 保存ROI信息到Json文件
void Save_JsonFile::SaveRoiJsonFile(const std::vector<cv::Rect> &roi_data,
                                    const int &type_index,
                                    const QString &type_name)
{
    QString file_name = "ROI_" + type_name + ".json";
    try {
        QString save_path = "../../JsonFiles/ROI/" + file_name;

        this->SaveRoiRegion(roi_data, save_path);
        std::string info = file_name.toStdString() + "保存成功";
        My_LOG(QLog4cplus::Level::l_INFO, info.c_str());
        QMessageBox::information(nullptr, "提示", "ROI信息保存成功！");
    } catch (const std::exception &e) {
        std::string error = file_name.toStdString() + "保存失败";
        My_LOG(QLog4cplus::Level::l_ERROR, error.c_str());
        QMessageBox::information(nullptr, "提示", QString::fromStdString(error));
    }
}

void Save_JsonFile::SaveRoiRegion(const std::vector<cv::Rect> &roi_region, const QString &file_name)
{
    // 创建原始 JSON 对象
    QJsonObject roiJsonObj;

    // 填充数据（假设 m_rect 已正确填充）
    for (int i = 0; i < roi_region.size(); ++i) {
        const cv::Rect &rect = roi_region[i];
        QJsonArray rectArray;
        rectArray.append(rect.x);
        rectArray.append(rect.y);
        rectArray.append(rect.width);
        rectArray.append(rect.height);
        roiJsonObj[QString("roi%1").arg(i + 1)] = rectArray; // 键为 "roi1", "roi2", ...
    }

    // Step 1: 提取键并转换为整数
    QList<int> keys;
    for (auto it = roiJsonObj.begin(); it != roiJsonObj.end(); ++it) {
        QString keyStr = it.key();
        bool ok;
        int key = keyStr.mid(3).toInt(&ok); // 去掉前缀 "roi" 并转换为整数
        if (ok) {
            keys.append(key);
        } else {
            qWarning() << "无效的键格式:" << keyStr;
        }
    }
    // Step 2: 按整数排序键
    std::sort(keys.begin(), keys.end());

    // Step 3: 按排序后的键重建 JSON 对象
    QJsonObject sortedRoiJsonObj;
    for (int key : keys) {
        QString sortedKey = QString("roi%1").arg(key);
        if (roiJsonObj.contains(sortedKey)) {
            sortedRoiJsonObj.insert(sortedKey, roiJsonObj.value(sortedKey));
        }
    }

    // 序列化并保存到文件
    QJsonDocument jsonDoc(sortedRoiJsonObj);
    QString jsonString = jsonDoc.toJson(QJsonDocument::Indented);

    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件进行写入!";
        return;
    }

    QTextStream out(&file);
    out << jsonString;
    file.close();
}
