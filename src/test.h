#ifndef QLOG4CPLUS_H
#define QLOG4CPLUS_H

#include <QObject>
#include <QDateTime>
#include <QDir>
#include <mutex>
#include <QDebug>

//日志
#include "log4cplus/log4cplus.h"
#include "log4cplus/fileappender.h"
#include "log4cplus/layout.h"

//宏定义日志，减少代码量
#define My_LOG(level, message) QLog4cplus::getQLog4cplusInstance().writeLog(level, message)
#define My_OLog(level, message) QLog4cplus::getQLog4cplusInstance().onlyWriteLog(level, message)

// 宏定义日志等级
#define QLOG_DEBUG QLog4cplus::l_DEBUG
#define QLOG_INFO QLog4cplus::l_INFO
#define QLOG_WARN QLog4cplus::l_WARN
#define QLOG_ERROR QLog4cplus::l_ERROR

class QLog4cplus : public QObject
{
    Q_OBJECT
public:
    enum Level {l_DEBUG=1,l_INFO,l_WARN,l_ERROR};
private:
    //私有构造函数
    QLog4cplus();
public:
    //删除拷贝构造和赋值构造
    QLog4cplus(const QLog4cplus&) = delete;
    QLog4cplus& operator=(const QLog4cplus&) = delete;
    //提供一个公共的静态方法来获取实例
    static QLog4cplus& getQLog4cplusInstance();
    //析构函数
    ~QLog4cplus();
    //设置底层日志等级
    void setQlogLevel(QLog4cplus::Level level);
    //设置同步到界面的日志等级
    void setQUIlogLevel(QLog4cplus::Level level);
    //写入日志并且同步界面日志
    void writeLog(QLog4cplus::Level,const char* message);
    //只写入日志
    void onlyWriteLog(QLog4cplus::Level,const char* message);
protected:
    //日志等级转QString
    QString levelToString(QLog4cplus::Level level);
signals:
    void signalLogOutput(QLog4cplus::Level,QString);
private:
    //log4cplus日志对象
    log4cplus::Logger m_logger;
    //饿汉式的私有静态实例
    static QLog4cplus instance;
    //日志写入日志并且同步界面日志互斥锁
    std::mutex m_mtx;
    //日志只写入日志互斥锁
    std::mutex m_mtxOnly;
    //底层日志等级
    int m_logLevel = 2;
    //是否同步到界面日志等级
    int m_UIlogLevel = 1;
};

#endif // QLOG4CPLUS_H
