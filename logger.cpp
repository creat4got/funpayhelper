#include "logger.h"
#include <QDateTime>

Logger* Logger::s_instance = nullptr;

Logger* Logger::instance()
{
    if (!s_instance)
        s_instance = new Logger();
    return s_instance;
}

Logger::Logger(QObject* parent) : QObject(parent) {}

void Logger::log(LogLevel level, const QString& message)
{
    QString ts = QDateTime::currentDateTime().toString("hh:mm:ss");
    emit newLogEntry(static_cast<int>(level), ts, message);
}
