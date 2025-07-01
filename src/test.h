#ifndef RECTANGULARCALIBRATIONGRAPHICSVIEW_H
#define RECTANGULARCALIBRATIONGRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QMenu>
#include <QDebug>
#include <QAction>
#include <QList>
#include <QGraphicsPolygonItem>

#include "opencv2/opencv.hpp"

// QX-800矩形标定视图模块
class RectangularCalibrationGraphicsview : public QGraphicsView
{
    Q_OBJECT
public:
    RectangularCalibrationGraphicsview(QWidget *parent = nullptr);
    ~RectangularCalibrationGraphicsview();
public:
    // 根据图像路径加载图像到场景
    void loadFileImage(const QString &imagePath);
    // 直接传递QPixmap图像加载到场景
    void loadQPixmapImage(const QPixmap& pixmap);
    // 直接传递CV::Mat图像加载到场景
    void loadMatImage(const cv::Mat& srcImg);
    // 获取加载图像的尺寸
    QSize getImageSize();
    // 更新图像缩放
    void updateImageScale();
    // 场景清除
    void clear();
public slots:
    // 接收图像路径加载图像到场景
    void slotLoadFileImage(const QString &imagePath);
    // 直接传递QPixmap图像加载到场景
    void slotLoadQPixmapImage(const QPixmap& pixmap);
    // 直接传递CV::Mat图像加载到场景
    void slotLoadMatImage(const cv::Mat& srcImg);
    // 接收自动生成的标定区域
    void slotReciveQRectFRoi(QVector<QRectF>);
signals:
    // 发送绘制完成的标定ROI信息
    void signalSendROI(QRectF);
    void signalSend_RectangleInformation(cv::Rect);

protected:
    // 将cv::Mat 转换为QPixmap
    QPixmap matToQPixmap(const cv::Mat& matImg);
protected:
    // 鼠标点击事件
    void mousePressEvent(QMouseEvent *event) override;
    // 鼠标按下移动
    void mouseMoveEvent(QMouseEvent *event) override;
    // 鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    // 场景
    QGraphicsScene* m_Scene = nullptr;
    // 源图像
    QPixmap m_Pixmap;
    // 起始坐标和结束坐标
    QPointF m_StartPoint,m_EndPoint;
    // 正在绘制标识符
    bool m_FRectDrawing;
    // 拖拽模式标识符
    bool m_FRectDragging;
    // 存储拖拽原始点击位置
    QPointF m_DraggingStartPoint;
    // 矩形图元
    QGraphicsRectItem* m_CurrentRect = nullptr;
    // 存储矩形区域
    QRectF m_RectF;
    // 存储矩形区域
    cv::Rect m_Rect;
    // 存储缩放比例
    qreal m_Scale;
};

#endif // RECTANGULARCALIBRATIONGRAPHICSVIEW_H
