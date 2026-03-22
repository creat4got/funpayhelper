#include "dashboardwindow.h"
#include "settingsmanager.h"
#include "funpayapi.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFrame>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QFont>
#include <QThread>
#include <QTreeWidgetItem>

// ─── Constructor ──────────────────────────────────────────────────────────────
DashboardWindow::DashboardWindow(const UserInfo& userInfo,
                                 const QString& cookieHeader,
                                 QWidget* parent)
    : QMainWindow(parent)
    , m_userInfo(userInfo)
    , m_cookieHeader(cookieHeader)
{
    setWindowTitle("BotFunPay — " + userInfo.username);
    resize(1020, 700);

    m_botEngine = new BotEngine(m_cookieHeader, this);
    connect(m_botEngine, &BotEngine::logMessage,    this, &DashboardWindow::onBotLog);
    connect(m_botEngine, &BotEngine::newMessage,    this, &DashboardWindow::onBotNewMessage);
    connect(m_botEngine, &BotEngine::newOrder,      this, &DashboardWindow::onBotNewOrder);
    connect(m_botEngine, &BotEngine::statusChanged, this, &DashboardWindow::onBotStatusChanged);
    connect(m_botEngine, &BotEngine::stopped,       this, &DashboardWindow::onBotStopped);
    connect(m_botEngine, &BotEngine::messageSent,   this, [this](const QString& nodeId, bool ok, const QString& err) {
        emit chatHistoryLoadError(nodeId, ok ? QString() : "Ошибка отправки: " + err);
    });

    // Сигналы лотов (из рабочих потоков → UI)
    connect(this, &DashboardWindow::lotsLoaded,        this, &DashboardWindow::onLotsLoaded,        Qt::QueuedConnection);
    connect(this, &DashboardWindow::lotsLoadError,     this, &DashboardWindow::onLotsLoadError,     Qt::QueuedConnection);
    connect(this, &DashboardWindow::raiseResult,       this, &DashboardWindow::onRaiseResult,       Qt::QueuedConnection);
    connect(this, &DashboardWindow::raiseAllFinished,  this, &DashboardWindow::onRaiseAllFinished,  Qt::QueuedConnection);
    connect(this, &DashboardWindow::chatsLoaded,       this, &DashboardWindow::onChatsLoaded,       Qt::QueuedConnection);
    connect(this, &DashboardWindow::chatsLoadError,    this, &DashboardWindow::onChatsLoadError,    Qt::QueuedConnection);
    connect(this, &DashboardWindow::chatHistoryLoaded, this, &DashboardWindow::onChatHistoryLoaded, Qt::QueuedConnection);
    connect(this, &DashboardWindow::chatHistoryLoadError,this,&DashboardWindow::onChatHistoryLoadError,Qt::QueuedConnection);

    setupUi();

    auto* s = SettingsManager::instance();
    m_autoRaiseCheck->setChecked(s->autoRaiseEnabled());
    m_raiseIntervalSpin->setValue(s->raiseIntervalMinutes());
    m_autoResponderCheck->setChecked(s->autoResponderEnabled());
    m_autoDeliveryCheck->setChecked(s->autoDeliveryEnabled());

    m_responderRules = s->responderRules();
    m_deliveryItems  = s->deliveryItems();
    refreshResponderTable();
    refreshDeliveryTable();
}

DashboardWindow::~DashboardWindow()
{
    if (m_botRunning)
        m_botEngine->stop();
}

// ─── UI setup ─────────────────────────────────────────────────────────────────
void DashboardWindow::setupUi()
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* root = new QVBoxLayout(central);
    root->setSpacing(6);
    root->setContentsMargins(8, 8, 8, 8);

    // Верхняя панель
    QWidget* bar = new QWidget;
    QHBoxLayout* barL = new QHBoxLayout(bar);
    barL->setContentsMargins(0, 0, 0, 0);

    m_userLabel = new QLabel(QString("Пользователь: %1  |  ID: %2")
                             .arg(m_userInfo.username, m_userInfo.userId));
    QFont uf = m_userLabel->font();
    uf.setBold(true);
    m_userLabel->setFont(uf);

    m_statusLabel = new QLabel("Статус: Остановлен");
    m_statusLabel->setStyleSheet("color:#888888;");

    m_startStopBtn = new QPushButton("Запустить бота");
    m_startStopBtn->setFixedWidth(160);
    m_startStopBtn->setStyleSheet(
        "QPushButton{background:#27ae60;color:white;border-radius:4px;"
        "padding:5px 10px;font-weight:bold;}"
        "QPushButton:hover{background:#2ecc71;}"
        "QPushButton:disabled{background:#aaaaaa;}");
    connect(m_startStopBtn, &QPushButton::clicked,
            this, &DashboardWindow::onStartStopClicked);

    barL->addWidget(m_userLabel);
    barL->addStretch();
    barL->addWidget(m_statusLabel);
    barL->addSpacing(16);
    barL->addWidget(m_startStopBtn);

    QFrame* sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);

    root->addWidget(bar);
    root->addWidget(sep);

    QTabWidget* tabs = new QTabWidget;
    tabs->addTab(createLogsTab(),          "Журнал");
    tabs->addTab(createLotsTab(),          "Лоты");
    tabs->addTab(createChatTab(),          "Сообщения");
    tabs->addTab(createAutoResponderTab(), "Авто-ответчик");
    tabs->addTab(createAutoDeliveryTab(),  "Авто-выдача");
    tabs->addTab(createSettingsTab(),      "Настройки");
    root->addWidget(tabs, 1);
}

