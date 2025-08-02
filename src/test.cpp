void MainWindowModel::slots_MainWindowModel_Receive_AlgorithmDeteceResult(
    Detect_Result &detect_result)
{
    this->m_DetectResultVector.push_back(detect_result);
    // 多少个相机就有多少个容器
    if (this->m_DetectResultVector.size() == 1) {
        for (int i = 0; i < this->m_DetectResultVector.size(); ++i) {
            const Detect_Result &detectResult = m_DetectResultVector[i];

            for (int j = 0; j < detectResult.socket_error.size(); ++j) {
                const QString &error = detectResult.socket_error[j];
                // 如果需要转换为 std::string 输出：
                // std::cout << "Detect_Result["<< i << "].socket_error["<< j << "]:"
                //           << error.toStdString() << std::endl;
                // 检查是否包含 "NG:"（表示有损坏）
                if (error.contains("NG:")) {
                    My_LOG(QLog4cplus::Level::l_ERROR, error.toUtf8().constData());
                    m_NG.push_back(error); // 插入到 m_NG 容器
                }
            }
        }

        if (this->m_NG.empty()) { // 如果所有 socket_error 都为空，发出OK信号
            emit this->signals_MainWindowModel_Sent_DetectResult_IsTrue();
            My_LOG(QLog4cplus::Level::l_INFO, u8"本次Socket没有错误");
        } else { // 否则，将报错信息
            emit this->signals_MainWindowModel_Sent_DetectResult_IsFalse(m_NG);
        }

        // 发检测图到前端显示
        std::vector<cv::Mat> mats;
        for (const auto &result : this->m_DetectResultVector) {
            mats.push_back(result.result_img);
        }
        // 拼图
        cv::Mat m_DetectResultImage;
        cv::hconcat(mats, m_DetectResultImage);
        emit this->signals_MainWindowModel_Sent_DetectResultPicture(m_DetectResultImage);

        // 容器清空
        this->m_DetectResultVector.clear();
        this->m_NG.clear();
        mats.clear();
    }
}
