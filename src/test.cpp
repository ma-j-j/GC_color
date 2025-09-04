#include "TitleView.h"
#include "ui_TitleView.h"

TitleView::TitleView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TitleView)
{
    ui->setupUi(this);

    // 设置标题栏最大高度为32
    this->setFixedHeight(40);
    // 隐藏标题栏
    this->setWindowFlags(Qt::FramelessWindowHint);
    // 设置窗口图标
    this->setWindowIcon(QIcon(":/Image/TitleViewResources/BW_Vision.ico"));

    // 控件样式表初始化
    ControStyleInit();

    // 连接信号和槽
    connect(this->ui->pushButton_Min,&QPushButton::clicked,this,&TitleView::slotTitleView_On_AllPushButton_Clicked);//最小化按钮点击
    connect(this->ui->pushButton_Max,&QPushButton::clicked,this,&TitleView::slotTitleView_On_AllPushButton_Clicked);//最大化按钮点击
    connect(this->ui->pushButton_Close,&QPushButton::clicked,this,&TitleView::slotTitleView_On_AllPushButton_Clicked);//关闭按钮点击
    connect(this->ui->pushButton_User,&QPushButton::clicked,this,&TitleView::slotTitleView_On_AllPushButton_Clicked);//用户按钮点击

}

TitleView::~TitleView()
{
    delete ui;
}

void TitleView::slotTitleView_On_AllPushButton_Clicked()
{

    QPushButton *pButton = qobject_cast<QPushButton *>(sender());
    QWidget *pWindow = this->window();

    if (pWindow->isTopLevel())
    {
        if(pButton == this->ui->pushButton_Min)//最小化
        {
            pWindow->showMinimized();
        }
        else if(pButton == this->ui->pushButton_Close)//关闭
        {
            // if (m_trayIcon->isVisible()) {
            //     pWindow->hide();
            //     this->m_trayIcon->showMessage(u8"30000视觉",u8"程序已缩小至系统托盘,双击可打开程序。",QSystemTrayIcon::Information,3000);
            // }
            this->close();        // 关闭主窗口
            QApplication::quit(); // 确保进程退出（如果仍有后台线程）
        }
        else if(pButton == this->ui->pushButton_Max)//最大化
        {
            // pWindow->isMaximized()?pWindow->showNormal():pWindow->showMaximized();
            if(pWindow->isMaximized())
            {
                // 开启窗口还原
                this->ui->pushButton_Max->setStyleSheet(
                    "QPushButton {"
                    "   image: url(:/Image/TitleViewResources/fullScreen.svg);"
                    "   width: 32px;"
                    "   height: 32px;"
                    "   background-color: rgb(240, 240, 240);"
                    "   border: none;"
                    "   padding: 5px;"
                    "}"
                    "QPushButton:hover {"
                    "    background-color: rgba(0, 120, 215, 0.2);"  // 鼠标悬停时背景颜色为淡蓝色
                    "}"
                    "QPushButton:pressed {"
                    "    background-color: rgba(0,120,215, 0.5);"       // 按下时背景颜色为深蓝色
                    "}"
                    );
                pWindow->showNormal();
            }
            else
            {
                // 开启窗口全屏
                this->ui->pushButton_Max->setStyleSheet(
                    "QPushButton {"
                    "   image: url(:/Image/TitleViewResources/restore.svg);"
                    "   width: 32px;"
                    "   height: 32px;"
                    "   background-color: rgb(240, 240, 240);"
                    "   border: none;"
                    "   padding: 5px;"
                    "}"
                    "QPushButton:hover {"
                    "    background-color: rgba(0, 120, 215, 0.2);"  // 鼠标悬停时背景颜色为淡蓝色
                    "}"
                    "QPushButton:pressed {"
                    "    background-color: rgba(0,120,215, 0.5);"       // 按下时背景颜色为深蓝色
                    "}"
                    );
                pWindow->showMaximized();
                emit signalTitleView_ShowAnnoMin();
            }
            emit signalTitleView_ShowAnnoMax();
        }
      else if(pButton == this->ui->pushButton_User)//用户
        {
            emit this->signalTitleView_PushButton_User_Clicked();
        }
    }
}

void TitleView::slotTitleView_On_TrayIconQuit()
{
    //先弹出窗口
    emit this->signalTitleView_ShowMainWindow();
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, u8"退出", u8"确定要退出视觉程序吗？", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        qDebug()<<"Yes";
        emit QApplication::quit();
    }
    else
    {
        qDebug()<<"No";
    }
}

void TitleView::mousePressEvent(QMouseEvent *event)
{
    // QPushButton *pButton = qobject_cast<QPushButton *>(sender());
    QWidget *pWindow = this->window();
    if (pWindow->isMaximized()) {
        // 如果窗口最大化，则不处理拖动事件
        return;
    }
    //鼠标左键按下事件
    if(event->button()==Qt::LeftButton)
    {
        // 记录鼠标左键状态
        m_leftButtonPressed = true;
        // 记录鼠标在屏幕中的位置
        m_start = event->globalPos();
    }
    QWidget::mousePressEvent(event);
}

