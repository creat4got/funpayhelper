#include "settingsmanager.h"
#include <QSettings>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

SettingsManager* SettingsManager::s_instance = nullptr;

SettingsManager* SettingsManager::instance()
{
    if (!s_instance)
        s_instance = new SettingsManager();
    return s_instance;
}

SettingsManager::SettingsManager(QObject* parent)
    : QObject(parent)
{
    m_settings = new QSettings("botfunpay", "botfunpay", this);
}

int SettingsManager::raiseIntervalMinutes() const
{
    return m_settings->value("raise/interval", 180).toInt();
}
void SettingsManager::setRaiseIntervalMinutes(int m)
{
    m_settings->setValue("raise/interval", m);
}

bool SettingsManager::autoRaiseEnabled() const
{
    return m_settings->value("raise/enabled", false).toBool();
}
void SettingsManager::setAutoRaiseEnabled(bool v)
{
    m_settings->setValue("raise/enabled", v);
}

bool SettingsManager::autoResponderEnabled() const
{
    return m_settings->value("responder/enabled", false).toBool();
}
void SettingsManager::setAutoResponderEnabled(bool v)
{
    m_settings->setValue("responder/enabled", v);
}

bool SettingsManager::autoDeliveryEnabled() const
{
    return m_settings->value("delivery/enabled", false).toBool();
}
void SettingsManager::setAutoDeliveryEnabled(bool v)
{
    m_settings->setValue("delivery/enabled", v);
}

QList<AutoResponderRule> SettingsManager::responderRules() const
{
    QList<AutoResponderRule> rules;
    QByteArray data = m_settings->value("responder/rules").toByteArray();
    if (data.isEmpty()) return rules;

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) return rules;

    for (const QJsonValue& v : doc.array()) {
        if (!v.isObject()) continue;
        QJsonObject o = v.toObject();
        AutoResponderRule r;
        r.trigger   = o["trigger"].toString();
        r.response  = o["response"].toString();
        r.enabled   = o["enabled"].toBool(true);
        r.matchExact= o["matchExact"].toBool(false);
        rules.append(r);
    }
    return rules;
}

void SettingsManager::setResponderRules(const QList<AutoResponderRule>& rules)
{
    QJsonArray arr;
    for (const auto& r : rules) {
        QJsonObject o;
        o["trigger"]   = r.trigger;
        o["response"]  = r.response;
        o["enabled"]   = r.enabled;
        o["matchExact"]= r.matchExact;
        arr.append(o);
    }
    m_settings->setValue("responder/rules", QJsonDocument(arr).toJson());
}

QList<DeliveryItem> SettingsManager::deliveryItems() const
{
    QList<DeliveryItem> items;
    QByteArray data = m_settings->value("delivery/items").toByteArray();
    if (data.isEmpty()) return items;

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) return items;

    for (const QJsonValue& v : doc.array()) {
        if (!v.isObject()) continue;
        QJsonObject o = v.toObject();
        DeliveryItem item;
        item.lotId       = o["lotId"].toString();
        item.lotTitle    = o["lotTitle"].toString();
        item.deliveryText= o["deliveryText"].toString();
        item.enabled     = o["enabled"].toBool(true);
        items.append(item);
    }
    return items;
}

void SettingsManager::setDeliveryItems(const QList<DeliveryItem>& items)
{
    QJsonArray arr;
    for (const auto& item : items) {
        QJsonObject o;
        o["lotId"]       = item.lotId;
        o["lotTitle"]    = item.lotTitle;
        o["deliveryText"]= item.deliveryText;
        o["enabled"]     = item.enabled;
        arr.append(o);
    }
    m_settings->setValue("delivery/items", QJsonDocument(arr).toJson());
}
