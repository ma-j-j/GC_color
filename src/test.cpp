#include "RectangularCalibrationGraphicsview.h"


RectangularCalibrationGraphicsview::RectangularCalibrationGraphicsview(QWidget *parent)
: QGraphicsView(parent)
{
    // 创建场景
    this->m_Scene = new QGraphicsScene(this);
    this->setScene(this->m_Scene);


    // 设置抗锯齿和渲染质量
    this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // 设置滚动条的策略
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

RectangularCalibrationGraphicsview::~RectangularCalibrationGraphicsview()
{
    if(nullptr != this->m_Scene)
    {
        delete this->m_Scene;
        this->m_Scene = nullptr;
    }
}

void RectangularCalibrationGraphicsview::loadFileImage(const QString &imagePath)
{
    QPixmap pixmap(imagePath);
    this->m_Pixmap = pixmap.copy();
    if (!pixmap.isNull())
    {
        this->m_Scene->clear();
        this->m_CurrentRect = nullptr;
        resetTransform();

        // 添加图片并设置场景矩形
        QGraphicsPixmapItem *item = this->m_Scene->addPixmap(this->m_Pixmap);
        this->m_Scene->setSceneRect(item->boundingRect());

        // 计算缩放比例
        qreal minVal = qMin(
            qreal(this->height()) / qreal(this->m_Pixmap.height()),
            qreal(this->width()) / qreal(this->m_Pixmap.width())
            );
        scale(minVal, minVal);

        // 可选：强制限制滚动范围
        this->fitInView(this->m_Scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void RectangularCalibrationGraphicsview::loadQPixmapImage(const QPixmap &pixmap)
{
    this->m_Pixmap = pixmap.copy();
    if (!pixmap.isNull())
    {
        this->m_Scene->clear();
        this->m_CurrentRect = nullptr;
        resetTransform();

        // 添加图片并设置场景矩形
        QGraphicsPixmapItem *item = this->m_Scene->addPixmap(this->m_Pixmap);
        this->m_Scene->setSceneRect(item->boundingRect());

        // 计算缩放比例
        qreal minVal = qMin(
            qreal(this->height()) / qreal(this->m_Pixmap.height()),
            qreal(this->width()) / qreal(this->m_Pixmap.width())
            );
        // qDebug()<<"minVal:"<<minVal;
        this->m_Scale = minVal;
        scale(minVal, minVal);

        // 可选：强制限制滚动范围
        this->fitInView(this->m_Scene->sceneRect(), Qt::KeepAspectRatio);

    }
}

void RectangularCalibrationGraphicsview::loadMatImage(const cv::Mat &srcImg)
{
    // 先将图像转换为QPixmap
    this->loadQPixmapImage(this->matToQPixmap(srcImg));
}

QSize RectangularCalibrationGraphicsview::getImageSize()
{
    return this->m_Pixmap.size();
}

void RectangularCalibrationGraphicsview::updateImageScale()
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

void RectangularCalibrationGraphicsview::clear()
{
    this->m_Scene->clear();
}

void RectangularCalibrationGraphicsview::slotLoadFileImage(const QString &imagePath)
{
    QPixmap pixmap(imagePath);
    this->m_Pixmap = pixmap.copy();
    if (!pixmap.isNull())
    {
        this->m_Scene->clear();
        this->m_CurrentRect = nullptr;
        resetTransform();

        // 添加图片并设置场景矩形
        QGraphicsPixmapItem *item = this->m_Scene->addPixmap(this->m_Pixmap);
        this->m_Scene->setSceneRect(item->boundingRect());

        // 计算缩放比例
        qreal minVal = qMin(
            qreal(this->height()) / qreal(this->m_Pixmap.height()),
            qreal(this->width()) / qreal(this->m_Pixmap.width())
            );
        scale(minVal, minVal);

        // 可选：强制限制滚动范围
        this->fitInView(this->m_Scene->sceneRect(), Qt::KeepAspectRatio);
    } else {
        qDebug() << "pixmap isnull";
    }
}

void RectangularCalibrationGraphicsview::slotLoadQPixmapImage(const QPixmap &pixmap)
{
    this->m_Pixmap = pixmap.copy();
    if (!pixmap.isNull())
    {
        this->m_Scene->clear();
        this->m_CurrentRect = nullptr;
        resetTransform();

        // 添加图片并设置场景矩形
        QGraphicsPixmapItem *item = this->m_Scene->addPixmap(this->m_Pixmap);
        this->m_Scene->setSceneRect(item->boundingRect());

        // 计算缩放比例
        qreal minVal = qMin(
            qreal(this->height()) / qreal(this->m_Pixmap.height()),
            qreal(this->width()) / qreal(this->m_Pixmap.width())
            );
        qDebug()<<"minVal:"<<minVal;
        this->m_Scale = minVal;
        scale(minVal, minVal);

        // 可选：强制限制滚动范围
        this->fitInView(this->m_Scene->sceneRect(), Qt::KeepAspectRatio);

    }
}

void RectangularCalibrationGraphicsview::slotLoadMatImage(const cv::Mat &srcImg)
{
    // 先将图像转换为QPixmap
    this->loadQPixmapImage(this->matToQPixmap(srcImg));
}

void RectangularCalibrationGraphicsview::slotReciveQRectFRoi(QVector<QRectF> V_rectRoi)
{
    // 清空所有的矩形框
    QList<QGraphicsItem*> allItems = this->m_Scene->items();

    // 筛选出所有QGraphicsRectItem
    for (QGraphicsItem* item : allItems)
    {
        if (qgraphicsitem_cast<QGraphicsRectItem*>(item)) {
            this->m_Scene->removeItem(item);
        }
    }
    this->m_CurrentRect = nullptr;

    int minSize = qMin(this->m_Pixmap.width()/600, this->m_Pixmap.height()/600);
    if(minSize < 1)
        minSize = 1;
    for(int i = 0; i< V_rectRoi.size(); ++i)
    {
        QGraphicsRectItem * rectItem = new QGraphicsRectItem(V_rectRoi[i]);
        rectItem->setPen(QPen(QColor("#00BFFF"), minSize, Qt::DashDotLine));
        this->m_Scene->addItem(rectItem);
    }
}

QPixmap RectangularCalibrationGraphicsview::matToQPixmap(const cv::Mat &matImg)
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

void RectangularCalibrationGraphicsview::mousePressEvent(QMouseEvent *event)
{
    // 必须要已经加载图片
    if(true == this->m_Pixmap.isNull())
    {
        // 调用基类mousePressEvent函数，保留鼠标释放事件处理逻辑
        QGraphicsView::mousePressEvent(event);
        return ;
    }
    QPointF clickPoint = mapToScene(event->pos());
    // 左键点击则开始绘制矩形
    if(Qt::LeftButton == event->button() && m_CurrentRect != nullptr && m_CurrentRect->rect().contains(clickPoint))
    {
        // 开启拖拽模式
        this->m_FRectDragging = true;
        // 存储拖拽起始点
        this->m_DraggingStartPoint = clickPoint;
    }
    else if(Qt::LeftButton == event->button())
    {
        qreal startX = clickPoint.x();
        qreal startY = clickPoint.y();

        if( 0 < startX && startX < m_Pixmap.width() && 0 < startY && startY < m_Pixmap.height())
        {
            // 说明在图像内部绘制
            this->m_FRectDrawing = true;
            this->m_StartPoint = clickPoint;
        }
        else
        {
            // 图像外面，无效的绘制区域
            qDebug()<<"图像外面,无效的绘制区域";
        }
    }

    // 调用基类mousePressEvent函数，保留鼠标释放事件处理逻辑
    QGraphicsView::mousePressEvent(event);
}

void RectangularCalibrationGraphicsview::mouseMoveEvent(QMouseEvent *event)
{
    // 必须要已经加载图片
    if(true == this->m_Pixmap.isNull())
    {
        // 调用基类mouseMoveEvent函数，保留鼠标释放事件处理逻辑
        QGraphicsView::mouseMoveEvent(event);
        return ;
    }
    QPointF clickPoint = mapToScene(event->pos());
    if(true == this->m_FRectDragging)
    {
        // 表示拖动矩形
        QPointF delta = clickPoint - m_DraggingStartPoint;
        QRectF newRect = m_CurrentRect->rect().translated(delta);
        // 获取图像在界面上的像素长宽，而不是自身真实的的长宽
        int maxWidth = this->m_Pixmap.width();
        int maxHeight = this->m_Pixmap.height();
        if(newRect.x()<=0)
        {
            newRect = m_CurrentRect->rect();
        }
        if(newRect.x()+newRect.width()>=maxWidth)
        {
            newRect = m_CurrentRect->rect();
        }
        if(newRect.y()<=0)
        {
            newRect = m_CurrentRect->rect();
        }
        if(newRect.y()+newRect.height()>=maxHeight)
        {
            newRect = m_CurrentRect->rect();
        }

        m_CurrentRect->setRect(newRect);
        m_DraggingStartPoint = clickPoint; // 更新起点以持续拖动
    }
    else if(true == this->m_FRectDrawing)
    {
        if(nullptr != this->m_CurrentRect)
        {
            this->m_Scene->removeItem(this->m_CurrentRect);
            delete this->m_CurrentRect;
            this->m_CurrentRect = nullptr;
        }

        this->m_CurrentRect = new QGraphicsRectItem();
        this->m_CurrentRect->setRect(QRectF(this->m_StartPoint, QSizeF(0,0)));
        int minSize = qMin(this->m_Pixmap.width()/600, this->m_Pixmap.height()/600);
        if(minSize < 1)
            minSize = 1;
        this->m_CurrentRect->setPen(QPen(Qt::red, minSize, Qt::DashDotLine));
        this->m_Scene->addItem(this->m_CurrentRect);

        int moveX = clickPoint.x();
        int moveY = clickPoint.y();
        if(moveX <= 0)
        {
            moveX = 1;
        }
        if(moveX >= this->m_Pixmap.width())
        {
            moveX = this->m_Pixmap.width() - 1;
        }
        if(moveY <= 0)
        {
            moveY = 1;
        }
        if(moveY >= this->m_Pixmap.height())
        {
            moveY = this->m_Pixmap.height() - 1;
        }
        this->m_EndPoint = QPoint(moveX,moveY);

        QPointF rectTopLeft = QPointF(std::min(m_StartPoint.x(), m_EndPoint.x()), std::min(m_StartPoint.y(), m_EndPoint.y()));
        QSizeF rectSize = QSizeF(std::abs(m_StartPoint.x() - m_EndPoint.x()), std::abs(m_StartPoint.y() - m_EndPoint.y()));
        this->m_RectF = QRectF(rectTopLeft, rectSize);
        m_Rect = cv::Rect(static_cast<int>(m_RectF.x() + 0.5),     // x 坐标四舍五入
                          static_cast<int>(m_RectF.y() + 0.5),     // y 坐标四舍五入
                          static_cast<int>(m_RectF.width() + 0.5), // 宽度四舍五入（注意可能为负数）
                          static_cast<int>(m_RectF.height() + 0.5) // 高度四舍五入（注意可能为负数）
        );

        m_CurrentRect->setRect(this->m_RectF);
    }

    // 调用基类mouseMoveEvent函数，保留鼠标释放事件处理逻辑
    QGraphicsView::mouseMoveEvent(event);
}

void RectangularCalibrationGraphicsview::mouseReleaseEvent(QMouseEvent *event)
{
    // 必须要已经加载图片
    if(true == this->m_Pixmap.isNull())
    {
        // 调用基类mouseReleaseEvent函数，保留鼠标释放事件处理逻辑
        QGraphicsView::mouseReleaseEvent(event);
        return ;
    }
    QPointF clickPoint = mapToScene(event->pos());
    this->m_EndPoint = clickPoint;
    if(true == this->m_FRectDragging)
    {
        // 拖拽流程完成,恢复拖拽标识符
        this->m_FRectDragging = false;
        // 发送标定信息
        emit this->signalSendROI(this->m_CurrentRect->rect());
        emit this->signalSend_RectangleInformation(m_Rect);
    }
    else if(true == this->m_FRectDrawing && this->m_StartPoint != this->m_EndPoint)
    {
        // 绘制流程完成,恢复绘制标识符
        this->m_FRectDrawing = false;
        // 发送标定信息
        emit this->signalSendROI(this->m_CurrentRect->rect());
        emit this->signalSend_RectangleInformation(m_Rect);
    }

    // 调用基类mouseReleaseEvent函数，保留鼠标释放事件处理逻辑
    QGraphicsView::mouseReleaseEvent(event);
}