// ── Журнал ────────────────────────────────────────────────────────────────────
QWidget* DashboardWindow::createLogsTab()
{
    QWidget* w = new QWidget;
    QVBoxLayout* l = new QVBoxLayout(w);

    m_logOutput = new QPlainTextEdit;
    m_logOutput->setReadOnly(true);
    m_logOutput->setMaximumBlockCount(3000);
    QFont f("Monospace");
    f.setStyleHint(QFont::TypeWriter);
    f.setPointSize(9);
    m_logOutput->setFont(f);

    QPushButton* clearBtn = new QPushButton("Очистить");
    clearBtn->setFixedWidth(100);
    connect(clearBtn, &QPushButton::clicked, m_logOutput, &QPlainTextEdit::clear);

    l->addWidget(m_logOutput, 1);
    QHBoxLayout* bl = new QHBoxLayout;
    bl->addStretch();
    bl->addWidget(clearBtn);
    l->addLayout(bl);
    return w;
}

// ── Лоты ─────────────────────────────────────────────────────────────────────
QWidget* DashboardWindow::createLotsTab()
{
    QWidget* w = new QWidget;
    QVBoxLayout* l = new QVBoxLayout(w);

    // Toolbar
    QHBoxLayout* tbar = new QHBoxLayout;
    m_refreshLotsBtn = new QPushButton("Обновить список");
    m_raiseNowBtn    = new QPushButton("Поднять все лоты");
    m_raiseNowBtn->setStyleSheet(
        "QPushButton{background:#2980b9;color:white;border-radius:4px;"
        "padding:4px 10px;font-weight:bold;}"
        "QPushButton:hover{background:#3498db;}"
        "QPushButton:disabled{background:#aaaaaa;}");
    m_raiseNowBtn->setEnabled(false);

    connect(m_refreshLotsBtn, &QPushButton::clicked, this, &DashboardWindow::onRefreshLots);
    connect(m_raiseNowBtn,    &QPushButton::clicked, this, &DashboardWindow::onRaiseNow);

    m_lotsStatusLabel = new QLabel("Нажмите «Обновить» для загрузки лотов");
    m_lotsStatusLabel->setStyleSheet("color:#666666;");

    tbar->addWidget(m_refreshLotsBtn);
    tbar->addWidget(m_raiseNowBtn);
    tbar->addStretch();
    tbar->addWidget(m_lotsStatusLabel);

    // Дерево: корневые узлы = секции, дочерние = лоты
    m_lotsTree = new QTreeWidget;
    m_lotsTree->setColumnCount(5);
    m_lotsTree->setHeaderLabels({"Секция / Описание", "Цена, ₽", "Lot ID", "Node ID", "Game ID"});
    m_lotsTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_lotsTree->header()->resizeSection(1, 90);
    m_lotsTree->header()->resizeSection(2, 90);
    m_lotsTree->header()->resizeSection(3, 80);
    m_lotsTree->header()->resizeSection(4, 80);
    m_lotsTree->setAlternatingRowColors(true);
    m_lotsTree->setRootIsDecorated(true);

    l->addLayout(tbar);
    l->addWidget(m_lotsTree, 1);
    return w;
}

// ── Авто-ответчик ─────────────────────────────────────────────────────────────
QWidget* DashboardWindow::createAutoResponderTab()
{
    QWidget* w = new QWidget;
    QVBoxLayout* l = new QVBoxLayout(w);

    QLabel* hint = new QLabel(
        "Ключевое слово: пусто = ответ на любое сообщение. "
        "Плейсхолдер %username% заменяется на имя покупателя.");
    hint->setWordWrap(true);
    hint->setStyleSheet("color:#666666; padding:4px;");

    QHBoxLayout* tbar = new QHBoxLayout;
    QPushButton* addBtn  = new QPushButton("Добавить");
    QPushButton* delBtn  = new QPushButton("Удалить");
    QPushButton* saveBtn = new QPushButton("Сохранить");
    connect(addBtn,  &QPushButton::clicked, this, &DashboardWindow::onAddRule);
    connect(delBtn,  &QPushButton::clicked, this, &DashboardWindow::onDeleteRule);
    connect(saveBtn, &QPushButton::clicked, this, &DashboardWindow::onSaveRules);
    tbar->addWidget(addBtn); tbar->addWidget(delBtn);
    tbar->addStretch(); tbar->addWidget(saveBtn);

    m_responderTable = new QTableWidget(0, 4);
    m_responderTable->setHorizontalHeaderLabels(
        {"Ключевое слово", "Ответ", "Точное совп.", "Вкл"});
    m_responderTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_responderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_responderTable->setAlternatingRowColors(true);

    l->addWidget(hint);
    l->addLayout(tbar);
    l->addWidget(m_responderTable, 1);
    return w;
}

