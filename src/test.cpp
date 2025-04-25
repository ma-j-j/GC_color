#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QJsonDocument>
#include<iostream>

#include "QX-800_Model/Img_detect.h"
#include "QX-800_Model/daheng_camera.h"
#include "QX-800_Model/read_json.h"
#include "QX-800_Model/receive_signal.h"
#include "QX-800_View/MainWindowView/MainWindowView.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);
    void modify_json();

signals:

private:
    MainWindowView* m_MainWindowView = nullptr;
    //MainWindowModel* m_MainWindowModel = nullptr;

    receive* m_A;
    DaHeng_camera* m_C;
    read_json *m_Json;
    chip_detect *m_I;
};

#endif // CONTROLLER_H
