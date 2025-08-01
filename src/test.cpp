#include "AlgorithmParameterView.h"
#include "ui_AlgorithmParameterView.h"

AlgorithmParameterView::AlgorithmParameterView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AlgorithmParameterView)
{
    ui->setupUi(this);

    // 控件初始化
    this->controlInit();
    // 控件样式初始化
    this->controlStyleInit();
}

AlgorithmParameterView::~AlgorithmParameterView()
{
    delete ui;
}

void AlgorithmParameterView::slots_AlgorithmParameterView_On_PushButton_SaveParame_Clecked()
{
    try {
        // 创建一个空的JsonObj
        QJsonObject jsonObject;
        // 将界面的参数读取并加入JsonObj
        jsonObject["chip_binary_low"] = this->ui->spinBox_BGA63_chip_binary_low->value();
        jsonObject["chip_binary_high"] = this->ui->spinBox_BGA63_chip_binary_high->value();
        jsonObject["buckle_binary_low"] = this->ui->spinBox_BGA63_buckle_binary_low->value();
        jsonObject["buckle_binary_high"] = this->ui->spinBox_BGA63_buckle_binary_high->value();
        jsonObject["buckle_width_low"] = this->ui->spinBox_BGA63_buckle_width_low->value();
        jsonObject["buckle_width_high"] = this->ui->spinBox_BGA63_buckle_width_high->value();
        jsonObject["buckle_height_low"] = this->ui->spinBox_BGA63_buckle_height_low->value();
        jsonObject["buckle_height_high"] = this->ui->spinBox_BGA63_buckle_height_high->value();
        jsonObject["buckle_left_low"] = this->ui->spinBox_BGA63_buckle_left_low->value();
        jsonObject["buckle_left_high"] = this->ui->spinBox_BGA63_buckle_left_high->value();
        jsonObject["buckle_right_low"] = this->ui->spinBox_BGA63_buckle_right_low->value();
        jsonObject["buckle_right_high"] = this->ui->spinBox_BGA63_buckle_right_high->value();

        emit this->signals_AlgorithmParameterView_Sent_SaveParamJson(jsonObject);

    } catch (...) {
        My_LOG(QLOG_ERROR,u8"AlgorithmParameterView::slotAlgorithmParameterView_On_PushButton_SaveParame_Clecked 异常");
    }
}