// ── Авто-выдача ───────────────────────────────────────────────────────────────
QWidget* DashboardWindow::createAutoDeliveryTab()
{
    QWidget* w = new QWidget;
    QVBoxLayout* l = new QVBoxLayout(w);

    QLabel* hint = new QLabel(
        "При новом заказе бот отправит текст товара покупателю. "
        "Lot ID берётся из вкладки Лоты.");
    hint->setWordWrap(true);
    hint->setStyleSheet("color:#666666; padding:4px;");

    QHBoxLayout* tbar = new QHBoxLayout;
    QPushButton* addBtn  = new QPushButton("Добавить");
    QPushButton* delBtn  = new QPushButton("Удалить");
    QPushButton* saveBtn = new QPushButton("Сохранить");
    connect(addBtn,  &QPushButton::clicked, this, &DashboardWindow::onAddDelivery);
    connect(delBtn,  &QPushButton::clicked, this, &DashboardWindow::onDeleteDelivery);
    connect(saveBtn, &QPushButton::clicked, this, &DashboardWindow::onSaveDelivery);
    tbar->addWidget(addBtn); tbar->addWidget(delBtn);
    tbar->addStretch(); tbar->addWidget(saveBtn);

    m_deliveryTable = new QTableWidget(0, 4);
    m_deliveryTable->setHorizontalHeaderLabels(
        {"Lot ID", "Название лота", "Текст товара", "Вкл"});
    m_deliveryTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_deliveryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_deliveryTable->setAlternatingRowColors(true);

    l->addWidget(hint);
    l->addLayout(tbar);
    l->addWidget(m_deliveryTable, 1);
    return w;
}

// ── Настройки ─────────────────────────────────────────────────────────────────
QWidget* DashboardWindow::createSettingsTab()
{
    QWidget* w = new QWidget;
    QVBoxLayout* root = new QVBoxLayout(w);
    root->setAlignment(Qt::AlignTop);

    QGroupBox* raiseGb = new QGroupBox("Авто-поднятие лотов");
    QFormLayout* rfl   = new QFormLayout(raiseGb);

    m_autoRaiseCheck = new QCheckBox("Включить авто-поднятие");
    m_raiseIntervalSpin = new QSpinBox;
    m_raiseIntervalSpin->setRange(1, 1440);
    m_raiseIntervalSpin->setSuffix(" мин.");
    m_raiseIntervalSpin->setValue(180);

    QLabel* raiseHint = new QLabel(
        "Game ID и Node ID загружаются автоматически со страницы профиля.\n"
        "Нажмите «Обновить список» во вкладке Лоты перед запуском бота.");
    raiseHint->setStyleSheet("color:#666666; font-size:11px;");
    raiseHint->setWordWrap(true);

    rfl->addRow("Состояние:", m_autoRaiseCheck);
    rfl->addRow("Интервал:",  m_raiseIntervalSpin);
    rfl->addRow("",           raiseHint);

    QGroupBox* botGb = new QGroupBox("Функции бота");
    QVBoxLayout* bvl = new QVBoxLayout(botGb);
    m_autoResponderCheck = new QCheckBox("Авто-ответчик");
    m_autoDeliveryCheck  = new QCheckBox("Авто-выдача");
    bvl->addWidget(m_autoResponderCheck);
    bvl->addWidget(m_autoDeliveryCheck);

    QPushButton* saveBtn = new QPushButton("Сохранить настройки");
    saveBtn->setFixedWidth(180);
    connect(saveBtn, &QPushButton::clicked, this, &DashboardWindow::onSaveSettings);

    root->addWidget(raiseGb);
    root->addSpacing(8);
    root->addWidget(botGb);
    root->addSpacing(12);
    QHBoxLayout* bl = new QHBoxLayout;
    bl->addStretch(); bl->addWidget(saveBtn);
    root->addLayout(bl);
    return w;
}

// ─── Bot control ──────────────────────────────────────────────────────────────
void DashboardWindow::onStartStopClicked()
{
    if (!m_botRunning) {
        m_botRunning = true;
        m_startStopBtn->setText("Остановить бота");
        m_startStopBtn->setStyleSheet(
            "QPushButton{background:#c0392b;color:white;border-radius:4px;"
            "padding:5px 10px;font-weight:bold;}"
            "QPushButton:hover{background:#e74c3c;}"
            "QPushButton:disabled{background:#aaaaaa;}");
        m_startStopBtn->setEnabled(false);
        m_botEngine->start();
    } else {
        m_startStopBtn->setEnabled(false);
        m_botEngine->stop();
    }
}

void DashboardWindow::onBotStopped()
{
    m_botRunning = false;
    m_startStopBtn->setText("Запустить бота");
    m_startStopBtn->setStyleSheet(
        "QPushButton{background:#27ae60;color:white;border-radius:4px;"
        "padding:5px 10px;font-weight:bold;}"
        "QPushButton:hover{background:#2ecc71;}"
        "QPushButton:disabled{background:#aaaaaa;}");
    m_startStopBtn->setEnabled(true);
}

void DashboardWindow::onBotStatusChanged(const QString& status)
{
    m_statusLabel->setText("Статус: " + status);
    if (status == "Работает") {
        m_statusLabel->setStyleSheet("color:#27ae60;font-weight:bold;");
        m_startStopBtn->setEnabled(true);
    } else if (status == "Ошибка") {
        m_statusLabel->setStyleSheet("color:#e74c3c;");
        onBotStopped();
    } else {
        m_statusLabel->setStyleSheet("color:#888888;");
    }
}

