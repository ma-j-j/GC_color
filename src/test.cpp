#include "Save_JsonFile.h"

Save_JsonFile::Save_JsonFile(QWidget *parent)
    : QWidget(parent)
{}

void Save_JsonFile::SaveAlgorithmParameterJsonFile(QJsonObject jsonObj)
{
    // 将QJsonObject转换为QJsonDocument
    QJsonDocument doc(jsonObj);

    // 写入文件
    QFile file("../JsonFiles/Algorithm_Parameter.json");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not open Algorithm file for writing:";
    }

    file.write(doc.toJson(QJsonDocument::Indented)); // 使用缩进格式美化JSON
    file.close();
    QMessageBox::information(nullptr, "提示", "算法信息保存成功！");
}

void Save_JsonFile::SaveOtherConfigJsonFiel(QJsonObject jsonObj)
{
    // 将QJsonObject转换为QJsonDocument
    QJsonDocument doc(jsonObj);

    // 写入文件
    QFile file("../JsonFiles/OtherConfig.json");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not open Algorithm file for writing:";
    }

    file.write(doc.toJson(QJsonDocument::Indented)); // 使用缩进格式美化JSON
    file.close();
    QMessageBox::information(nullptr, "提示", "算法信息保存成功！");
}

// 保存ROI信息到Json文件
void Save_JsonFile::SaveRoiJsonFile(std::vector<cv::Rect> roi_data)
{
    // 创建原始 JSON 对象
    QJsonObject roiJsonObj;

    // 填充数据（假设 m_rect 已正确填充）
    for (int i = 0; i < roi_data.size(); ++i) {
        const cv::Rect &rect = roi_data[i];
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

    QFile file("../JsonFiles/ROI.json");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件进行写入!";
        return;
    }

    QTextStream out(&file);
    out << jsonString;
    file.close();
    QMessageBox::information(nullptr, "提示", "ROI信息保存成功！");
}
