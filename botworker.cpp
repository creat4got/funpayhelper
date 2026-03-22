#include "botworker.h"
#include "settingsmanager.h"
#include <QTimer>
#include <QJsonArray>
#include <QJsonObject>
#include <QSet>
#include <QThread>

BotWorker::BotWorker(const QString& cookieHeader, QObject* parent)
    : QObject(parent), m_cookieHeader(cookieHeader)
{
}

BotWorker::~BotWorker()
{
    stop();
}

void BotWorker::start()
{
    if (!m_api) {
        m_api = new FunPayAPI(m_cookieHeader, this);
    }

    QString err;
    if (!m_api->init(err)) {
        emit logMessage(3, QString("Ошибка инициализации API: %1").arg(err));

        return;
    }

    m_running = true;

    if (!m_pollTimer) {
        m_pollTimer = new QTimer(this);
        connect(m_pollTimer, &QTimer::timeout, this, &BotWorker::onPollTimer);
    }
    m_pollTimer->start(3000);

    SettingsManager* s = SettingsManager::instance();

    if (s->autoRaiseEnabled()) {
        if (!m_raiseTimer) {
            m_raiseTimer = new QTimer(this);
            connect(m_raiseTimer, &QTimer::timeout, this, &BotWorker::onRaiseTimer);
        }

        int ms = s->raiseIntervalMinutes() * 60 * 1000;
        if (ms < 60 * 1000)
            ms = 60 * 1000;

        m_raiseTimer->start(ms);
        emit logMessage(1, QString("[Авто-поднятие] Запущено, интервал %1 мин.")
                            .arg(s->raiseIntervalMinutes()));
    } else {
        if (m_raiseTimer)
            m_raiseTimer->stop();
    }


    emit statusChanged("Работает");
    emit logMessage(1, "Бот запущен");
}

void BotWorker::stop()
{
    if (!m_running) return;
    m_running = false;

    if (m_pollTimer)  m_pollTimer->stop();
    if (m_raiseTimer) m_raiseTimer->stop();

    emit statusChanged("Остановлен");
    emit logMessage(2, "Бот остановлен");
    emit finished();
}

// ─── Runner poll ──────────────────────────────────────────────────────────────
void BotWorker::onPollTimer()
{
    if (!m_api || !m_running) return;

    QList<RunnerEvent> events;
    QString err;

    if (!m_api->poll(events, err)) {
        emit logMessage(3, "Ошибка опроса: " + err);
        return;
    }

    for (const RunnerEvent& ev : events) {
        // ── Новые сообщения ──
        if (ev.type == "chat_bookmarks" && ev.hasData) {
            QJsonArray bookmarks = ev.data["bookmarks"].toArray();
            for (const QJsonValue& bv : bookmarks) {
                if (!bv.isObject()) continue;
                QJsonObject bk = bv.toObject();

                QJsonObject node    = bk["node"].toObject();
                QJsonObject lastMsg = bk["lastMessage"].toObject();

                QString msgId = QString::number(lastMsg["id"].toDouble());
                if (m_processedMsgIds.contains(msgId)) continue;
                m_processedMsgIds.insert(msgId);

                ChatMessage msg;
                msg.id         = msgId;
                msg.nodeId     = QString::number(node["id"].toDouble());
                msg.chatTitle  = node["title"].toString();
                msg.text       = lastMsg["text"].toString();
                msg.authorId   = QString::number(lastMsg["author"].toDouble());
                msg.isMyMessage= (msg.authorId == m_api->userInfo().userId);

                if (!msg.isMyMessage && !msg.text.isEmpty()) {
                    emit logMessage(0,
                        QString("[Сообщение] %1: %2").arg(msg.chatTitle, msg.text));
                    emit newMessage(msg);
                    handleNewMessage(msg);
                }
            }
        }

        // ── Новые заказы ──
        else if (ev.type == "orders_counters" && ev.hasData) {
            QJsonArray paidIds = ev.data["paid_ids"].toArray();
            for (const QJsonValue& idV : paidIds) {
                QString orderId = idV.isString()
                                      ? idV.toString()
                                      : QString::number(idV.toDouble());
                if (orderId.isEmpty() || m_processedOrderIds.contains(orderId))
                    continue;
                m_processedOrderIds.insert(orderId);

                OrderInfo order;
                order.id = orderId;

                emit logMessage(1, "[Заказ] Новый заказ: " + orderId);
                emit newOrder(order);
                handleNewOrder(order);
            }
        }
    }
}

