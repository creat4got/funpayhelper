#include "botengine.h"

BotEngine::BotEngine(const QString& cookieHeader, QObject* parent)
    : QObject(parent), m_cookieHeader(cookieHeader)
{
}

BotEngine::~BotEngine()
{
    if (m_running) stop();
    if (m_thread) {
        m_thread->quit();
        m_thread->wait(3000);
    }
}

void BotEngine::start()
{
    if (m_running) return;
    m_running = true;

    m_thread = new QThread(this);
    m_worker = new BotWorker(m_cookieHeader); // no parent — будет перемещён в поток
    m_worker->moveToThread(m_thread);

    connect(m_thread, &QThread::started,       m_worker, &BotWorker::start);
    connect(m_worker, &BotWorker::finished,    this,     &BotEngine::onWorkerFinished);
    connect(m_worker, &BotWorker::logMessage,  this,     &BotEngine::logMessage);
    connect(m_worker, &BotWorker::newMessage,  this,     &BotEngine::newMessage);
    connect(m_worker, &BotWorker::newOrder,    this,     &BotEngine::newOrder);
    connect(m_worker, &BotWorker::statusChanged,this,    &BotEngine::statusChanged);
    connect(m_worker, &BotWorker::initialized,  this,     &BotEngine::initialized);
    connect(m_worker, &BotWorker::messageSent,  this,     &BotEngine::messageSent);

    m_thread->start();
}

void BotEngine::stop()
{
    if (!m_running || !m_worker) return;
    QMetaObject::invokeMethod(m_worker, "stop", Qt::QueuedConnection);
}

void BotEngine::sendChatMessage(const QString& nodeId, const QString& text)
{
    if (!m_worker) return;
    QMetaObject::invokeMethod(m_worker, "sendChatMessage",
                              Qt::QueuedConnection,
                              Q_ARG(QString, nodeId),
                              Q_ARG(QString, text));
}

void BotEngine::onWorkerFinished()
{
    m_running = false;

    if (m_thread) {
        m_thread->quit();
        m_thread->wait(3000);
        m_thread->deleteLater();
        m_thread = nullptr;
    }
    if (m_worker) {
        m_worker->deleteLater();
        m_worker = nullptr;
    }

    emit stopped();
}