void TitleView::mouseMoveEvent(QMouseEvent *event)
{
    if(true==this->m_leftButtonPressed)
    {
        // 将父窗体移动到父窗体原来的位置加上鼠标移动的位置
        this->window()->move(this->window()->geometry().topLeft()+event->globalPos()-this->m_start);
        // 将鼠标在屏幕中的位置替换为新的位置
        this->m_start = event->globalPos();
    }
    QWidget::mouseMoveEvent(event);
}

void TitleView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        this->m_leftButtonPressed =false;
    }
    QWidget::mouseReleaseEvent(event);
}

void TitleView::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit this->ui->pushButton_Max->clicked(true);
}

//更改样式
bool TitleView::ControStyleInit()
{
    try {
        //设置标题栏整体背景颜色
        // QPalette palette = this->palette();
        // palette.setColor(QPalette::Window, QColor(235, 235, 235));
        // this->setPalette(palette);
        // this->setAutoFillBackground(true); //确保背景被填充

        // 标题栏最左侧图标设置
        QPixmap pixmap(":/Image/TitleViewResources/BW_logo.png");
        QPixmap scaledPixmap = pixmap.scaledToHeight(40, Qt::SmoothTransformation);
        this->ui->label_BW_Icon->setPixmap(scaledPixmap);

        QPixmap pixmap2(":/Image/TitleViewResources/15000-2D.png");
        QPixmap scaledPixmap2 = pixmap2.scaledToHeight(40, Qt::SmoothTransformation);
        this->ui->label_title->setPixmap(scaledPixmap2);

        this->ui->pushButton_User->setStyleSheet(
            "QPushButton {"
            // "   image: url(:/Image/TitleViewResources/userLogin.svg);"
            "   background-color:rgb(240, 240, 240);"
            "   border: none;"
            "   padding: 5px;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(0, 120, 215, 0.2);"  // 鼠标悬停时背景颜色为淡蓝色
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(0,120,215, 0.5);"       // 按下时背景颜色为深蓝色
            "}"
            );
        this->ui->pushButton_User->setIcon(QIcon(":/Image/TitleViewResources/userLogin.svg"));
        this->ui->pushButton_User->setIconSize(QSize(38,38));

        this->ui->pushButton_Min->setStyleSheet(
            "QPushButton {"
            "   image: url(:/Image/TitleViewResources/minimize.svg);"
            "   width: 32px;"
            "   height: 32px;"
            "   background-color:rgb(240, 240, 240);"
            "   border: none;"
            "   padding: 5px;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(0, 120, 215, 0.2);"  // 鼠标悬停时背景颜色为淡蓝色
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(0,120,215, 0.5);"    // 按下时背景颜色为深蓝色
            "}"
        );

        this->ui->pushButton_Max->setStyleSheet(
            "QPushButton {"
            "   image: url(:/Image/TitleViewResources/fullScreen.svg);"
            "   width: 32px;"
            "   height: 32px;"
            "   background-color: rgb(240, 240, 240);"
            "   border: none;"
            "   padding: 5px;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(0, 120, 215, 0.2);"  // 鼠标悬停时背景颜色为淡蓝色
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(0,120,215, 0.5);"       // 按下时背景颜色为深蓝色
            "}"
            );

        this->ui->pushButton_Close->setStyleSheet(
            "QPushButton {"
            "   image: url(:/Image/TitleViewResources/close.svg);"
            "   width: 32px;"
            "   height: 32px;"
            "   background-color: rgb(240, 240, 240);"
            "   border: none;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(225,72,72, 0.7);"  // 鼠标悬停时背景颜色为淡红色
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(225,72,72, 1);"    // 按下时背景颜色为深红色
            "}"
            );

        // 添加系统托盘
        m_trayIcon = new QSystemTrayIcon(this);
        m_trayIcon->setIcon(QIcon(":/Image/TitleViewResources/BW_Vision.ico"));
        // 设置系统托盘可见
        m_trayIcon->setVisible(true);
        m_trayIcon->setToolTip(u8"30000视觉检测软件");

        QMenu *trayIconMenu = new QMenu(this);
        QAction *quitAction = trayIconMenu->addAction(u8"退出");
        //点击退出选项程序将退出
        connect(quitAction, &QAction::triggered, this, &TitleView::slotTitleView_On_TrayIconQuit);

        connect(m_trayIcon, &QSystemTrayIcon::activated, this, [&](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::DoubleClick) {
                emit this->signalTitleView_ShowMainWindow();
            }
        });
        m_trayIcon->setContextMenu(trayIconMenu);

        return true;
    } catch (...) {
        return false;
    }

}