void DashboardWindow::onBotLog(int level, const QString& msg)
{
    QString ts = QDateTime::currentDateTime().toString("hh:mm:ss");
    appendLog(level, ts, msg);
}

void DashboardWindow::onBotNewMessage(const ChatMessage& msg)
{
    // Обновить превью в списке контактов
    for (ChatContact& c : m_contacts) {
        if (c.nodeId == msg.nodeId) {
            c.lastMessage = msg.text;
            break;
        }
    }
    refreshContactsList();

    // Если этот чат сейчас открыт — добавить в историю и перерендерить
    if (m_currentChatNodeId == msg.nodeId) {
        ChatHistoryMsg hm;
        hm.author      = msg.isMyMessage ? m_userInfo.username : msg.chatTitle;
        hm.text        = msg.text;
        hm.time        = QDateTime::currentDateTime().toString("hh:mm:ss");
        hm.isMyMessage = msg.isMyMessage;
        m_currentMessages.append(hm);
        renderChatHistory();
    }
}

void DashboardWindow::onBotNewOrder(const OrderInfo&) {}

// ─── Chat tab ─────────────────────────────────────────────────────────────────
QWidget* DashboardWindow::createChatTab()
{
    QWidget* w = new QWidget;
    QVBoxLayout* vl = new QVBoxLayout(w);
    vl->setContentsMargins(4, 4, 4, 4);
    vl->setSpacing(4);

    // Верхняя панель
    m_chatRefreshBtn = new QPushButton("Обновить список");
    m_chatRefreshBtn->setFixedWidth(160);
    connect(m_chatRefreshBtn, &QPushButton::clicked,
            this, &DashboardWindow::onRefreshChats);

    QHBoxLayout* topBar = new QHBoxLayout;
    topBar->addWidget(m_chatRefreshBtn);
    topBar->addStretch();
    vl->addLayout(topBar);

    // Горизонтальный сплиттер: список чатов | история + ввод
    QSplitter* splitter = new QSplitter(Qt::Horizontal);

    // ── Левая панель: список контактов ──────────────────────────────────────
    m_chatContactsList = new QListWidget;
    m_chatContactsList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_chatContactsList->setMinimumWidth(180);
    m_chatContactsList->setMaximumWidth(260);
    connect(m_chatContactsList, &QListWidget::itemClicked,
            this, &DashboardWindow::onChatContactSelected);
    splitter->addWidget(m_chatContactsList);

    // ── Правая панель: история + форма ввода ────────────────────────────────
    QWidget* rightPanel = new QWidget;
    QVBoxLayout* rightVl = new QVBoxLayout(rightPanel);
    rightVl->setContentsMargins(4, 0, 0, 0);
    rightVl->setSpacing(4);

    m_chatTitleLabel = new QLabel("Выберите чат слева");
    QFont tf = m_chatTitleLabel->font();
    tf.setBold(true);
    tf.setPointSize(tf.pointSize() + 1);
    m_chatTitleLabel->setFont(tf);

    QFrame* sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);

    m_chatHistory = new QTextEdit;
    m_chatHistory->setReadOnly(true);
    m_chatHistory->setStyleSheet("background:#f8f9fa;");

    // Строка ввода
    QHBoxLayout* inputRow = new QHBoxLayout;
    m_chatInput = new QLineEdit;
    m_chatInput->setPlaceholderText("Введите сообщение…");
    m_chatInput->setEnabled(false);
    connect(m_chatInput, &QLineEdit::returnPressed,
            this, &DashboardWindow::onSendChatMessage);

    m_sendMsgBtn = new QPushButton("Отправить");
    m_sendMsgBtn->setEnabled(false);
    m_sendMsgBtn->setStyleSheet(
        "QPushButton{background:#0d6efd;color:white;border-radius:4px;"
        "padding:5px 12px;font-weight:bold;}"
        "QPushButton:hover{background:#0b5ed7;}"
        "QPushButton:disabled{background:#aaaaaa;}");
    connect(m_sendMsgBtn, &QPushButton::clicked,
            this, &DashboardWindow::onSendChatMessage);

    inputRow->addWidget(m_chatInput, 1);
    inputRow->addWidget(m_sendMsgBtn);

    rightVl->addWidget(m_chatTitleLabel);
    rightVl->addWidget(sep);
    rightVl->addWidget(m_chatHistory, 1);
    rightVl->addLayout(inputRow);

    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({220, 600});

    vl->addWidget(splitter, 1);
    return w;
}

// ── Обновить список чатов ─────────────────────────────────────────────────────
void DashboardWindow::onRefreshChats()
{
    m_chatRefreshBtn->setEnabled(false);
    m_chatContactsList->clear();
    QListWidgetItem* loadItem = new QListWidgetItem("Загрузка…");
    loadItem->setForeground(Qt::gray);
    m_chatContactsList->addItem(loadItem);

    QString cookie = m_cookieHeader;
    QThread* t = QThread::create([this, cookie]() {
        FunPayAPI api(cookie);
        QString err;
        if (!api.init(err)) { emit chatsLoadError(err); return; }
        QList<ChatContact> contacts;
        if (!api.fetchChatContacts(contacts, err)) { emit chatsLoadError(err); return; }
        emit chatsLoaded(contacts);
    });
    connect(t, &QThread::finished, t, &QObject::deleteLater);
    t->start();
}