// 接收后端算法参数到前端页面进行显示
void AlgorithmParameterView::slots_AlgorithmParameterView_Receive_AlgorithmParameterModel(
    QJsonObject &JsonObj)
{
    try {
        if (true == JsonObj.empty()) {
            // json为空
            My_LOG(QLOG_ERROR, u8"算法参数界面接收到的算法Json为空。");
            return;
        } else {
            // 芯片二值化阈值low
            if (true != JsonObj.value("chip_binary_low").isDouble()) {
                My_LOG(QLOG_ERROR, "算法参数Json中未找到Key值:chip_binary_low");
            } else {
                int value = JsonObj.value("chip_binary_low").toInt();
                this->ui->spinBox_BGA63_chip_binary_low->setValue(value);
            }
            // 芯片二值化阈值high
            if (true != JsonObj.value("chip_binary_high").isDouble()) {
                My_LOG(QLOG_ERROR, "算法参数Json中未找到Key值:chip_binary_high");
            } else {
                int value = JsonObj.value("chip_binary_high").toInt();
                this->ui->spinBox_BGA63_chip_binary_high->setValue(value);
            }
            // 卡扣二值化阈值low
            if (true != JsonObj.value("buckle_binary_low").isDouble()) {
                My_LOG(QLOG_ERROR, "算法参数Json中未找到Key值:buckle_binary_low");
            } else {
                int value = JsonObj.value("buckle_binary_low").toInt();
                this->ui->spinBox_BGA63_buckle_binary_low->setValue(value);
            }
            // 卡扣二值化阈值high
            if (true != JsonObj.value("buckle_binary_high").isDouble()) {
                My_LOG(QLOG_ERROR, "算法参数Json中未找到Key值:buckle_binary_high");
            } else {
                int value = JsonObj.value("buckle_binary_high").toInt();
                this->ui->spinBox_BGA63_buckle_binary_high->setValue(value);
            }
            // 卡扣宽度阈值low
            if (true != JsonObj.value("buckle_width_low").isDouble()) {
                My_LOG(QLOG_ERROR, "算法参数Json中未找到Key值:buckle_width_low");
            } else {
                int value = JsonObj.value("buckle_width_low").toInt();
                this->ui->spinBox_BGA63_buckle_width_low->setValue(value);
            }
            // 卡扣宽度阈值high
            if (true != JsonObj.value("buckle_width_high").isDouble()) {
                My_LOG(QLOG_ERROR, "算法参数Json中未找到Key值:buckle_width_high");
            } else {
                int value = JsonObj.value("buckle_width_high").toInt();
                this->ui->spinBox_BGA63_buckle_width_high->setValue(value);
            }
            // 卡扣高度阈值low
            if (true != JsonObj.value("buckle_height_low").isDouble()) {
                My_LOG(QLOG_ERROR, "算法参数Json中未找到Key值:buckle_height_low");
            } else {
                int value = JsonObj.value("buckle_height_low").toInt();
                this->ui->spinBox_BGA63_buckle_height_low->setValue(value);
            }
            // 卡扣高度阈值high
            if (true != JsonObj.value("buckle_height_high").isDouble()) {
                My_LOG(QLOG_ERROR, "算法参数Json中未找到Key值:buckle_height_high");
            } else {
                int value = JsonObj.value("buckle_height_high").toInt();
                this->ui->spinBox_BGA63_buckle_height_high->setValue(value);
            }
            // 卡扣左边阈值low
            if (true != JsonObj.value("buckle_left_low").isDouble()) {
                My_LOG(QLOG_ERROR, "算法参数Json中未找到Key值:buckle_left_low");
            } else {
                int value = JsonObj.value("buckle_left_low").toInt();
                this->ui->spinBox_BGA63_buckle_left_low->setValue(value);
            }
            // 卡扣左边阈值high
            if (true != JsonObj.value("buckle_left_high").isDouble()) {
                My_LOG(QLOG_ERROR, "算法参数Json中未找到Key值:buckle_left_high");
            } else {
                int value = JsonObj.value("buckle_left_high").toInt();
                this->ui->spinBox_BGA63_buckle_left_high->setValue(value);
            }
            // 卡扣右边阈值low
            if (true != JsonObj.value("buckle_right_low").isDouble()) {
                My_LOG(QLOG_ERROR, "算法参数Json中未找到Key值:buckle_right_low");
            } else {
                int value = JsonObj.value("buckle_right_low").toInt();
                this->ui->spinBox_BGA63_buckle_right_low->setValue(value);
            }
            // 卡扣右边阈值high
            if (true != JsonObj.value("buckle_right_high").isDouble()) {
                My_LOG(QLOG_ERROR, "算法参数Json中未找到Key值:buckle_right_high");
            } else {
                int value = JsonObj.value("buckle_right_high").toInt();
                this->ui->spinBox_BGA63_buckle_right_high->setValue(value);
            }
        }
    } catch (...) {
        My_LOG(QLOG_ERROR, u8"异常，保存算法参数失败");
    }
}

void AlgorithmParameterView::slots_AlgorithmParameterView_Receive_BIBBoard_TypeChanged(int type_index)
{ // 通过索引获取当前选中的文本
    QString selectedText = this->ui->comboBox_BIBBoradType->itemText(type_index);
    this->m_BIBBoradTypeIndex = type_index;
    this->m_BIBBoradTypeName = selectedText;
    if (selectedText == "BGA63") {
        this->ui->stackedWidget->setCurrentWidget(this->ui->page1_BGA63);
    } else if (selectedText == "BGA154") {
        this->ui->stackedWidget->setCurrentWidget(this->ui->page2_BGA154);
    } else if (selectedText == "WSON8") {
        this->ui->stackedWidget->setCurrentWidget(this->ui->page3_WSON8);
    } else if (selectedText == "SOIC16") {
        this->ui->stackedWidget->setCurrentWidget(this->ui->page4_SOIC16);
    }
}

