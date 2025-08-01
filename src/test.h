#ifndef ALGORITHMPARAMETERVIEW_H
#define ALGORITHMPARAMETERVIEW_H

#include <QWidget>
#include <QJsonObject>
#include <QObject>
#include <QMessageBox>

#include "15000_BackEnd/log4cplus/qlog4cplus.h"

namespace Ui {
class AlgorithmParameterView;
}

class AlgorithmParameterView : public QWidget
{
    Q_OBJECT

public:
    explicit AlgorithmParameterView(QWidget *parent = nullptr);
    ~AlgorithmParameterView();
public slots:
    // 保存参数按钮被点击
    void slots_AlgorithmParameterView_On_PushButton_SaveParame_Clecked();
    // 接收后端参数模型数据显示到界面
    void slots_AlgorithmParameterView_Receive_AlgorithmParameterModel(QJsonObject &);
    // 接收选择的BIB板类型
    void slots_AlgorithmParameterView_Receive_BIBBoard_TypeChanged(int);

signals:
    // 向后端发送待保存本地的参数并且让参数生效
    void signals_AlgorithmParameterView_Sent_SaveParamJson(QJsonObject &);

protected:
    // 算法参数模块按钮初始化
    void controlInit();
    // 算法参数模块按钮样式初始化
    void controlStyleInit();

private:
    Ui::AlgorithmParameterView *ui;
    // 通用样式设置函数
    void applyStyleToWidget(QWidget *widget, const QString &qssStyle);
    int m_BIBBoradTypeIndex;
    QString m_BIBBoradTypeName;
};

#endif // ALGORITHMPARAMETERVIEW_H
