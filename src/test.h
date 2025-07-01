#ifndef DISPLAYIMAGEGRAPHICSVIEW_H
#define DISPLAYIMAGEGRAPHICSVIEW_H

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

class DisplayImageGraphicsview : public QGraphicsView
{
    Q_OBJECT
public:
    DisplayImageGraphicsview(QWidget *parent = nullptr);
    ~DisplayImageGraphicsview();
public:
    // 根据图像路径加载图像到场景
    void loadFileImage(const QString &imagePath);
    // 直接传递QPixmap图像加载到场景
    void loadQPixmapImage(const QPixmap& pixmap);
    // 直接传递CV::Mat图像加载到场景
    void loadMatImage(const cv::Mat& srcImg);
    // 更新图像缩放
    void updateImageScale();
    // 将cv::Mat 转换为QPixmap
    QPixmap matToQPixmap(const cv::Mat& matImg);
protected:
    // 重写右键菜单事件
    void contextMenuEvent(QContextMenuEvent* event) override;
    // 重写鼠标滚轮事件
    void wheelEvent(QWheelEvent* event) override;
protected:

    // 重置缩放
    void resetZoom();
private:
    // 场景
    QGraphicsScene* m_Scene = nullptr;
    // 源图像
    QPixmap m_Pixmap;
    // 存储缩放比例
    qreal m_Scale;
    // 右键菜单
    QMenu* m_contextMenu = nullptr;
    QAction* m_resetZoomAction = nullptr;
};

#endif // DISPLAYIMAGEGRAPHICSVIEW_H