void AlgorithmParameterView::controlInit()
{
    this->ui->stackedWidget->setCurrentWidget(this->ui->page1_BGA63);
    this->ui->comboBox_BIBBoradType->addItem("BGA63");
    this->ui->comboBox_BIBBoradType->addItem("BGA154");
    this->ui->comboBox_BIBBoradType->addItem("WSON8");
    this->ui->comboBox_BIBBoradType->addItem("SOIC16");

    // 芯片二值化阈值
    this->ui->spinBox_BGA63_chip_binary_low->setRange(0, 255);
    this->ui->spinBox_BGA63_chip_binary_low->setSingleStep(1);
    this->ui->spinBox_BGA63_chip_binary_high->setRange(0, 255);
    this->ui->spinBox_BGA63_chip_binary_high->setSingleStep(1);
    // 卡扣二值化阈值
    this->ui->spinBox_BGA63_buckle_binary_low->setRange(0, 255);
    this->ui->spinBox_BGA63_buckle_binary_low->setSingleStep(1);
    this->ui->spinBox_BGA63_buckle_binary_high->setRange(0, 255);
    this->ui->spinBox_BGA63_buckle_binary_high->setSingleStep(1);
    // 卡扣宽度阈值
    this->ui->spinBox_BGA63_buckle_width_low->setRange(0, 1000);
    this->ui->spinBox_BGA63_buckle_width_low->setSingleStep(1);
    this->ui->spinBox_BGA63_buckle_width_high->setRange(0, 1000);
    this->ui->spinBox_BGA63_buckle_width_high->setSingleStep(1);
    // 卡扣高度阈值
    this->ui->spinBox_BGA63_buckle_height_low->setRange(0, 1000);
    this->ui->spinBox_BGA63_buckle_height_low->setSingleStep(1);
    this->ui->spinBox_BGA63_buckle_height_high->setRange(0, 1000);
    this->ui->spinBox_BGA63_buckle_height_high->setSingleStep(1);
    // 卡扣左边差值阈值最小值
    this->ui->spinBox_BGA63_buckle_left_low->setRange(0, 1000);
    this->ui->spinBox_BGA63_buckle_left_low->setSingleStep(1);
    this->ui->spinBox_BGA63_buckle_left_high->setRange(0, 1000);
    this->ui->spinBox_BGA63_buckle_left_high->setSingleStep(1);
    // 卡扣右边差值阈值最小值
    this->ui->spinBox_BGA63_buckle_right_low->setRange(0, 1000);
    this->ui->spinBox_BGA63_buckle_right_low->setSingleStep(1);
    this->ui->spinBox_BGA63_buckle_right_high->setRange(0, 1000);
    this->ui->spinBox_BGA63_buckle_right_high->setSingleStep(1);

    connect(this->ui->pushButton_SaveAlgorithmParameter,
            &QPushButton::clicked,
            this,
            &AlgorithmParameterView::slots_AlgorithmParameterView_On_PushButton_SaveParame_Clecked);
    // 接收BIB板的型号
    connect(this->ui->comboBox_BIBBoradType,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,
            &AlgorithmParameterView::slots_AlgorithmParameterView_Receive_BIBBoard_TypeChanged);
}

