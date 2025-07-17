#include "qlog4cplus.h"

//类外初始化静态成员
QLog4cplus QLog4cplus::instance;

QLog4cplus::QLog4cplus()
{
    //先判断存储日志文件夹是否存在，不存在则创建
    QDir dir("logs");
    if (!dir.exists()) {
        if (dir.mkpath(".")) {
        }
        else {
        }
    } else {
    }
    //输出到回卷文件
    log4cplus::SharedAppenderPtr fileAppender(new log4cplus::RollingFileAppender(LOG4CPLUS_TEXT("logs/mylog.log"),50*1024*1024,10));
    log4cplus::tstring pattern = LOG4CPLUS_TEXT("%D{%m/%d/%y %H:%M:%S,%Q} [%t] %-5p: %m%n");
    fileAppender->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(pattern)));
    this->m_logger = log4cplus::Logger::getRoot();
    this->m_logger.addAppender(fileAppender);
//默认状态
#ifdef QT_DEBUG
    //Debug模式所有日志打印
    this->m_logger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL);
#else
    //Realse模式只输出info及以上级别的日志
    this->m_logger.setLogLevel(log4cplus::INFO_LOG_LEVEL);
#endif
}

QLog4cplus &QLog4cplus::getQLog4cplusInstance()
{
    return QLog4cplus::instance;
}

QLog4cplus::~QLog4cplus()
{
    // 关闭log4cplus
    log4cplus::Logger::shutdown();
}

void QLog4cplus::setQlogLevel(QLog4cplus::Level level)
{
    switch (level)
    {
    case QLog4cplus::Level::l_DEBUG:
        this->m_logger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL);
        break;
    case QLog4cplus::Level::l_INFO:
        this->m_logger.setLogLevel(log4cplus::INFO_LOG_LEVEL);
        break;
    case QLog4cplus::Level::l_WARN:
        this->m_logger.setLogLevel(log4cplus::WARN_LOG_LEVEL);
        break;
    case QLog4cplus::Level::l_ERROR:
        this->m_logger.setLogLevel(log4cplus::ERROR_LOG_LEVEL);
        break;
    default:
        break;
    }
}

void QLog4cplus::setQUIlogLevel(QLog4cplus::Level level)
{
    this->m_UIlogLevel = level;
}

void QLog4cplus::writeLog(QLog4cplus::Level level,const char* message)
{
    //上锁，确保每次只有一条写入
    std::lock_guard<std::mutex> lock(this->m_mtx);
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString logLevel = this->levelToString(level);
    QString logMessage = QString("%1 %2 :%3 ")
                            .arg(timestamp)
                            .arg(logLevel)
                            .arg(message);
    qDebug()<<logMessage;
    if(level >= m_UIlogLevel)
    {   //同步到UI界面
        emit this->signalLogOutput(level,logMessage);
    }
    else
    {
        //不同步到UI界面
    }
    switch (level)
    {
    case QLog4cplus::Level::l_DEBUG:
        LOG4CPLUS_DEBUG(this->m_logger,message);
        break;
    case QLog4cplus::Level::l_INFO:
        LOG4CPLUS_INFO(this->m_logger,message);
        break;
    case QLog4cplus::Level::l_WARN:
        LOG4CPLUS_WARN(this->m_logger,message);
        break;
    case QLog4cplus::Level::l_ERROR:
        LOG4CPLUS_ERROR(this->m_logger,message);
        break;
    default:
        break;
    }
}

void QLog4cplus::onlyWriteLog(QLog4cplus::Level level, const char *message)
{
    //上锁，确保每次只有一条写入
    std::lock_guard<std::mutex> lock(this->m_mtxOnly);
    switch (level)
    {
    case QLog4cplus::Level::l_DEBUG:
        LOG4CPLUS_DEBUG(this->m_logger,message);
        break;
    case QLog4cplus::Level::l_INFO:
        LOG4CPLUS_INFO(this->m_logger,message);
        break;
    case QLog4cplus::Level::l_WARN:
        LOG4CPLUS_WARN(this->m_logger,message);
        break;
    case QLog4cplus::Level::l_ERROR:
        LOG4CPLUS_ERROR(this->m_logger,message);
        break;
    default:
        break;
    }
}

QString QLog4cplus::levelToString(QLog4cplus::Level level)
{
    switch (level)
    {
    case QLog4cplus::Level::l_DEBUG:
        return "[Debug]";
        break;
    case QLog4cplus::Level::l_INFO:
        return "[Info]";
        break;
    case QLog4cplus::Level::l_WARN:
        return "[Warn]";
        break;
    case QLog4cplus::Level::l_ERROR:
        return "[Error]";
        break;
    default:
        break;
    }
}





