// ─── Auto-responder ───────────────────────────────────────────────────────────
void BotWorker::handleNewMessage(const ChatMessage& msg)
{
    auto* s = SettingsManager::instance();
    if (!s->autoResponderEnabled()) return;

    for (const AutoResponderRule& rule : s->responderRules()) {
        if (!rule.enabled) continue;

        bool hit = false;
        if (rule.trigger.isEmpty()) {
            hit = true;
        } else if (rule.matchExact) {
            hit = msg.text.trimmed().toLower() == rule.trigger.trimmed().toLower();
        } else {
            hit = msg.text.toLower().contains(rule.trigger.toLower());
        }

        if (hit) {
            QString response = rule.response;
            response.replace("%username%", msg.chatTitle);
            response.replace("%buyer%", msg.chatTitle);

            emit logMessage(2,
                QString("[Авто-ответ] Совпадение для '%1', но автоотправка временно отключена до стабилизации runner-отправки.")
                    .arg(msg.chatTitle));
            break;
        }
    }
}

// ─── Auto-delivery ────────────────────────────────────────────────────────────
void BotWorker::handleNewOrder(const OrderInfo& order)
{
    auto* s = SettingsManager::instance();
    if (!s->autoDeliveryEnabled()) return;

    // Сопоставление по lotId или lotTitle
    for (const DeliveryItem& item : s->deliveryItems()) {
        if (!item.enabled) continue;
        // Без node_id мы не можем отправить сообщение автоматически;
        // FunPay нужен отдельный запрос деталей заказа.
        // Пока логируем намерение — доработать при наличии API деталей.
        emit logMessage(1,
            QString("[Авто-выдача] Заказ %1 → товар будет выдан по лоту '%2'")
            .arg(order.id, item.lotTitle));
        break;
    }
}

// ─── Send chat message (uses live m_api with current CSRF) ────────────────────
void BotWorker::sendChatMessage(const QString& nodeId, const QString& text)
{
    if (!m_api) {
        emit messageSent(nodeId, false, "Бот не запущен");
        return;
    }

    FunPayAPI::ChatNodeInfo dummyInfo;
    QList<ChatContact> dummyContacts;
    QString err;
    bool ok = m_api->sendMessageNode(dummyInfo, -1, nodeId, text, dummyContacts, err);
    emit messageSent(nodeId, ok, err);
}

// ─── Auto-raise ───────────────────────────────────────────────────────────────
void BotWorker::raiseAllConfigured()
{
    QList<SectionInfo> sections;
    QString err;

    if (!m_api->fetchUserOffers(sections, err)) {
        emit logMessage(3, QString("[Поднятие] Не удалось загрузить лоты: %1").arg(err));
        return;
    }

    if (!m_api->fetchSectionGameIds(sections, err)) {
        emit logMessage(2, QString("[Поднятие] Не удалось получить game_id: %1").arg(err));
        // продолжаем с тем, что смогли загрузить
    }

    QSet<QString> processedGames;

    for (const SectionInfo& sec : sections) {
        if (sec.gameId.isEmpty() || sec.nodeId.isEmpty())
            continue;

        if (processedGames.contains(sec.gameId))
            continue;

        processedGames.insert(sec.gameId);

        QString resultMsg;
        QString raiseErr;
        bool ok = m_api->raiseSection(sec.gameId, sec.nodeId, resultMsg, raiseErr);

        if (ok) {
            emit logMessage(1, QString("[Поднятие] %1 (game=%2, node=%3): %4")
                                .arg(sec.name, sec.gameId, sec.nodeId, resultMsg));
        } else if (raiseErr.toLower().contains("wait")) {
            emit logMessage(2, QString("[Поднятие] %1 (game=%2, node=%3): %4")
                                .arg(sec.name, sec.gameId, sec.nodeId, raiseErr));
        } else {
            emit logMessage(3, QString("[Поднятие] %1 (game=%2, node=%3): %4")
                                .arg(sec.name, sec.gameId, sec.nodeId, raiseErr));
        }

        QThread::msleep(600);
    }
}

void BotWorker::onRaiseTimer()
{
    if (!m_api || !m_running) return;
    emit logMessage(0, "[Поднятие] Запуск авто-поднятия…");
    raiseAllConfigured();
}
