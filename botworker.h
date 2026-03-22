#ifndef BOTWORKER_H
#define BOTWORKER_H

#include <QObject>
#include <QSet>
#include "funpayapi.h"
#include "models.h"

class QTimer;

class BotWorker : public QObject
{
    Q_OBJECT
public:
    explicit BotWorker(const QString& cookieHeader, QObject* parent = nullptr);
    ~BotWorker() override;

public slots:
    void start();
    void stop();
    void sendChatMessage(const QString& nodeId, const QString& text);

signals:
    void logMessage(int level, const QString& msg);
    void newMessage(ChatMessage msg);
    void newOrder(OrderInfo order);
    void statusChanged(const QString& status);
    void initialized(UserInfo info);
    void finished();
    void messageSent(QString nodeId, bool ok, QString error);

private slots:
    void onPollTimer();
    void onRaiseTimer();

private:
    void handleNewMessage(const ChatMessage& msg);
    void handleNewOrder(const OrderInfo& order);
    void raiseAllConfigured();

    QString    m_cookieHeader;
    FunPayAPI* m_api       = nullptr;
    QTimer*    m_pollTimer = nullptr;
    QTimer*    m_raiseTimer= nullptr;
    bool       m_running   = false;

    QSet<QString> m_processedMsgIds;
    QSet<QString> m_processedOrderIds;
};

#endif // BOTWORKER_H
