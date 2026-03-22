#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QTabWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QSplitter>

#include "botengine.h"
#include "funpayapi.h"
#include "models.h"

class DashboardWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DashboardWindow(const UserInfo& userInfo,
                             const QString& cookieHeader,
                             QWidget* parent = nullptr);
    ~DashboardWindow() override;

private slots:
    // Бот
    void onStartStopClicked();
    void onBotLog(int level, const QString& msg);
    void onBotNewMessage(const ChatMessage& msg);
    void onBotNewOrder(const OrderInfo& order);
    void onBotStatusChanged(const QString& status);
    void onBotStopped();

    // Лоты
    void onRefreshLots();
    void onRaiseNow();
    // Вызываются из worker-потока через Qt::QueuedConnection
    void onLotsLoaded(QList<SectionInfo> sections);
    void onLotsLoadError(QString error);
    void onRaiseResult(QString nodeId, bool ok, QString msg);
    void onRaiseAllFinished();

    // Авто-ответчик
    void onAddRule();
    void onDeleteRule();
    void onSaveRules();

    // Авто-выдача
    void onAddDelivery();
    void onDeleteDelivery();
    void onSaveDelivery();

    // Настройки
    void onSaveSettings();

    // Чаты
    void onRefreshChats();
    void onChatContactSelected(QListWidgetItem* item);
    void onSendChatMessage();
    void onChatsLoaded(QList<ChatContact> contacts);
    void onChatsLoadError(QString error);
    void onChatHistoryLoaded(QString nodeId, FunPayAPI::ChatNodeInfo info, qint64 lastMsgId, QList<ChatHistoryMsg> messages);
    void onChatHistoryLoadError(QString nodeId, QString error);

signals:
    // Используются для связи с рабочими потоками лотов
    void lotsLoaded(QList<SectionInfo> sections);
    void lotsLoadError(QString error);
    void raiseResult(QString nodeId, bool ok, QString msg);
    void raiseAllFinished();

    // Чаты (из рабочих потоков → UI)
    void chatsLoaded(QList<ChatContact> contacts);
    void chatsLoadError(QString error);
    void chatHistoryLoaded(QString nodeId, FunPayAPI::ChatNodeInfo info, qint64 lastMsgId, QList<ChatHistoryMsg> messages);
    void chatHistoryLoadError(QString nodeId, QString error);

private:
    void setupUi();
    QWidget* createLogsTab();
    QWidget* createLotsTab();
    QWidget* createAutoResponderTab();
    QWidget* createAutoDeliveryTab();
    QWidget* createSettingsTab();
    QWidget* createChatTab();

    void refreshContactsList();
    void renderChatHistory();

    void appendLog(int level, const QString& ts, const QString& msg);
    void refreshResponderTable();
    void refreshDeliveryTable();
    void setLotsButtonsEnabled(bool enabled);
    void populateLotsTree(const QList<SectionInfo>& sections);

    // ── Верхняя панель ──────────────────────────────────────────────────────
    QLabel*      m_userLabel;
    QLabel*      m_statusLabel;
    QPushButton* m_startStopBtn;

    // ── Журнал ──────────────────────────────────────────────────────────────
    QPlainTextEdit* m_logOutput;

    // ── Лоты ────────────────────────────────────────────────────────────────
    QTreeWidget*         m_lotsTree;
    QPushButton*         m_refreshLotsBtn;
    QPushButton*         m_raiseNowBtn;
    QLabel*              m_lotsStatusLabel;
    QList<SectionInfo>   m_sections; // закешированные данные

    // ── Авто-ответчик ────────────────────────────────────────────────────────
    QTableWidget*            m_responderTable;
    QList<AutoResponderRule> m_responderRules;

    // ── Авто-выдача ──────────────────────────────────────────────────────────
    QTableWidget*       m_deliveryTable;
    QList<DeliveryItem> m_deliveryItems;

    // ── Настройки ────────────────────────────────────────────────────────────
    QCheckBox* m_autoRaiseCheck;
    QSpinBox*  m_raiseIntervalSpin;
    QCheckBox* m_autoResponderCheck;
    QCheckBox* m_autoDeliveryCheck;

    // ── Чаты ─────────────────────────────────────────────────────────────────
    QListWidget*          m_chatContactsList;
    QTextEdit*            m_chatHistory;
    QLineEdit*            m_chatInput;
    QPushButton*          m_sendMsgBtn;
    QLabel*               m_chatTitleLabel;
    QPushButton*          m_chatRefreshBtn;
    QList<ChatContact>    m_contacts;
    QString               m_currentChatNodeId;  // числовой node ID
    FunPayAPI::ChatNodeInfo m_currentChatInfo;   // всё для runner
    qint64                m_currentChatLastMsgId = 0;
    QList<ChatHistoryMsg> m_currentMessages;

    // ── Бот ──────────────────────────────────────────────────────────────────
    BotEngine* m_botEngine;
    UserInfo   m_userInfo;
    QString    m_cookieHeader;
    bool       m_botRunning = false;
};

#endif // DASHBOARDWINDOW_H
