#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QList>
#include "models.h"

class QSettings;

class SettingsManager : public QObject
{
    Q_OBJECT
public:
    static SettingsManager* instance();

    // General
    int  raiseIntervalMinutes() const;
    void setRaiseIntervalMinutes(int minutes);

    bool autoRaiseEnabled() const;
    void setAutoRaiseEnabled(bool enabled);

    bool autoResponderEnabled() const;
    void setAutoResponderEnabled(bool enabled);

    bool autoDeliveryEnabled() const;
    void setAutoDeliveryEnabled(bool enabled);

    // Auto-responder rules
    QList<AutoResponderRule> responderRules() const;
    void setResponderRules(const QList<AutoResponderRule>& rules);

    // Delivery items
    QList<DeliveryItem> deliveryItems() const;
    void setDeliveryItems(const QList<DeliveryItem>& items);

private:
    explicit SettingsManager(QObject* parent = nullptr);
    QSettings* m_settings;
    static SettingsManager* s_instance;
};

#endif // SETTINGSMANAGER_H
