#ifndef MODELS_H
#define MODELS_H

#include <QString>
#include <QList>

struct UserInfo {
    QString username;
    QString userId;
    QString csrfToken;
};

// Один активный лот продавца
struct LotInfo {
    QString id;           // offer?id=XXXXXX
    QString nodeId;       // /lots/NODE_ID/ (категория, нужна для поднятия)
    QString gameId;       // data-game на trade-странице (нужен для поднятия)
    QString sectionName;  // название раздела ("Adobe Accounts", "Dota 2 Accounts"...)
    QString description;  // текст описания лота
    double  price = 0.0;
    bool    active = true;
};

// Секция (раздел) продавца — объединяет node_id + game_id + лоты
struct SectionInfo {
    QString nodeId;
    QString gameId;       // заполняется при fetchSectionGameIds
    QString name;
    QList<LotInfo> lots;
};

struct ChatMessage {
    QString id;
    QString nodeId;
    QString chatTitle;
    QString text;
    QString authorId;
    bool isMyMessage = false;
};

// Контакт (элемент списка чатов)
struct ChatContact {
    QString nodeId;       // data-id
    QString username;     // .media-user-name
    QString lastMessage;  // .contact-item-message (preview)
    QString time;         // .contact-item-time
    qint64  lastMsgId = 0;// data-node-msg
};

// Одно сообщение в истории чата
struct ChatHistoryMsg {
    qint64  id = 0;
    QString author;
    bool    isMyMessage = false;
    bool    isSystem    = false;  // сообщение от FunPay (уведомление)
    QString text;
    QString time;         // из title="22 March, 7:56:24"
};

struct OrderInfo {
    QString id;
    QString buyerUsername;
    QString buyerId;
    QString lotTitle;
    QString nodeId;
    double price = 0.0;
};

struct AutoResponderRule {
    QString trigger;
    QString response;
    bool enabled = true;
    bool matchExact = false;
};

struct DeliveryItem {
    QString lotId;
    QString lotTitle;
    QString deliveryText;
    bool enabled = true;
};

#endif // MODELS_H