void DashboardWindow::onChatsLoaded(QList<ChatContact> contacts)
{
    m_contacts = contacts;
    m_chatRefreshBtn->setEnabled(true);
    refreshContactsList();
}

void DashboardWindow::onChatsLoadError(QString error)
{
    m_chatRefreshBtn->setEnabled(true);
    m_chatContactsList->clear();
    QListWidgetItem* item = new QListWidgetItem("Ошибка: " + error);
    item->setForeground(Qt::red);
    m_chatContactsList->addItem(item);
}

void DashboardWindow::refreshContactsList()
{
    // Запоминаем текущее выделение
    QString selNodeId = m_currentChatNodeId;
    m_chatContactsList->clear();

    for (const ChatContact& c : m_contacts) {
        QString preview = c.username;
        if (!c.time.isEmpty())
            preview += "  " + c.time;
        if (!c.lastMessage.isEmpty())
            preview += "\n" + c.lastMessage.left(60);

        QListWidgetItem* item = new QListWidgetItem(preview);
        item->setData(Qt::UserRole, c.nodeId);
        item->setToolTip(c.username);

        // Подсветить активный чат
        if (c.nodeId == selNodeId)
            item->setBackground(QColor("#d0e8ff"));

        m_chatContactsList->addItem(item);
    }
}

// ── Загрузить историю выбранного чата ─────────────────────────────────────────
void DashboardWindow::onChatContactSelected(QListWidgetItem* item)
{
    QString nodeId = item->data(Qt::UserRole).toString();
    if (nodeId.isEmpty()) return;

    m_currentChatNodeId = nodeId;

    // Найти имя контакта
    QString username;
    for (const ChatContact& c : m_contacts) {
        if (c.nodeId == nodeId) { username = c.username; break; }
    }
    m_chatTitleLabel->setText("Чат с " + username + "  (node " + nodeId + ")");
    m_chatHistory->setHtml("<p style='color:#888;'>Загрузка истории…</p>");
    m_chatInput->setEnabled(false);
    m_sendMsgBtn->setEnabled(false);

    refreshContactsList(); // обновить подсветку

    QString cookie = m_cookieHeader;
    QThread* t = QThread::create([this, cookie, nodeId]() {
        FunPayAPI api(cookie);
        QString err;
        if (!api.init(err)) { emit chatHistoryLoadError(nodeId, err); return; }

        // Получить актуальные runner-теги (ordersCountersTag, bookmarksTag)
        // Без них сервер отклоняет запрос отправки с HTTP 400
        QList<RunnerEvent> dummyEvents;
        api.poll(dummyEvents, err); // ошибку игнорируем — теги нужны, но не критичны

        QList<ChatHistoryMsg> messages;
        FunPayAPI::ChatNodeInfo info;
        if (!api.fetchChatHistory(nodeId, messages, info, err)) {
            emit chatHistoryLoadError(nodeId, err); return;
        }
        qint64 lastMsgId = messages.isEmpty() ? 0 : messages.last().id;
        emit chatHistoryLoaded(nodeId, info, lastMsgId, messages);
    });
    connect(t, &QThread::finished, t, &QObject::deleteLater);
    t->start();
}

void DashboardWindow::onChatHistoryLoaded(QString nodeId, FunPayAPI::ChatNodeInfo info,
                                           qint64 lastMsgId, QList<ChatHistoryMsg> messages)
{
    if (nodeId != m_currentChatNodeId) return;

    m_currentChatInfo      = info;
    m_currentChatLastMsgId = lastMsgId;
    m_currentMessages      = messages;

    renderChatHistory();

    m_chatInput->setEnabled(true);
    m_sendMsgBtn->setEnabled(true);
    m_chatInput->setFocus();
}

void DashboardWindow::onChatHistoryLoadError(QString nodeId, QString error)
{
    if (nodeId != m_currentChatNodeId) return;

    // Пустая строка = сигнал успешной отправки (разблокировать UI)
    if (error.isEmpty()) {
        m_chatInput->setEnabled(true);
        m_sendMsgBtn->setEnabled(true);
        return;
    }

    // Различаем: загрузка истории vs отправка сообщения
    // Если история уже есть — это ошибка отправки
    if (!m_currentMessages.isEmpty()) {
        // Показать ошибку в чате как системное сообщение
        ChatHistoryMsg errMsg;
        errMsg.isSystem = true;
        errMsg.text     = "⚠ " + error;
        m_currentMessages.append(errMsg);
        renderChatHistory();
        m_chatInput->setEnabled(true);
        m_sendMsgBtn->setEnabled(true);
    } else {
        // Ошибка загрузки истории
        m_chatHistory->setHtml(
            "<p style='color:red; margin:12px;'>Ошибка загрузки истории: " +
            error.toHtmlEscaped() + "</p>");
        m_chatInput->setEnabled(false);
        m_sendMsgBtn->setEnabled(false);
    }
}