void AlgorithmParameterView::controlStyleInit()
{
    // QPushButton样式表设置
    QString buttonStyle = (R"(
        QPushButton {
            font-family: 'Microsoft YaHei';
            border: 2px solid #696969; /* #92bd6c; */
            color: black;
            padding: 2px 3px;
            font-size: 16px;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #E0E0E0; /* 浅灰 */
            color: black;
        }
        QPushButton:pressed {
            background-color: #696969; /* 点击时背景颜色为黑色 */
        }
    )");

    // QGroupBox样式表设置
    QString groupBoxStyle = (R"(
        QGroupBox {
            border: 2px solid #888;         /* 边框粗细和颜色 */
            border-radius: 3px;             /* 边框圆角半径 */
            margin-top: 6px;
        }
        QGroupBox::title {
            color: #000000;                 /* 标题文字颜色 */
            font-size: 14px;                /* 字体大小 */
            subcontrol-origin: margin;      /* 标题定位基准（margin/padding/content） */
            subcontrol-position: top left;  /* 标题位置 */
            padding: 0 3px;                 /* 标题内边距 */
            left: 10px;                     /* 水平偏移（相对 subcontrol-origin） */
        }
    )");
    // QComboBox样式表设置
    QString comboBoxStyle(R"(
        QComboBox {                         /* 基础样式 */
            font-family: 'Microsoft YaHei';
            background-color: #EFEEEE;      /* 背景色 */
            color: #000000;                 /* 文字颜色 */
            font-size: 15px;                /* 字号 */
            border: 1px solid #696969;      /* 边框 */
            border-radius: 4px;             /* 圆角 */
            padding: 1px;                   /* 内边距 */
        }
        QComboBox::drop-down {              /* 下拉按钮左侧 */
            subcontrol-origin: padding;     /* 定位基准 */
            subcontrol-position: right;     /* 位置 */
            width: 20px;                    /* 按钮宽度 */
            border-left: 1px solid #CCC;    /* 左侧分隔线 */
        }
        QComboBox::down-arrow {             /* 下拉按钮右侧 */
            width: 16px;
            height: 16px;
            image: url(:/Image/OtherConfigViewResources/下拉箭头.svg);  /* 自定义箭头图标 */
        }
        QComboBox QAbstractItemView {       /* 下拉列表 */
            background: #E1E1E1;            /* 下拉列表背景 */
            border: 1px solid #CCC;         /* 下拉列表边框 */
            selection-background-color: #A0A0A0;    /* 下拉列表选中项背景 */
            selection-color: white;         /* 下拉列表选中项颜色 */
            outline: 0;                     /* 去除虚线框 */
        }
        QComboBox:hover {       /* 鼠标悬停 */
            background-color: #C0C0C0;      /* 背景颜色 */
        }
    )");

    this->applyStyleToWidget(this->ui->pushButton_SaveAlgorithmParameter, buttonStyle);
    this->applyStyleToWidget(this->ui->comboBox_BIBBoradType, comboBoxStyle);

    this->applyStyleToWidget(this->ui->groupBox_BGA63_BuckleParameter, groupBoxStyle);
    this->applyStyleToWidget(this->ui->groupBox_BGA63_ChipParameter, groupBoxStyle);

    this->applyStyleToWidget(this->ui->groupBox_BGA154_BuckleParameter, groupBoxStyle);
    this->applyStyleToWidget(this->ui->groupBox_BGA154_ChipParameter, groupBoxStyle);

    this->applyStyleToWidget(this->ui->groupBox_WSON8_BuckleParameter, groupBoxStyle);
    this->applyStyleToWidget(this->ui->groupBox_WSON8_ChipParameter, groupBoxStyle);

    this->applyStyleToWidget(this->ui->groupBox_SOIC16_BuckleParameter, groupBoxStyle);
    this->applyStyleToWidget(this->ui->groupBox_SOIC16_ChipParameter, groupBoxStyle);
}

void AlgorithmParameterView::applyStyleToWidget(QWidget *widget, const QString &qssStyle)
{
    if (widget) { // 检查控件是否有效
        widget->setStyleSheet(qssStyle);
    } else {
        qDebug() << "警告：传入的控件指针为空！";
    }
}
