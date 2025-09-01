#include "Logger.h"
#include <QCoreApplication>
#include <QDir>
#include <iostream>

// 修改：初始化QObject父类
Logger::Logger(QObject *parent)
    : QObject(parent)
    , m_consoleOutput(true)
    , m_logLevel(LogLevel::LOG_DEBUG)
{
    // 默认日志前缀
    m_logPrefix = "mylog";

    // 默认日志路径：应用程序目录下的logs文件夹
    // m_logDirectory = QCoreApplication::applicationDirPath() + "/logs";
    m_logDirectory = "../../Logs";

    // 创建日志目录（如果不存在）
    QDir().mkpath(m_logDirectory);

    // 初始化日志文件
    checkAndSwitchLogFile();
}

Logger::~Logger()
{
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
}

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::setLogPrefix(const QString &prefix)
{
    QMutexLocker locker(&m_mutex);
    m_logPrefix = prefix;
    checkAndSwitchLogFile(); // 可能需要切换日志文件
}

void Logger::setLogPrefix(const std::string &prefix)
{
    setLogPrefix(QString::fromStdString(prefix));
}

void Logger::setLogDirectory(const QString &directory)
{
    QMutexLocker locker(&m_mutex);
    m_logDirectory = directory;

    // 创建目录（如果不存在）
    QDir().mkpath(m_logDirectory);

    checkAndSwitchLogFile(); // 可能需要切换日志文件
}

void Logger::setLogDirectory(const std::string &directory)
{
    setLogDirectory(QString::fromStdString(directory));
}

void Logger::setConsoleOutput(bool enable)
{
    QMutexLocker locker(&m_mutex);
    m_consoleOutput = enable;
}

void Logger::setLogLevel(LogLevel level)
{
    QMutexLocker locker(&m_mutex);
    m_logLevel = level;
}

// 原有接口：只写入日志
void Logger::writeLog(LogLevel level, const QString &message)
{
    QMutexLocker locker(&m_mutex);
    writeLogInternal(level, message);
}

void Logger::writeLog(LogLevel level, const std::string &message)
{
    writeLog(level, QString::fromStdString(message));
}

// 新增接口：写入日志并发送信号
void Logger::writeLogWithSignal(LogLevel level, const QString &message)
{
    QMutexLocker locker(&m_mutex);
    writeLogInternal(level, message);
    // 发送信号
    QString timeStr = getCurrentTimeString();
    QString levelStr = levelToString(level);
    QString logMessage = QString("[%1] [%2] %3\n").arg(timeStr).arg(levelStr).arg(message);
    emit signals_Logger_Sent_Log(level, logMessage);
}

void Logger::writeLogWithSignal(LogLevel level, const std::string &message)
{
    writeLogWithSignal(level, QString::fromStdString(message));
}

// 新增：内部日志写入实现（复用逻辑）
void Logger::writeLogInternal(LogLevel level, const QString &message)
{
    // 检查是否需要切换日志文件（基于当前日期）
    checkAndSwitchLogFile();

    // 如果当前日志级别低于设置的级别，则不输出
    if (static_cast<int>(level) < static_cast<int>(m_logLevel)) {
        return;
    }

    QString timeStr = getCurrentTimeString();
    QString levelStr = levelToString(level);
    QString logMessage = QString("[%1] [%2] %3\n").arg(timeStr).arg(levelStr).arg(message);

    // 写入文件
    if (m_logFile.isOpen()) {
        m_fileStream << logMessage;
        m_fileStream.flush();
    }

    // 输出到控制台
    if (m_consoleOutput) {
        // std::cout << logMessage.toStdString();
    }
}

void Logger::checkAndSwitchLogFile()
{
    QString currentDate = getCurrentDateString();

    // 如果日期已更改或日志文件未打开，则创建新的日志文件
    if (currentDate != m_currentDate || !m_logFile.isOpen()) {
        m_currentDate = currentDate;

        // 关闭当前日志文件
        if (m_logFile.isOpen()) {
            m_logFile.close();
        }

        // 构建新的日志文件名：前缀_日期.log
        QString logFileName = QString("%1_%2.log").arg(m_logPrefix).arg(currentDate);
        QString logFilePath = m_logDirectory + "/" + logFileName;

        // 打开新的日志文件
        m_logFile.setFileName(logFilePath);
        if (m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            m_fileStream.setDevice(&m_logFile);
            // qDebug() << "日志文件已切换至：" << logFilePath;
        } else {
            QString error = "无法打开日志文件: " + logFilePath;
            LOG_ERROR_SIGNAL(error);
        }
    }
}

QString Logger::getCurrentDateString() const
{
    return QDateTime::currentDateTime().toString("yyyyMMdd");
}

QString Logger::levelToString(LogLevel level) const
{
    switch (level) {
    case LogLevel::LOG_DEBUG:
        return "DEBUG";
    case LogLevel::LOG_INFO:
        return "INFO";
    case LogLevel::LOG_WARNING:
        return "WARNING";
    case LogLevel::LOG_ERROR:
        return "ERROR";
    case LogLevel::LOG_FATAL:
        return "FATAL";
    default:
        return "UNKNOWN";
    }
}

QString Logger::getCurrentTimeString() const
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
}