// ── Отправить сообщение ───────────────────────────────────────────────────────
void DashboardWindow::onSendChatMessage()
{
    QString text = m_chatInput->text().trimmed();
    if (text.isEmpty() || m_currentChatNodeId.isEmpty())
        return;

    m_chatInput->clear();
    m_chatInput->setEnabled(false);
    m_sendMsgBtn->setEnabled(false);

    QString nodeId = m_currentChatNodeId;
    QString cookie = m_cookieHeader;
    FunPayAPI::ChatNodeInfo info = m_currentChatInfo;
    qint64 lastMsgId = m_currentChatLastMsgId;
    QList<ChatContact> contacts = m_contacts;

    // Сразу показываем сообщение в UI
    ChatHistoryMsg myMsg;
    myMsg.author = m_userInfo.username;
    myMsg.text = text;
    myMsg.time = QDateTime::currentDateTime().toString("hh:mm:ss");
    myMsg.isMyMessage = true;
    m_currentMessages.append(myMsg);
    renderChatHistory();

    // Если бот запущен — используем его API (актуальный CSRF-токен).
    // Иначе создаём временный API (бот не запущен, конкурирующего poll нет).
    if (m_botEngine && m_botEngine->isRunning()) {
        m_botEngine->sendChatMessage(nodeId, text);
    } else {
        QThread* t = QThread::create([this, cookie, nodeId, info, lastMsgId, contacts, text]() {
            FunPayAPI api(cookie);
            QString err;
            if (!api.init(err)) {
                emit chatHistoryLoadError(nodeId, "Ошибка отправки: " + err);
                return;
            }
            QList<RunnerEvent> events;
            QString pollErr;
            api.poll(events, pollErr);

            QString sendErr;
            if (!api.sendMessageNode(info, lastMsgId, nodeId, text, contacts, sendErr)) {
                emit chatHistoryLoadError(nodeId, "Ошибка отправки: " + sendErr);
            } else {
                emit chatHistoryLoadError(nodeId, QString());
            }
        });
        connect(t, &QThread::finished, t, &QObject::deleteLater);
        t->start();
    }
}

// ── Рендер истории: группировка по автору ─────────────────────────────────────
// Дизайн: имя жирным слева, время серым справа, сообщения ниже — без цветов
void DashboardWindow::renderChatHistory()
{
    QString html =
        "<html><body style='font-family:Arial,sans-serif;"
        " font-size:13px; margin:10px 14px; background:#ffffff; color:#1a1a1a;'>";

    int i = 0;
    const int n = m_currentMessages.size();
    while (i < n) {
        const ChatHistoryMsg& first = m_currentMessages[i];

        if (first.isSystem) {
            html += QString(
                "<p style='text-align:center; color:#bbb; font-size:11px;"
                " margin:8px 0; font-style:italic;'>%1</p>")
                .arg(first.text.toHtmlEscaped());
            ++i;
            continue;
        }

        const QString& author = first.author;

        // Заголовок группы: имя жирным слева, время серым справа
        // <table> — единственный надёжный способ в Qt rich text
        html += QString(
            "<table width='100%%' cellpadding='0' cellspacing='0'"
            " style='margin-top:12px; margin-bottom:1px;'><tr>"
            "<td><b>%1</b></td>"
            "<td align='right'>"
            "<span style='color:#bbb; font-size:11px;'>%2</span>"
            "</td></tr></table>")
            .arg(author.toHtmlEscaped(),
                 first.time.toHtmlEscaped());

        // Сообщения группы: явный text-align:left чтобы нейтрализовать
        // эффект align="right" из <td> таблицы заголовка
        while (i < n
               && !m_currentMessages[i].isSystem
               && m_currentMessages[i].author == author)
        {
            html += QString("<p align='left' style='margin:0 0 1px 0; text-align:left;'>%1</p>")
                .arg(m_currentMessages[i].text.toHtmlEscaped()
                                              .replace("\n", "<br>"));
            ++i;
        }
    }

    html += "</body></html>";
    m_chatHistory->setHtml(html);
    m_chatHistory->verticalScrollBar()->setValue(
        m_chatHistory->verticalScrollBar()->maximum());
}

// ─── Log ──────────────────────────────────────────────────────────────────────
void DashboardWindow::appendLog(int level, const QString& ts, const QString& msg)
{
    static const char* colors[]   = {"#808080","#00aa00","#aaaa00","#cc0000"};
    static const char* prefixes[] = {"[INF]","[OK ]","[WRN]","[ERR]"};
    int idx = qBound(0, level, 3);

    QString html = QString(
        "<span style='color:#888888;'>%1</span> "
        "<span style='color:%2;font-weight:%3;'>%4 %5</span>")
        .arg(ts, colors[idx],
             idx >= 2 ? "bold" : "normal",
             prefixes[idx],
             msg.toHtmlEscaped());

    m_logOutput->appendHtml(html);
    m_logOutput->verticalScrollBar()->setValue(
        m_logOutput->verticalScrollBar()->maximum());
}

// ─── Lots: загрузка ───────────────────────────────────────────────────────────
void DashboardWindow::onRefreshLots()
{
    setLotsButtonsEnabled(false);
    m_lotsStatusLabel->setText("Загрузка лотов…");
    m_lotsTree->clear();

    // Запускаем в отдельном потоке чтобы не блокировать GUI
    QString cookieHeader = m_cookieHeader;
    UserInfo userInfo    = m_userInfo;

    QThread* t = QThread::create([this, cookieHeader, userInfo]() {
        FunPayAPI api(cookieHeader);
        // Копируем данные инициализации без лишнего HTTP запроса
        // (используем уже известные userId и csrfToken)
        QString err;
        if (!api.init(err)) {
            emit lotsLoadError("Ошибка инициализации: " + err);
            return;
        }

        QList<SectionInfo> sections;
        if (!api.fetchUserOffers(sections, err)) {
            emit lotsLoadError(err);
            return;
        }

        // Подгружаем game_id для каждой секции
        QString gameIdErr;
        api.fetchSectionGameIds(sections, gameIdErr); // partial errors не фатальны

        emit lotsLoaded(sections);
    });

    connect(t, &QThread::finished, t, &QObject::deleteLater);
    t->start();
}

