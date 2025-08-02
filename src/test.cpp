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
        if (this->m_BIBBoradTypeName == "BGA63") {
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

            // emit this->signals_AlgorithmParameterView_Sent_SaveParamJson(jsonObject);
        }
    } catch (...) {
        My_LOG(QLOG_ERROR,u8"AlgorithmParameterView::slotAlgorithmParameterView_On_PushButton_SaveParame_Clecked 异常");
    }
}

// 接收后端算法参数到前端页面进行显示
void AlgorithmParameterView::slots_AlgorithmParameterView_Receive_AlgorithmParameterModel(
    std::map<QString, QJsonObject> &algorithmParams, QString &type_name)
{
    for (const auto &boardPair : algorithmParams) {
        const QString &boardType = boardPair.first;      // 如 "BGA63"
        const QJsonObject &paramJson = boardPair.second; // 如 {"chip_binary_low":10, ...}

        // 查找该板型是否在映射表中
        auto it = boardTypeToControls.find(boardType);
        if (it == boardTypeToControls.end()) {
            // My_LOG(QLOG_WARN, QString("未找到板型 '%1' 的控件映射").arg(boardType));
            continue;
        }

        BoardControlMap &controlMap = it->second;

        // 切换到对应页面
        this->ui->stackedWidget->setCurrentWidget(controlMap.page);

        // 遍历该板型的所有参数，并尝试设置到对应的 spinBox 上
        for (auto jsonIt = paramJson.begin(); jsonIt != paramJson.end(); ++jsonIt) {
            const QString &paramName = jsonIt.key(); // 如 "chip_binary_low"
            QJsonValue value = jsonIt.value();

            if (!value.isDouble()) {
                // My_LOG(QLOG_WARN, QString("参数 '%1' 不是数字类型").arg(paramName));
                continue;
            }

            int intValue = value.toInt(); // QSpinBox 需要 int

            // 查找该参数名对应的控件
            auto spinBoxIt = controlMap.spinBoxMap.find(paramName);
            if (spinBoxIt != controlMap.spinBoxMap.end() && spinBoxIt->second) {
                spinBoxIt->second->setValue(intValue); // 设置值
            } else {
                // My_LOG(QLOG_WARN, QString("未找到控件: 参数 '%1' 对应的 SpinBox").arg(paramName));
            }
        }
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

    this->QSpinBoxInit();  // QSpinBox 初始化
    this->BoardTypeInit(); // 映射表初始化

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

void AlgorithmParameterView::QSpinBoxInit()
{
    // 初始化QSpinBox
    QList<QSpinBox *> spinBoxes = this->findChildren<QSpinBox *>();
    for (QSpinBox *spinBox : spinBoxes) {
        QString name = spinBox->objectName();

        // 判断是否是你要初始化的控件，比如名称中包含 "chip_binary_low" 或 "chip_binary_high"
        if (name.contains("chip_binary_low") || name.contains("chip_binary_high")
            || name.contains("buckle_binary_low") || name.contains("buckle_binary_high")) {
            spinBox->setRange(0, 255);
            spinBox->setSingleStep(1);
            qDebug() << "已初始化:" << name;
        }

        if (name.contains("buckle_width_low") || name.contains("buckle_width_high")
            || name.contains("buckle_height_low") || name.contains("buckle_height_high")) {
            spinBox->setRange(0, 1000);
            spinBox->setSingleStep(1);
            qDebug() << "已初始化:" << name;
        }
    }
}

void AlgorithmParameterView::BoardTypeInit()
{
    // 构建 BGA63 的控件映射
    BoardControlMap bga63Map;
    bga63Map.page = this->ui->page1_BGA63;

    bga63Map.spinBoxMap["chip_binary_low"] = this->ui->spinBox_BGA63_chip_binary_low;
    bga63Map.spinBoxMap["chip_binary_high"] = this->ui->spinBox_BGA63_chip_binary_high;
    bga63Map.spinBoxMap["buckle_binary_low"] = this->ui->spinBox_BGA63_buckle_binary_low;
    bga63Map.spinBoxMap["buckle_binary_high"] = this->ui->spinBox_BGA63_buckle_binary_high;
    bga63Map.spinBoxMap["buckle_height_low"] = this->ui->spinBox_BGA63_buckle_height_low;
    bga63Map.spinBoxMap["buckle_height_high"] = this->ui->spinBox_BGA63_buckle_height_high;
    bga63Map.spinBoxMap["buckle_left_low"] = this->ui->spinBox_BGA63_buckle_left_low;
    bga63Map.spinBoxMap["buckle_left_high"] = this->ui->spinBox_BGA63_buckle_left_high;
    bga63Map.spinBoxMap["buckle_right_low"] = this->ui->spinBox_BGA63_buckle_right_low;
    bga63Map.spinBoxMap["buckle_right_high"] = this->ui->spinBox_BGA63_buckle_right_high;
    bga63Map.spinBoxMap["buckle_width_low"] = this->ui->spinBox_BGA63_buckle_width_low;
    bga63Map.spinBoxMap["buckle_width_high"] = this->ui->spinBox_BGA63_buckle_width_high;

    // 构建 BGA154 的控件映射
    BoardControlMap bga154Map;
    bga154Map.page = this->ui->page2_BGA154;
    bga154Map.spinBoxMap["chip_binary_low"] = this->ui->spinBox_BGA154_chip_binary_low;
    bga154Map.spinBoxMap["chip_binary_high"] = this->ui->spinBox_BGA154_chip_binary_high;
    // ... 添加 BGA154 的其它控件

    // 构建 WSON8 的控件映射

    // 构建 SOIC16 的控件映射

    // 将板型与控件映射表存入全局 map
    boardTypeToControls["BGA63"] = bga63Map;
    boardTypeToControls["BGA154"] = bga154Map;
    // 继续添加 WSON8, SOIC16...
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
