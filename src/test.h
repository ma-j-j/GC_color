#ifndef LOGGER_H
#define LOGGER_H

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QMutex>
#include <QObject> // 新增：添加QObject头文件
#include <QString>
#include <QTextStream>
#include <string>

enum class LogLevel { LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_FATAL };

// 新增：继承QObject以支持信号
class Logger : public QObject
{
    Q_OBJECT // 新增：启用Qt元对象系统

public:
    // 单例模式
    static Logger &getInstance();

    // 禁用拷贝构造和赋值操作
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    // 设置日志文件前缀（如"mylog"）
    void setLogPrefix(const QString &prefix);
    void setLogPrefix(const std::string &prefix);

    // 设置日志目录
    void setLogDirectory(const QString &directory);
    void setLogDirectory(const std::string &directory);

    // 设置是否输出到控制台
    void setConsoleOutput(bool enable);

    // 设置日志级别
    void setLogLevel(LogLevel level);

    // 原有接口：只写入日志，不发送信号
    void writeLog(LogLevel level, const QString &message);
    void writeLog(LogLevel level, const std::string &message);

    // 新增接口：写入日志并发送信号
    void writeLogWithSignal(LogLevel level, const QString &message);
    void writeLogWithSignal(LogLevel level, const std::string &message);

signals: // 新增：声明信号
    void signals_Logger_Sent_Log(LogLevel level, QString message);

private:
    Logger(QObject *parent = nullptr); // 修改：添加QObject父类构造
    ~Logger();

    // 核心日志写入实现（内部使用）
    void writeLogInternal(LogLevel level, const QString &message);

    // 检查并切换日志文件（如果日期已更改）
    void checkAndSwitchLogFile();

    // 获取当前日期字符串（格式：yyyyMMdd）
    QString getCurrentDateString() const;

    // 日志级别转字符串
    QString levelToString(LogLevel level) const;

    // 获取当前时间字符串（格式：yyyy-MM-dd hh:mm:ss.zzz）
    QString getCurrentTimeString() const;

    QFile m_logFile;          // 日志文件
    QTextStream m_fileStream; // 文件流
    QMutex m_mutex;           // 互斥锁，保证线程安全
    bool m_consoleOutput;     // 是否输出到控制台
    LogLevel m_logLevel;      // 日志级别
    QString m_logPrefix;      // 日志文件前缀
    QString m_logDirectory;   // 日志目录
    QString m_currentDate;    // 当前日志文件的日期
};

// 便捷宏定义（原有宏保持不变，新增带信号的宏）
#define LOG_DEBUG(msg) Logger::getInstance().writeLog(LogLevel::LOG_DEBUG, msg)
#define LOG_INFO(msg) Logger::getInstance().writeLog(LogLevel::LOG_INFO, msg)
#define LOG_WARNING(msg) Logger::getInstance().writeLog(LogLevel::LOG_WARNING, msg)
#define LOG_ERROR(msg) Logger::getInstance().writeLog(LogLevel::LOG_ERROR, msg)
#define LOG_FATAL(msg) Logger::getInstance().writeLog(LogLevel::LOG_FATAL, msg)

// 新增带信号的日志宏
#define LOG_DEBUG_SIGNAL(msg) Logger::getInstance().writeLogWithSignal(LogLevel::LOG_DEBUG, msg)
#define LOG_INFO_SIGNAL(msg) Logger::getInstance().writeLogWithSignal(LogLevel::LOG_INFO, msg)
#define LOG_WARNING_SIGNAL(msg) Logger::getInstance().writeLogWithSignal(LogLevel::LOG_WARNING, msg)
#define LOG_ERROR_SIGNAL(msg) Logger::getInstance().writeLogWithSignal(LogLevel::LOG_ERROR, msg)
#define LOG_FATAL_SIGNAL(msg) Logger::getInstance().writeLogWithSignal(LogLevel::LOG_FATAL, msg)

#endif // LOGGER_H