void DashboardWindow::onLotsLoaded(QList<SectionInfo> sections)
{
    m_sections = sections;
    populateLotsTree(sections);

    int totalLots = 0;
    for (const auto& s : sections) totalLots += s.lots.size();

    m_lotsStatusLabel->setText(
        QString("Секций: %1  |  Лотов: %2")
        .arg(sections.size()).arg(totalLots));

    setLotsButtonsEnabled(true);
    m_raiseNowBtn->setEnabled(!sections.isEmpty());

    onBotLog(1, QString("Лоты загружены: %1 секций, %2 лотов")
             .arg(sections.size()).arg(totalLots));
}

void DashboardWindow::onLotsLoadError(QString error)
{
    m_lotsStatusLabel->setText("Ошибка: " + error);
    setLotsButtonsEnabled(true);
    onBotLog(3, "Ошибка загрузки лотов: " + error);
}

void DashboardWindow::populateLotsTree(const QList<SectionInfo>& sections)
{
    m_lotsTree->clear();

    for (const SectionInfo& sec : sections) {
        // Корневой узел = секция
        QTreeWidgetItem* root = new QTreeWidgetItem(m_lotsTree);
        root->setText(0, sec.name);
        root->setText(1, "");
        root->setText(2, "");
        root->setText(3, sec.nodeId);
        root->setText(4, sec.gameId.isEmpty() ? "—" : sec.gameId);

        // Полужирный шрифт для секций
        QFont bf = root->font(0);
        bf.setBold(true);
        root->setFont(0, bf);
        root->setForeground(0, QBrush(QColor("#2c3e50")));
        root->setBackground(0, QBrush(QColor("#ecf0f1")));
        root->setBackground(1, QBrush(QColor("#ecf0f1")));
        root->setBackground(2, QBrush(QColor("#ecf0f1")));
        root->setBackground(3, QBrush(QColor("#ecf0f1")));
        root->setBackground(4, QBrush(QColor("#ecf0f1")));

        // Дочерние узлы = лоты
        for (const LotInfo& lot : sec.lots) {
            QTreeWidgetItem* item = new QTreeWidgetItem(root);
            item->setText(0, lot.description.isEmpty() ? "(без описания)" : lot.description);
            item->setText(1, QString::number(lot.price, 'f', 2));
            item->setText(2, lot.id);
            item->setText(3, lot.nodeId);
            item->setText(4, lot.gameId.isEmpty() ? sec.gameId : lot.gameId);
        }

        root->setExpanded(true);
    }
}

void DashboardWindow::setLotsButtonsEnabled(bool enabled)
{
    m_refreshLotsBtn->setEnabled(enabled);
    // Кнопка поднятия включается только когда есть данные
    if (enabled)
        m_raiseNowBtn->setEnabled(!m_sections.isEmpty());
    else
        m_raiseNowBtn->setEnabled(false);
}

// ─── Lots: поднятие ───────────────────────────────────────────────────────────
void DashboardWindow::onRaiseNow()
{
    if (m_sections.isEmpty()) {
        QMessageBox::information(this, "Поднятие",
            "Сначала нажмите «Обновить список» для загрузки лотов.");
        return;
    }

    setLotsButtonsEnabled(false);
    m_lotsStatusLabel->setText("Поднятие лотов…");
    onBotLog(0, "[Поднятие] Запуск поднятия для всех секций…");

    // Копируем данные в поток
    QList<SectionInfo> sections = m_sections;
    QString cookieHeader        = m_cookieHeader;
    UserInfo userInfo           = m_userInfo;

    QThread* t = QThread::create([this, sections, cookieHeader, userInfo]() {
        FunPayAPI api(cookieHeader);
        QString err;
        if (!api.init(err)) {
            emit lotsLoadError("Ошибка инициализации для поднятия: " + err);
            emit raiseAllFinished();
            return;
        }

        for (const SectionInfo& sec : sections) {
            if (sec.gameId.isEmpty()) {
                emit raiseResult(sec.nodeId, false,
                    QString("Пропущено (game_id не загружен): %1").arg(sec.name));
                continue;
            }

            QString resultMsg, raiseErr;
            bool ok = api.raiseSection(sec.gameId, sec.nodeId, resultMsg, raiseErr);
            emit raiseResult(sec.nodeId, ok,
                ok ? resultMsg : raiseErr);
        }

        emit raiseAllFinished();
    });

    connect(t, &QThread::finished, t, &QObject::deleteLater);
    t->start();
}

