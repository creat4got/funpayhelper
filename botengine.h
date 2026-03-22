#ifndef BOTENGINE_H
#define BOTENGINE_H

#include <QObject>
#include <QThread>
#include "botworker.h"
#include "models.h"

class BotEngine : public QObject
{
    Q_OBJECT
public:
    explicit BotEngine(const QString& cookieHeader, QObject* parent = nullptr);
    ~BotEngine() override;

    bool isRunning() const { return m_running; }
    void start();
    void stop();
    void sendChatMessage(const QString& nodeId, const QString& text);

signals:
    void logMessage(int level, const QString& msg);
    void newMessage(ChatMessage msg);
    void newOrder(OrderInfo order);
    void statusChanged(const QString& status);
    void initialized(UserInfo info);
    void stopped();
    void messageSent(QString nodeId, bool ok, QString error);

private slots:
    void onWorkerFinished();

private:
    QString     m_cookieHeader;
    QThread*    m_thread = nullptr;
    BotWorker*  m_worker = nullptr;
    bool        m_running= false;
};

#endif // BOTENGINE_H
