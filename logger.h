#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>

enum class LogLevel { Info = 0, Success = 1, Warning = 2, Error = 3 };

class Logger : public QObject
{
    Q_OBJECT
public:
    static Logger* instance();

    void log(LogLevel level, const QString& message);
    void info(const QString& msg)    { log(LogLevel::Info, msg); }
    void success(const QString& msg) { log(LogLevel::Success, msg); }
    void warn(const QString& msg)    { log(LogLevel::Warning, msg); }
    void error(const QString& msg)   { log(LogLevel::Error, msg); }

signals:
    void newLogEntry(int level, const QString& timestamp, const QString& message);

private:
    explicit Logger(QObject* parent = nullptr);
    static Logger* s_instance;
};

#endif // LOGGER_H
