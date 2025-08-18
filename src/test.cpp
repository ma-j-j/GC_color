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


void MainWindowModel::slots_MainWindowModel_Receive_AlgorithmParameter_UpDate(
    std::map<QString, QJsonObject> algorithmParams, QString type_name)
{
    // 同步修改的算法参数
    m_Algorithm_Parameter_Map = algorithmParams;

    // 在映射表中查找对应的 AlgorithmParamete 数据
    auto algorithmparams = m_Algorithm_Parameter_Map.find(type_name); // 查找键为 type_name 的条目

    if (algorithmparams != m_Algorithm_Parameter_Map.end()) {
        m_Algorithm_Parameter_QJsonObj
            = algorithmparams->second; // algorithmparams->second 是对应的 std::vector<cv::Rect>
        this->QJsonObject_Algorithm(m_Algorithm_Parameter_QJsonObj, m_Algorithm_Parameter);
    }
    if (type_name != "") //如果类型名不为空，则保存Json文件
    {
        // qDebug() << "类型为," << type_name << "是保存算法文件信号";
        this->m_SaveJsonFiles->SaveAlgorithmParameterJsonFile(m_Algorithm_Parameter_QJsonObj,
                                                              type_name);
    } else {
        // qDebug() << "类型为空，不是保存算法文件信号";
    }

    // 初始化离线测试算法参数
    // 默认离线检测算法为"BGA63"
    int init_index = 0;
    QString init_offline_type_name = "BGA63";
    this->QJsonObject_Algorithm(m_Algorithm_Parameter_Map[init_offline_type_name],
                                m_OffLineTest_AlgorithmParameter);
    this->m_OffLineTest->OffLineTest_BIBBoardType(init_index, init_offline_type_name);
    // qDebug() << "算法参数信息已更新";
}
