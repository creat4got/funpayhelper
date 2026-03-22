#ifndef FUNPAYAPI_H
#define FUNPAYAPI_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QList>
#include <QJsonObject>
#include "models.h"

struct RunnerEvent {
    QString type;
    QString id;
    QString tag;
    QJsonObject data;
    bool hasData = false;
};

class FunPayAPI : public QObject
{
    Q_OBJECT
public:
    explicit FunPayAPI(const QString& cookieHeader, QObject* parent = nullptr);

    bool init(QString& error);
    bool refreshToken(QString& error);

    // Runner
    bool poll(QList<RunnerEvent>& events, QString& error);

    // Лоты:
    //   1) Парсит профиль → секции с лотами (без game_id)
    bool fetchUserOffers(QList<SectionInfo>& sections, QString& error);
    //   2) Для каждой секции загружает trade-страницу → заполняет game_id
    bool fetchSectionGameIds(QList<SectionInfo>& sections, QString& error);
    //   3) Поднимает одну секцию
    bool raiseSection(const QString& gameId, const QString& nodeId,
                      QString& resultMsg, QString& error);

    // Чаты:
    //   1) Список чатов (GET /en/chat/)
    bool fetchChatContacts(QList<ChatContact>& contacts, QString& error);
    //   2) История сообщений одного чата (GET /en/chat/?node=NODE_ID)
    //      nodeName — data-name чата ("users-XXXX-YYYY"), нужен для отправки
    struct ChatNodeInfo
    {
        QString nodeName;
        QString otherUserId;

        QString chatTag;
        QString bookmarksTag;
        QString ordersCountersTag;
        QString cpuTag;
        QString cpuId;
    };

    bool fetchChatHistory(const QString& nodeId,
                          QList<ChatHistoryMsg>& messages,
                          ChatNodeInfo& info,
                          QString& error);

    bool sendMessageNode(const ChatNodeInfo& info,
                         qint64 lastMsgId,
                         const QString& numericNodeId,
                         const QString& text,
                         const QList<ChatContact>& contacts,
                         QString& error);

    const UserInfo& userInfo() const { return m_userInfo; }
    bool isInitialized() const       { return m_initialized; }

private:
    bool fetchPageInfo(QString& error);
    QString buildRunnerBody() const;
    bool parseRunnerResponse(const std::string& body,
                             QList<RunnerEvent>& events,
                             QString& error);
    bool raiseFromModal(const QString& modalHtml, const QString& originalNodeId,
                        QString& resultMsg, QString& error);

    QString  m_cookieHeader;
    UserInfo m_userInfo;
    QMap<QString, QString> m_runnerTags;
    bool m_initialized = false;
    bool m_firstPoll   = true;
};

#endif // FUNPAYAPI_H
