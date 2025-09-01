void Read_JsonFile::read_ROIJson()
{
    try {
        QString base_dir_path = "../../Job_Files/"; // 最外层目录，比如包含 BGA63、BGA154 等子文件夹
        QString roi_file_name = "ROIs.json";        // 每个子目录中的 ROI 文件名

        QDir base_dir(base_dir_path);
        if (!base_dir.exists()) {
            QString error = "ROI_JSON 文件目录不存在:" + base_dir_path;
            LOG_ERROR_SIGNAL(error);
            return;
        }

        // 获取所有子目录（BGA63, BGA154 ...）
        QFileInfoList sub_dirs = base_dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

        if (sub_dirs.isEmpty()) {
            QString error = "未找到任何子目录:" + base_dir_path;
            LOG_ERROR_SIGNAL(error);
            return;
        }

        // 定义存储结构：类型名（如 "BGA63"） -> ROI 数据
        std::map<QString, std::vector<cv::Rect>> roi_data_per_file;

        for (const QFileInfo &sub_dir_info : sub_dirs) {
            QString type_name = sub_dir_info.fileName(); // 子目录名，如 BGA63
            // qDebug() << "type_name:" << type_name;
            QString roi_file_path = sub_dir_info.absoluteFilePath() + "/" + roi_file_name;
            QFileInfo roi_file_info(roi_file_path);

            if (!roi_file_info.exists()) {
                QString error = "子目录中未找到 ROIs.json 文件:" + roi_file_path;
                LOG_ERROR_SIGNAL(error);
                continue; // 跳过这个子目录
            }

            // 调用 Extract_RoiRegion 提取 ROI 数据
            std::vector<cv::Rect> current_roi_data;
            try {
                this->Extract_RoiRegion(roi_file_info.absoluteFilePath(), current_roi_data);
            } catch (const std::exception &e) {
                std::string error = "解析 ROI 文件失败:" + roi_file_info.filePath().toStdString()
                                    + " 错误:" + e.what();
                LOG_ERROR_SIGNAL(error);
                continue;
            }

            // 将 ROI 数据存入映射表（键为子目录名，例如 "BGA63"）
            roi_data_per_file[type_name] = current_roi_data;
        }

        // 发送映射表（类型名 -> ROI 数据）
        emit this->signals_Model_Sent_RoiData(roi_data_per_file);
    } catch (...) {
        QString error = "读取所有 ROI_JSON 文件的数据失败";
        LOG_ERROR_SIGNAL(error);
    }
}

void Read_JsonFile::read_AlgorithmParameterJson()
{
    try {
        QString base_dir_path = "../../Job_Files/"; // 最外层目录
        QString algo_file_name = "Algorithms.json"; // 每个子目录中的算法参数文件

        QDir base_dir(base_dir_path);
        if (!base_dir.exists()) {
            QString error = "算法参数 JSON 文件目录不存在:" + base_dir_path;
            LOG_ERROR_SIGNAL(error);
            return;
        }

        // 获取所有子目录（如 BGA63, BGA154 ...）
        QFileInfoList sub_dirs = base_dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        if (sub_dirs.isEmpty()) {
            QString error = "未找到任何子目录:" + base_dir_path;
            LOG_ERROR_SIGNAL(error);
            return;
        }

        // 定义存储结构：类型名（如 "BGA63"） -> 算法参数 QJsonObject
        std::map<QString, QJsonObject> algorithm_parameter_map;

        for (const QFileInfo &sub_dir_info : sub_dirs) {
            QString type_name = sub_dir_info.fileName(); // 子目录名，如 "BGA63"
            // qDebug() << "type_name:" << type_name;
            QString algo_file_path = sub_dir_info.absoluteFilePath() + "/" + algo_file_name;
            QFileInfo algo_file_info(algo_file_path);

            if (!algo_file_info.exists()) {
                QString error = "子目录中未找到 Algorithms.json 文件:" + algo_file_path;
                LOG_ERROR_SIGNAL(error);
                continue;
            }

            // 读取并解析 JSON 文件
            QJsonObject jsonObj;
            try {
                jsonObj = this->readJsonFile(algo_file_info.absoluteFilePath());
            } catch (const std::exception &e) {
                QString error = QString("解析算法参数 JSON 文件失败: %1, 错误: %2")
                                    .arg(algo_file_info.fileName())
                                    .arg(e.what());
                LOG_ERROR_SIGNAL(error);
                continue;
            }

            // 检查是否解析成功
            if (jsonObj.isEmpty()) {
                QString error = QString("算法参数 JSON 文件为空或解析失败: %1")
                                    .arg(algo_file_info.filePath());
                LOG_ERROR_SIGNAL(error);
                continue;
            }

            // 存入映射表（键为子目录名，例如 "BGA63"）
            algorithm_parameter_map[type_name] = jsonObj;
        }

        // 发送映射表：类型名 -> 算法参数 JSON 对象
        QString type_name = "";
        emit this->signals_Model_Sent_AlgorithmParameter(algorithm_parameter_map, type_name);

    } catch (const std::exception &e) {
        QString error = "解析算法参数 JSON 文件错误:" + QString(e.what());
        LOG_ERROR_SIGNAL(error);
    }
}

// 保存 ROI 信息到 Json 文件
void Save_JsonFile::SaveRoiJsonFile(const std::vector<cv::Rect> &roi_data,
                                    const int &type_index,
                                    const QString &type_name)
{
    QString dir_path = "../../Job_Files/" + type_name + "/";
    QString file_name = "ROIs.json";
    QString save_path = dir_path + file_name;

    try {
        // 确保目录存在
        QDir dir;
        if (!dir.exists(dir_path)) {
            dir.mkpath(dir_path);
        }
        this->SaveRoiRegion(roi_data, save_path);
        std::string info = (type_name + "/ROIs.json").toStdString() + " 保存成功";
        LOG_INFO_SIGNAL(info);
        QMessageBox::information(nullptr, "提示", "ROI信息保存成功！");
    } catch (const std::exception &e) {
        std::string error = (type_name + "/ROIs.json").toStdString() + " 保存失败: " + e.what();
        LOG_ERROR_SIGNAL(error);
        QMessageBox::information(nullptr, "提示", QString::fromStdString(error));
    }
}

// 保存算法信息到 Json 文件
void Save_JsonFile::SaveAlgorithmParameterJsonFile(const QJsonObject &jsonObj,
                                                   const QString &type_name)
{
    QString dir_path = "../../Job_Files/" + type_name + "/";
    QString file_name = "Algorithms.json";
    QString save_path = dir_path + file_name;

    try {
        // 确保目录存在
        QDir dir;
        if (!dir.exists(dir_path)) {
            dir.mkpath(dir_path);
        }

        // 将 QJsonObject 转换为 QJsonDocument
        QJsonDocument doc(jsonObj);

        // 写入文件
        QFile file(save_path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QString error = "无法打开算法Json文件:" + save_path;
            LOG_ERROR_SIGNAL(error);
            QMessageBox::warning(nullptr, "提示", error);
            return;
        }

        file.write(doc.toJson(QJsonDocument::Indented)); // 缩进格式美化 JSON
        file.close();

        std::string info = (type_name + "/Algorithms.json").toStdString() + " 保存成功";
        LOG_INFO_SIGNAL(info);
        QMessageBox::information(nullptr, "提示", "算法参数保存成功！");
    } catch (const std::exception &e) {
        std::string error = (type_name + "/Algorithms.json").toStdString()
                            + " 保存失败: " + e.what();
        LOG_ERROR_SIGNAL(error);
        QMessageBox::information(nullptr, "提示", QString::fromStdString(error));
    }
}
