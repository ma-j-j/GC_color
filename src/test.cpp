#include "DisplayImageGraphicsview.h"


DisplayImageGraphicsview::DisplayImageGraphicsview(QWidget *parent)
: QGraphicsView(parent)
{
    // 创建场景
    this->m_Scene = new QGraphicsScene(this);
    this->setScene(this->m_Scene);


    // // 设置抗锯齿和渲染质量
    // this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // // 设置滚动条的策略
    // this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 视觉优化设置
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 关键交互设置
    // 缩放以鼠标为中心
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    // 调整大小以鼠标为中心
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    // 拖动手势支持
    setDragMode(QGraphicsView::ScrollHandDrag);

    // 初始化右键菜单
    m_contextMenu = new QMenu(this);
    m_resetZoomAction = new QAction("重置缩放", this);
    m_contextMenu->addAction(m_resetZoomAction);

    // 连接信号槽
    connect(m_resetZoomAction, &QAction::triggered, this, &DisplayImageGraphicsview::resetZoom);
}

DisplayImageGraphicsview::~DisplayImageGraphicsview()
{
    if(nullptr != this->m_Scene)
    {
        delete this->m_Scene;
        this->m_Scene = nullptr;
    }
}

void DisplayImageGraphicsview::loadFileImage(const QString &imagePath)
{
    QPixmap pixmap(imagePath);
    // this->m_Pixmap = pixmap.copy();
    // if (!pixmap.isNull())
    // {
    //     this->m_Scene->clear();
    //     resetTransform();

    //     // 添加图片并设置场景矩形
    //     QGraphicsPixmapItem *item = this->m_Scene->addPixmap(this->m_Pixmap);
    //     this->m_Scene->setSceneRect(item->boundingRect());

    //     // 计算缩放比例
    //     qreal minVal = qMin(
    //         qreal(this->height()) / qreal(this->m_Pixmap.height()),
    //         qreal(this->width()) / qreal(this->m_Pixmap.width())
    //         );
    //     scale(minVal, minVal);

    //     // 可选：强制限制滚动范围
    //     this->fitInView(this->m_Scene->sceneRect(), Qt::KeepAspectRatio);

    // }
    if (!pixmap.isNull()) {
        m_Pixmap = pixmap;
        m_Scene->clear();
        resetTransform();

        // 使用高质量转换模式
        QGraphicsPixmapItem* item = m_Scene->addPixmap(m_Pixmap);
        item->setTransformationMode(Qt::SmoothTransformation);
        m_Scene->setSceneRect(item->boundingRect());

        // 初始适配视图（保持宽高比）
        fitInView(m_Scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void DisplayImageGraphicsview::loadQPixmapImage(const QPixmap &pixmap)
{
    // this->m_Pixmap = pixmap.copy();
    // if (!pixmap.isNull())
    // {
    //     this->m_Scene->clear();
    //     resetTransform();

    //     // 添加图片并设置场景矩形
    //     QGraphicsPixmapItem *item = this->m_Scene->addPixmap(this->m_Pixmap);
    //     this->m_Scene->setSceneRect(item->boundingRect());

    //     // 计算缩放比例
    //     qreal minVal = qMin(
    //         qreal(this->height()) / qreal(this->m_Pixmap.height()),
    //         qreal(this->width()) / qreal(this->m_Pixmap.width())
    //         );
    //     qDebug()<<"minVal:"<<minVal;
    //     this->m_Scale = minVal;
    //     scale(minVal, minVal);

    //     // 可选：强制限制滚动范围
    //     this->fitInView(this->m_Scene->sceneRect(), Qt::KeepAspectRatio);

    // }
    if (!pixmap.isNull()) {
        m_Pixmap = pixmap;
        m_Scene->clear();
        resetTransform();

        // 使用高质量转换模式
        QGraphicsPixmapItem* item = m_Scene->addPixmap(m_Pixmap);
        item->setTransformationMode(Qt::SmoothTransformation);
        m_Scene->setSceneRect(item->boundingRect());

        // 初始适配视图（保持宽高比）
        fitInView(m_Scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void DisplayImageGraphicsview::loadMatImage(const cv::Mat &srcImg)
{
    // 先将图像转换为QPixmap
    // m_Scene->clear();
    this->loadQPixmapImage(this->matToQPixmap(srcImg));
}

void DisplayImageGraphicsview::updateImageScale()
{
    if(!this->m_Pixmap.isNull())
    {
        // m_Scene->clear();
        resetTransform();

        // // 使用高质量转换模式
        // QGraphicsPixmapItem* item = m_Scene->addPixmap(m_Pixmap);
        // item->setTransformationMode(Qt::SmoothTransformation);
        // m_Scene->setSceneRect(item->boundingRect());

        // 初始适配视图（保持宽高比）
        fitInView(m_Scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void DisplayImageGraphicsview::contextMenuEvent(QContextMenuEvent *event)
{
    if (m_Pixmap.isNull()) {
        return; // 如果没有图像，不显示菜单
    }
    m_contextMenu->exec(event->globalPos()); // 显示右键菜单
}

void DisplayImageGraphicsview::wheelEvent(QWheelEvent *event)
{
    // Ctrl+滚轮缩放（兼容常规交互习惯）
    if (event->modifiers() & Qt::ControlModifier) {
        const double zoomFactor = 1.15; // 缩放步长15%
        const double angleDelta = event->angleDelta().y();

        // 计算缩放方向
        double factor = (angleDelta > 0) ? zoomFactor : 1.0 / zoomFactor;

        // 限制缩放范围（0.1倍~10倍）
        qreal currentScale = transform().m11();
        if ((factor > 1 && currentScale >= 10.0) ||
            (factor < 1 && currentScale <= 0.1)) {
            event->ignore();
            return;
        }

        scale(factor, factor);
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event); // 其他情况交给父类处理
    }
}

QPixmap DisplayImageGraphicsview::matToQPixmap(const cv::Mat &matImg)
{
    // 判断图像是否为空
    if(true == matImg.empty())
    {
        // 说明源图为空,返回一个空的QPixmap
        qDebug()<<"matImg is Null";
        QPixmap pixmap;
        return  pixmap;
    }
    cv::Mat rgbImg;
    // Opencv存储的是BGR排列，需要转换为RGB
    if(3 == matImg.channels())
    {   //三通道
        cv::cvtColor(matImg,rgbImg,cv::COLOR_BGR2RGB);
    }
    else if(4 == matImg.channels())
    {   //四通道
        cv::cvtColor(matImg,rgbImg,cv::COLOR_BGRA2RGBA);
    }
    else
    {   //单通道
        rgbImg = matImg.clone();
    }
    // 创建 QImage，使用 Mat 的数据指针
    QImage image((const unsigned char*)rgbImg.data, rgbImg.cols, rgbImg.rows, rgbImg.step,
                 matImg.channels() == 1 ? QImage::Format_Grayscale8 : QImage::Format_RGB888);
    // 将 QImage 转换为 QPixmap
    return QPixmap::fromImage(image.copy());
}

void DisplayImageGraphicsview::resetZoom()
{
    if (m_Pixmap.isNull()) {
        return; // 如果没有图像，不执行操作
    }

    // 重置变换矩阵
    resetTransform();

    // 重新适应视图（保持宽高比）
    fitInView(m_Scene->sceneRect(), Qt::KeepAspectRatio);
}

