void DashboardWindow::onRaiseResult(QString nodeId, bool ok, QString msg)
{
    // Находим секцию по nodeId чтобы показать имя
    QString name = nodeId;
    for (const auto& s : m_sections)
        if (s.nodeId == nodeId) { name = s.name; break; }

    if (ok) {
        onBotLog(1, QString("[Поднятие] %1 (node=%2): %3").arg(name, nodeId, msg));
    } else {
        // "wait" — кулдаун, не ошибка
        bool isCooldown = msg.toLower().contains("wait") || msg.toLower().contains("час");
        int level = isCooldown ? 2 : 3;
        onBotLog(level, QString("[Поднятие] %1 (node=%2): %3").arg(name, nodeId, msg));
    }
}

void DashboardWindow::onRaiseAllFinished()
{
    m_lotsStatusLabel->setText(
        QString("Секций: %1  |  Поднятие завершено").arg(m_sections.size()));
    setLotsButtonsEnabled(true);
    onBotLog(0, "[Поднятие] Обработка всех секций завершена");
}

// ─── Auto-responder ───────────────────────────────────────────────────────────
void DashboardWindow::refreshResponderTable()
{
    m_responderTable->setRowCount(m_responderRules.size());
    for (int i = 0; i < m_responderRules.size(); ++i) {
        const auto& r = m_responderRules[i];
        m_responderTable->setItem(i, 0, new QTableWidgetItem(r.trigger));
        m_responderTable->setItem(i, 1, new QTableWidgetItem(r.response));
        m_responderTable->setItem(i, 2, new QTableWidgetItem(r.matchExact ? "Да" : "Нет"));
        m_responderTable->setItem(i, 3, new QTableWidgetItem(r.enabled    ? "Да" : "Нет"));
    }
}

void DashboardWindow::onAddRule()
{
    bool ok;
    QString trigger = QInputDialog::getText(this, "Ключевое слово",
        "Ключевое слово (пусто = любое сообщение):",
        QLineEdit::Normal, "", &ok);
    if (!ok) return;

    QString response = QInputDialog::getText(this, "Текст ответа",
        "Текст ответа (%username% = имя покупателя):",
        QLineEdit::Normal, "", &ok);
    if (!ok || response.isEmpty()) return;

    AutoResponderRule rule;
    rule.trigger  = trigger.trimmed();
    rule.response = response;
    rule.enabled  = true;
    m_responderRules.append(rule);
    refreshResponderTable();
}

void DashboardWindow::onDeleteRule()
{
    int row = m_responderTable->currentRow();
    if (row < 0 || row >= m_responderRules.size()) return;
    m_responderRules.removeAt(row);
    refreshResponderTable();
}

void DashboardWindow::onSaveRules()
{
    SettingsManager::instance()->setResponderRules(m_responderRules);
    onBotLog(1, QString("Правила авто-ответчика сохранены (%1 шт.)").arg(m_responderRules.size()));
}

// ─── Auto-delivery ────────────────────────────────────────────────────────────
void DashboardWindow::refreshDeliveryTable()
{
    m_deliveryTable->setRowCount(m_deliveryItems.size());
    for (int i = 0; i < m_deliveryItems.size(); ++i) {
        const auto& d = m_deliveryItems[i];
        m_deliveryTable->setItem(i, 0, new QTableWidgetItem(d.lotId));
        m_deliveryTable->setItem(i, 1, new QTableWidgetItem(d.lotTitle));
        m_deliveryTable->setItem(i, 2, new QTableWidgetItem(d.deliveryText));
        m_deliveryTable->setItem(i, 3, new QTableWidgetItem(d.enabled ? "Да" : "Нет"));
    }
}

void DashboardWindow::onAddDelivery()
{
    bool ok;
    QString lotId = QInputDialog::getText(this, "Lot ID",
        "ID лота (из вкладки Лоты):", QLineEdit::Normal, "", &ok);
    if (!ok || lotId.isEmpty()) return;

    QString title = QInputDialog::getText(this, "Название лота",
        "Название лота (для справки):", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    QString text = QInputDialog::getMultiLineText(this, "Товар",
        "Текст, который получит покупатель:", "", &ok);
    if (!ok || text.isEmpty()) return;

    DeliveryItem item;
    item.lotId        = lotId.trimmed();
    item.lotTitle     = title.trimmed();
    item.deliveryText = text;
    item.enabled      = true;
    m_deliveryItems.append(item);
    refreshDeliveryTable();
}

void DashboardWindow::onDeleteDelivery()
{
    int row = m_deliveryTable->currentRow();
    if (row < 0 || row >= m_deliveryItems.size()) return;
    m_deliveryItems.removeAt(row);
    refreshDeliveryTable();
}

void DashboardWindow::onSaveDelivery()
{
    SettingsManager::instance()->setDeliveryItems(m_deliveryItems);
    onBotLog(1, QString("Позиции авто-выдачи сохранены (%1 шт.)").arg(m_deliveryItems.size()));
}

// ─── Settings ─────────────────────────────────────────────────────────────────
void DashboardWindow::onSaveSettings()
{
    auto* s = SettingsManager::instance();
    s->setAutoRaiseEnabled(m_autoRaiseCheck->isChecked());
    s->setRaiseIntervalMinutes(m_raiseIntervalSpin->value());
    s->setAutoResponderEnabled(m_autoResponderCheck->isChecked());
    s->setAutoDeliveryEnabled(m_autoDeliveryCheck->isChecked());
    onBotLog(1, "Настройки сохранены. Перезапустите бота для применения.");
}
