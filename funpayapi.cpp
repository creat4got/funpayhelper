#include "funpayapi.h"
#include "httpclient.h"

#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrl>
#include <QUrlQuery>
#include <QFile>
#include <QDir>

static QString debugPath(const QString& fileName)
{
    return QDir::homePath() + "/" + fileName;
}

FunPayAPI::FunPayAPI(const QString& cookieHeader, QObject* parent)
    : QObject(parent), m_cookieHeader(cookieHeader)
{
    m_runnerTags["orders_counters"] = "";
    m_runnerTags["chat_bookmarks"]  = "";
}

// ─── Init ─────────────────────────────────────────────────────────────────────
bool FunPayAPI::init(QString& error)
{
    return fetchPageInfo(error);
}

bool FunPayAPI::refreshToken(QString& error)
{
    return fetchPageInfo(error);
}

bool FunPayAPI::fetchPageInfo(QString& error)
{
    HttpClient client;
    std::string body, httpErr, setCookieHdr;

    if (!client.get("funpay.com", "/", m_cookieHeader.toStdString(), body, httpErr, &setCookieHdr)) {
        error = QString::fromStdString(httpErr);
        return false;
    }

    // FunPay ротирует PHPSESSID при каждом GET / — обновляем m_cookieHeader новым значением.
    // CSRF в data-app-data привязан к новому PHPSESSID; без обновления POST /runner/ вернёт 400.
    if (!setCookieHdr.empty()) {
        QRegularExpression re("PHPSESSID=([^;\\s\\r\\n]+)",
                              QRegularExpression::CaseInsensitiveOption);
        auto m = re.match(QString::fromStdString(setCookieHdr));
        if (m.hasMatch()) {
            QString newPhpsessid = m.captured(1);
            QRegularExpression replaceRe("PHPSESSID=[^;]+",
                                         QRegularExpression::CaseInsensitiveOption);
            m_cookieHeader.replace(replaceRe, "PHPSESSID=" + newPhpsessid);
        }
    }

    if (body.empty()) {
        error = "Пустой ответ от сервера";
        return false;
    }

    QString html = QString::fromUtf8(body.c_str(), static_cast<int>(body.size()));

    // ── Шаг 1: data-app-data — csrf-token и userId ───────────────────────────
    // Формат: data-app-data="{&quot;csrf-token&quot;:&quot;TOKEN&quot;,&quot;userId&quot;:ID,...}"
    {
        QRegularExpression re("data-app-data=\"([^\"]+)\"");
        auto m = re.match(html);
        if (m.hasMatch()) {
            QString json = m.captured(1);
            json.replace("&quot;", "\"");
            json.replace("&amp;",  "&");
            json.replace("&lt;",   "<");
            json.replace("&gt;",   ">");

            QJsonParseError pe;
            QJsonDocument d = QJsonDocument::fromJson(json.toUtf8(), &pe);
            if (pe.error == QJsonParseError::NoError && d.isObject()) {
                QJsonObject obj = d.object();
                if (obj.contains("csrf-token"))
                    m_userInfo.csrfToken = obj["csrf-token"].toString();
                if (obj.contains("userId")) {
                    qint64 uid = static_cast<qint64>(obj["userId"].toDouble());
                    if (uid == 0) {
                        error = "Cookie недействительны или истекли.\n"
                                "Возможные причины:\n"
                                "• Вы вошли в другой аккаунт в браузере — это аннулирует старую сессию\n"
                                "• Cookie файл устарел — экспортируйте заново\n"
                                "• PHPSESSID / golden_key уже не действуют";
                        return false;
                    }
                    m_userInfo.userId = QString::number(uid);
                }
            }
        }
    }

    // ── Шаг 2: Username из HTML ───────────────────────────────────────────────
    {
        QRegularExpression re("<div class=\"user-link-name\">([^<]+)</div>");
        auto m = re.match(html);
        if (m.hasMatch()) {
            m_userInfo.username = m.captured(1).trimmed();
        }
    }

    // Username fallback: ссылка /users/ID/ → текст
    if (m_userInfo.username.isEmpty()) {
        QRegularExpression re(
            "href=\"https?://funpay\\.com(?:/\\w+)?/users/\\d+/[^\"]*\"[^>]*>\\s*([^\\s<][^<]{0,40}?)\\s*<",
            QRegularExpression::DotMatchesEverythingOption);
        auto m = re.match(html);
        if (m.hasMatch())
            m_userInfo.username = m.captured(1).trimmed();
    }

    // Username финальный fallback: взять из userId если всё провалилось
    if (m_userInfo.username.isEmpty() && !m_userInfo.userId.isEmpty())
        m_userInfo.username = "User#" + m_userInfo.userId;

    // ── Шаг 3: userId fallback через ссылку на профиль ───────────────────────
    if (m_userInfo.userId.isEmpty()) {
        QRegularExpression re(R"(href="/users/(\d+)/)");
        auto m = re.match(html);
        if (m.hasMatch())
            m_userInfo.userId = m.captured(1);
    }

    // ── Шаг 4: CSRF fallback — мета-тег ──────────────────────────────────────
    if (m_userInfo.csrfToken.isEmpty()) {
        QRegularExpression re("<meta name=\"csrf-token\" content=\"([^\"]+)\"");
        auto m = re.match(html);
        if (m.hasMatch())
            m_userInfo.csrfToken = m.captured(1);
    }

    if (m_userInfo.csrfToken.isEmpty()) {
        error = "Не удалось найти CSRF-токен на странице";
        return false;
    }

    if (m_userInfo.username.isEmpty()) {
        error = "Не удалось определить имя пользователя";
        return false;
    }

    m_initialized = true;
    return true;
}

// ─── Runner polling ───────────────────────────────────────────────────────────
QString FunPayAPI::buildRunnerBody() const
{
    QJsonArray objects;
    const QString uid = m_userInfo.userId.isEmpty()
                            ? "1"
                            : m_userInfo.userId;

    for (auto it = m_runnerTags.cbegin(); it != m_runnerTags.cend(); ++it) {
        QJsonObject obj;
        obj["type"] = it.key();
        obj["id"]   = uid;
        obj["tag"]  = it.value();
        obj["data"] = false;
        objects.append(obj);
    }

    QString json = QString::fromUtf8(
        QJsonDocument(objects).toJson(QJsonDocument::Compact));

    QUrlQuery q;
    q.addQueryItem("objects",     json);
    q.addQueryItem("request",     "false");
    q.addQueryItem("csrf_token",  m_userInfo.csrfToken);
    return q.toString(QUrl::FullyEncoded);
}

bool FunPayAPI::poll(QList<RunnerEvent>& events, QString& error)
{
    if (!m_initialized) {
        error = "FunPayAPI не инициализирован";
        return false;
    }

    HttpClient client;
    std::string respBody, httpErr;
    std::string reqBody = buildRunnerBody().toStdString();

    std::vector<std::pair<std::string,std::string>> hdrs = {
        {"X-Requested-With", "XMLHttpRequest"},
        {"Accept", "application/json, text/javascript, */*; q=0.01"},
        {"Referer", "https://funpay.com/en/chat/"},
        {"Origin", "https://funpay.com"}
    };

    if (!client.post("funpay.com", "/runner/",
                     m_cookieHeader.toStdString(),
                     reqBody,
                     "application/x-www-form-urlencoded; charset=UTF-8",
                     respBody, httpErr, hdrs)) {
        error = QString::fromStdString(httpErr);
        return false;
    }


    return parseRunnerResponse(respBody, events, error);
}

bool FunPayAPI::parseRunnerResponse(const std::string& body,
                                    QList<RunnerEvent>& events,
                                    QString& error)
{
    events.clear();
    if (body.empty()) return true;

    QJsonParseError pe;
    QJsonDocument doc = QJsonDocument::fromJson(
        QByteArray(body.c_str(), static_cast<int>(body.size())), &pe);

    if (pe.error != QJsonParseError::NoError) {
        error = "Runner JSON: " + pe.errorString();
        return false;
    }
    if (!doc.isObject()) return true;

    QJsonObject root = doc.object();

    // Обновить CSRF если сервер вернул новый
    if (root.contains("token") && root["token"].isString())
        m_userInfo.csrfToken = root["token"].toString();

    if (!root.contains("objects") || !root["objects"].isArray())
        return true;

    for (const QJsonValue& v : root["objects"].toArray()) {
        if (!v.isObject()) continue;
        QJsonObject obj = v.toObject();

        QString type = obj["type"].toString();
        QString tag  = obj["tag"].toString();

        QString oldTag = m_runnerTags.value(type);
        bool tagChanged = !tag.isEmpty() && (tag != oldTag);

        if (!tag.isEmpty())
            m_runnerTags[type] = tag;

        // При первом poll просто запоминаем теги, не эмитируем события
        if (m_firstPoll)
            continue;

        if (tagChanged && obj.contains("data") && obj["data"].isObject()) {
            RunnerEvent ev;
            ev.type    = type;
            ev.id      = obj["id"].toString();
            ev.tag     = tag;
            ev.data    = obj["data"].toObject();
            ev.hasData = true;
            events.append(ev);
        }
    }

    m_firstPoll = false;
    return true;
}

// ─── Raise section ────────────────────────────────────────────────────────────
// resultMsg: текст из ответа FunPay ("OK", "Please wait X hours." и т.п.)
bool FunPayAPI::raiseSection(const QString& gameId, const QString& nodeId,
                              QString& resultMsg, QString& error)
{
    if (!m_initialized) { error = "API не инициализирован"; return false; }

    QUrlQuery q;
    q.addQueryItem("game_id",    gameId);
    q.addQueryItem("node_id",    nodeId);
    q.addQueryItem("csrf_token", m_userInfo.csrfToken);

    // FunPay требует XHR-заголовок
    std::vector<std::pair<std::string,std::string>> hdrs = {
        {"X-Requested-With", "XMLHttpRequest"}
    };

    HttpClient client;
    std::string respBody, httpErr;

    if (!client.post("funpay.com", "/en/lots/raise",
                     m_cookieHeader.toStdString(),
                     q.toString(QUrl::FullyEncoded).toStdString(),
                     "application/x-www-form-urlencoded",
                     respBody, httpErr, hdrs)) {
        error = QString::fromStdString(httpErr);
        return false;
    }

    // Ответ: {"msg":"...","error":0|1}  или  {"modal":"<html>"}
    QJsonDocument doc = QJsonDocument::fromJson(
        QByteArray(respBody.c_str(), static_cast<int>(respBody.size())));

    if (doc.isObject()) {
        QJsonObject obj = doc.object();

        // FunPay вернул модальное окно — несколько категорий в одной игре
        if (obj.contains("modal")) {
            return raiseFromModal(obj["modal"].toString(), nodeId, resultMsg, error);
        }

        resultMsg = obj["msg"].toString(QString::fromStdString(respBody));
        int errCode = static_cast<int>(obj["error"].toDouble(-1));

        if (errCode == 0) return true;   // success

        // error:1 + "wait" — кулдаун, не фатальная ошибка
        if (errCode == 1 && resultMsg.toLower().contains("wait")) {
            error = resultMsg;   // caller покажет как предупреждение
            return false;
        }

        error = resultMsg;
        return false;
    }

    // Не JSON — нет ошибки сети, считаем успехом
    resultMsg = QString::fromStdString(respBody).left(100);
    return true;
}

// ─── Raise from modal (несколько категорий одной игры) ────────────────────────
// Payload (по данным DevTools):
//   game_id=41&node_id=81&node_ids[]=81&node_ids[]=867&csrf_token=TOKEN
//   node_id   — исходный node, с которого пришёл modal
//   node_ids[] — все node_id из чекбоксов (поднять сразу все категории)
bool FunPayAPI::raiseFromModal(const QString& modalHtml, const QString& originalNodeId,
                                QString& resultMsg, QString& error)
{
    // data-game="41" из <div class="raise-box" data-game="...">
    QRegularExpression gameRe("data-game=\"(\\d+)\"");
    auto gm = gameRe.match(modalHtml);
    if (!gm.hasMatch()) {
        error = "modal: не найден data-game";
        return false;
    }
    QString gameId = gm.captured(1);

    // Все value из <input type="checkbox" value="NODE_ID">
    QRegularExpression nodeRe("<input[^>]+value=\"(\\d+)\"");
    QStringList nodeIds;
    auto it = nodeRe.globalMatch(modalHtml);
    while (it.hasNext())
        nodeIds << it.next().captured(1);

    if (nodeIds.isEmpty()) {
        error = "modal: не найдены node_id в чекбоксах";
        return false;
    }

    // Строим тело точно по DevTools payload:
    //   game_id=41&node_id=81&node_ids[]=81&node_ids[]=867&csrf_token=TOKEN
    // %5B%5D — URL-кодирование [] (PHP распознаёт как массив node_ids)
    QString body = "game_id=" + QString::fromUtf8(QUrl::toPercentEncoding(gameId));
    body += "&node_id=" + QString::fromUtf8(QUrl::toPercentEncoding(originalNodeId));
    for (const QString& nid : nodeIds)
        body += "&node_ids%5B%5D=" + QString::fromUtf8(QUrl::toPercentEncoding(nid));
    body += "&csrf_token=" + QString::fromUtf8(QUrl::toPercentEncoding(m_userInfo.csrfToken));

    std::vector<std::pair<std::string,std::string>> hdrs = {
        {"X-Requested-With", "XMLHttpRequest"}
    };

    HttpClient client;
    std::string respBody, httpErr;

    if (!client.post("funpay.com", "/en/lots/raise",
                     m_cookieHeader.toStdString(),
                     body.toStdString(),
                     "application/x-www-form-urlencoded",
                     respBody, httpErr, hdrs)) {
        error = QString::fromStdString(httpErr);
        return false;
    }

    QJsonDocument doc2 = QJsonDocument::fromJson(
        QByteArray(respBody.c_str(), static_cast<int>(respBody.size())));

    if (doc2.isObject()) {
        QJsonObject obj = doc2.object();

        // Защита от повторного modal (неожиданный ответ сервера)
        if (obj.contains("modal")) {
            error = "modal: сервер вернул повторный modal после multi-raise";
            return false;
        }

        resultMsg = obj["msg"].toString(QString::fromStdString(respBody));
        int errCode = static_cast<int>(obj["error"].toDouble(-1));

        if (errCode == 0) return true;

        if (errCode == 1 && resultMsg.toLower().contains("wait")) {
            error = resultMsg;
            return false;
        }

        error = resultMsg;
        return false;
    }

    resultMsg = QString::fromStdString(respBody).left(100);
    return true;
}

// ─── Fetch user offers (профиль) ─────────────────────────────────────────────
// Парсит страницу профиля → список секций с лотами.
// game_id НЕ заполняется здесь — вызови fetchSectionGameIds отдельно.
bool FunPayAPI::fetchUserOffers(QList<SectionInfo>& sections, QString& error)
{
    sections.clear();
    if (!m_initialized) { error = "API не инициализирован"; return false; }

    HttpClient client;
    std::string body, httpErr;
    QString target = QString("/users/%1/").arg(m_userInfo.userId);

    if (!client.get("funpay.com", target.toStdString(),
                    m_cookieHeader.toStdString(), body, httpErr)) {
        error = QString::fromStdString(httpErr);
        return false;
    }

    QString html = QString::fromUtf8(body.c_str(), static_cast<int>(body.size()));

    // Делим страницу на блоки <div class="offer">
    // Каждый блок — одна секция (Adobe Accounts, Dota 2 Accounts…)
    QRegularExpression blockRe(
        "<div class=\"offer\">(.*?)(?=<div class=\"offer\">|$)",
        QRegularExpression::DotMatchesEverythingOption);

    // Название секции и node_id из заголовка блока
    QRegularExpression sectionRe(
        "href=\"https://funpay\\.com(?:/\\w+)?/lots/(\\d+)/\">([^<]+)</a>");

    // Отдельный лот: href="/lots/offer?id=ID"
    QRegularExpression itemRe(
        "href=\"[^\"]+/lots/offer\\?id=(\\d+)\"[^>]*class=\"tc-item\"[^>]*>(.*?)</a>",
        QRegularExpression::DotMatchesEverythingOption);
    // Порядок атрибутов: иногда class идёт раньше href
    QRegularExpression itemRe2(
        "class=\"tc-item\"[^>]+href=\"[^\"]+/lots/offer\\?id=(\\d+)\"[^>]*>(.*?)</a>",
        QRegularExpression::DotMatchesEverythingOption);

    // Описание лота
    QRegularExpression descRe(
        "<div\\s+class=\"tc-desc-text\">([^<]+)</div>",
        QRegularExpression::DotMatchesEverythingOption);

    // Цена: data-s="142.790224"
    QRegularExpression priceRe("data-s=\"([0-9.]+)\"");

    QRegularExpressionMatchIterator blockIt = blockRe.globalMatch(html);
    while (blockIt.hasNext()) {
        QString block = blockIt.next().captured(1);

        // Заголовок секции
        auto sm = sectionRe.match(block);
        if (!sm.hasMatch()) continue;

        SectionInfo sec;
        sec.nodeId = sm.captured(1);
        sec.name   = sm.captured(2).trimmed();

        // Лоты внутри блока — пробуем оба порядка атрибутов
        auto tryParse = [&](const QRegularExpression& re) {
            QRegularExpressionMatchIterator it = re.globalMatch(block);
            while (it.hasNext()) {
                auto m = it.next();
                LotInfo lot;
                lot.id       = m.captured(1);
                lot.nodeId   = sec.nodeId;
                lot.sectionName = sec.name;
                lot.active   = true;

                QString inner = m.captured(2);

                auto dm = descRe.match(inner);
                if (dm.hasMatch())
                    lot.description = dm.captured(1).trimmed();

                auto pm = priceRe.match(inner);
                if (pm.hasMatch())
                    lot.price = pm.captured(1).toDouble();

                sec.lots.append(lot);
            }
        };

        tryParse(itemRe);
        if (sec.lots.isEmpty()) tryParse(itemRe2);

        sections.append(sec);
    }

    if (sections.isEmpty()) {
        error = "Активные лоты не найдены на странице профиля";
        return false;
    }

    return true;
}

// ─── Fetch game_id for each section ──────────────────────────────────────────
// Для каждой секции делает GET /lots/NODE_ID/trade и берёт game_id
// из атрибутов кнопки "Boost offers".
bool FunPayAPI::fetchSectionGameIds(QList<SectionInfo>& sections, QString& error)
{
    if (!m_initialized) { error = "API не инициализирован"; return false; }

    QRegularExpression raiseRe(
        "js-lot-raise[^>]+data-game=\"(\\d+)\"[^>]+data-node=\"(\\d+)\"");
    // Иногда атрибуты в обратном порядке
    QRegularExpression raiseRe2(
        "js-lot-raise[^>]+data-node=\"(\\d+)\"[^>]+data-game=\"(\\d+)\"");

    bool anyError = false;

    for (SectionInfo& sec : sections) {
        HttpClient client;
        std::string body, httpErr;
        QString target = QString("/lots/%1/trade").arg(sec.nodeId);

        if (!client.get("funpay.com", target.toStdString(),
                        m_cookieHeader.toStdString(), body, httpErr)) {
            error = QString("Ошибка загрузки trade-страницы node=%1: %2")
                    .arg(sec.nodeId, QString::fromStdString(httpErr));
            anyError = true;
            continue;
        }

        QString html = QString::fromUtf8(body.c_str(), static_cast<int>(body.size()));

        auto m1 = raiseRe.match(html);
        if (m1.hasMatch()) {
            sec.gameId = m1.captured(1);
            continue;
        }

        auto m2 = raiseRe2.match(html);
        if (m2.hasMatch()) {
            sec.gameId = m2.captured(2); // в обратном порядке: node=1, game=2
            continue;
        }

        error = QString("Кнопка поднятия не найдена для node=%1 (возможно, лоты деактивированы)").arg(sec.nodeId);
        anyError = true;
    }

    return !anyError;
}

// ─── Fetch chat contacts (список чатов) ───────────────────────────────────────
// GET /en/chat/ → парсит .contact-item блоки
bool FunPayAPI::fetchChatContacts(QList<ChatContact>& contacts, QString& error)
{
    contacts.clear();
    if (!m_initialized) { error = "API не инициализирован"; return false; }

    HttpClient client;
    std::string body, httpErr;
    if (!client.get("funpay.com", "/en/chat/",
                    m_cookieHeader.toStdString(), body, httpErr)) {
        error = QString::fromStdString(httpErr);
        return false;
    }

    QString html = QString::fromUtf8(body.c_str(), static_cast<int>(body.size()));

    // Debug dump
    {
        QFile f("funpay_chat.html");
        if (f.open(QIODevice::WriteOnly))
            f.write(body.c_str(), static_cast<qint64>(body.size()));
    }

    // Группа 1 = весь открывающий тег <a ...>, группа 2 = содержимое
    // Так data-id и data-node-msg (которые стоят ПОСЛЕ class=) попадают в group 1
    QRegularExpression fullBlockRe(
        "(<a\\s[^>]*class=\"contact-item[^\"]*\"[^>]*>)(.*?)</a>",
        QRegularExpression::DotMatchesEverythingOption);

    QRegularExpression idRe("data-id=\"(\\d+)\"");
    QRegularExpression msgIdRe("data-node-msg=\"(\\d+)\"");
    QRegularExpression nameRe("<div class=\"media-user-name\">([^<]+)</div>");
    QRegularExpression lastMsgRe(
        "<div class=\"contact-item-message\">(.*?)</div>",
        QRegularExpression::DotMatchesEverythingOption);
    QRegularExpression timeRe("<div class=\"contact-item-time\">([^<]+)</div>");

    auto it = fullBlockRe.globalMatch(html);
    while (it.hasNext()) {
        auto m = it.next();
        QString openTag = m.captured(1);  // <a href="..." class="..." data-id="..." ...>
        QString inner   = m.captured(2);  // содержимое между тегами

        ChatContact c;

        auto im = idRe.match(openTag);
        if (!im.hasMatch()) continue;
        c.nodeId = im.captured(1);

        auto mm = msgIdRe.match(openTag);
        if (mm.hasMatch())
            c.lastMsgId = mm.captured(1).toLongLong();

        auto nm = nameRe.match(inner);
        if (nm.hasMatch())
            c.username = nm.captured(1).trimmed();

        auto lm = lastMsgRe.match(inner);
        if (lm.hasMatch())
            c.lastMessage = lm.captured(1)
                              .remove(QRegularExpression("<[^>]+>"))
                              .simplified();

        auto tm = timeRe.match(inner);
        if (tm.hasMatch())
            c.time = tm.captured(1).trimmed();

        if (!c.nodeId.isEmpty())
            contacts.append(c);
    }

    if (contacts.isEmpty()) {
        error = "Чаты не найдены";
        return false;
    }
    return true;
}

// ─── Fetch chat history (история одного чата) ─────────────────────────────────
// GET /en/chat/?node=NODE_ID → парсит .chat-msg-item блоки + data-name чата
bool FunPayAPI::fetchChatHistory(const QString& nodeId,
                                  QList<ChatHistoryMsg>& messages,
                                  ChatNodeInfo& info, QString& error)
{
    messages.clear();
    info = {};
    if (!m_initialized) { error = "API не инициализирован"; return false; }
    HttpClient client;
    std::string body, httpErr;
    QString target = QString("/en/chat/?node=%1").arg(nodeId);

    if (!client.get("funpay.com", target.toStdString(),
                    m_cookieHeader.toStdString(), body, httpErr)) {
        error = QString::fromStdString(httpErr);
        return false;
    }

    QString html = QString::fromUtf8(body.c_str(), static_cast<int>(body.size()));

    // 1) .chat.chat-float — data-name и data-tag
    {
        QRegularExpression re(
            "class=\"chat[^\"]*chat-float[^\"]*\"([^>]*)>",
            QRegularExpression::DotMatchesEverythingOption);
        auto m = re.match(html);
        if (m.hasMatch()) {
            QString attrs = m.captured(1);
            QRegularExpression nameRe("data-name=\"([^\"]+)\"");
            QRegularExpression tagRe("data-tag=\"([^\"]+)\"");
            auto nm = nameRe.match(attrs); if (nm.hasMatch()) info.nodeName = nm.captured(1);
            auto tm = tagRe.match(attrs);  if (tm.hasMatch()) info.chatTag  = tm.captured(1);
        }
    }
    // 2) c-p-u panel — data-id (ID собеседника) и data-tag
    // <div class="param-item chat-panel hidden" data-type="c-p-u" data-id="19320521" data-tag="xyhm8muo">
    {
        QRegularExpression re(
            "data-type=\"c-p-u\"[^>]+data-id=\"(\\d+)\"[^>]+data-tag=\"([^\"]+)\"");
        auto m = re.match(html);
        if (m.hasMatch()) {
            info.cpuId  = m.captured(1);
            info.cpuTag = m.captured(2);
        } else {
            // fallback: другой порядок атрибутов
            QRegularExpression re2(
                "data-type=\"c-p-u\"[^>]+data-tag=\"([^\"]+)\"[^>]+data-id=\"(\\d+)\"");
            auto m2 = re2.match(html);
            if (m2.hasMatch()) { info.cpuTag = m2.captured(1); info.cpuId = m2.captured(2); }
        }
    }

    // Каждое сообщение — <div class="chat-msg-item ..." id="message-XXXX">
    QRegularExpression blockRe(
        "<div class=\"chat-msg-item[^\"]*\" id=\"message-(\\d+)\">(.*?)"
        "(?=<div class=\"chat-msg-item|$)",
        QRegularExpression::DotMatchesEverythingOption);

    // Автор — ссылка или текст "FunPay"
    QRegularExpression authorRe(
        "<a[^>]+class=\"chat-msg-author-link\"[^>]*>([^<]+)</a>");
    // Время — из текста тега ("07:56:24"), а не из title (там дата + время)
    QRegularExpression dateRe(
        "<div class=\"chat-msg-date\"[^>]*>([^<]+)</div>");
    // Текст сообщения
    QRegularExpression textRe(
        "<div class=\"chat-msg-text\">(.*?)</div>",
        QRegularExpression::DotMatchesEverythingOption);

    auto it = blockRe.globalMatch(html);
    while (it.hasNext()) {
        auto m = it.next();
        ChatHistoryMsg msg;
        msg.id = m.captured(1).toLongLong();
        QString inner = m.captured(2);

        auto am = authorRe.match(inner);
        if (am.hasMatch()) {
            msg.author      = am.captured(1).trimmed();
            msg.isMyMessage = (msg.author == m_userInfo.username);
        } else {
            msg.author   = "FunPay";
            msg.isSystem = true;
        }

        auto dm = dateRe.match(inner);
        if (dm.hasMatch())
            msg.time = dm.captured(1);  // "22 March, 7:56:24"

        auto tm = textRe.match(inner);
        if (tm.hasMatch())
            msg.text = tm.captured(1)
                         .remove(QRegularExpression("<[^>]+>"))
                         .simplified();

        if (!msg.text.isEmpty())
            messages.append(msg);
    }
    info.ordersCountersTag = m_runnerTags.value("orders_counters", "");
    info.bookmarksTag      = m_runnerTags.value("chat_bookmarks", "");

    return true;
}

// ─── Send message via runner (/runner/) ───────────────────────────────────────
// Точно по Cardinal (account.py::send_message + get_payload_data):
//   objects = [один chat_node с tag="00000000", last_message=-1, content=""]
//   request = {"action":"chat_message","data":{"node":ID,"last_message":-1,"content":TEXT}}
//   endpoint = /runner/ (без /en/)
bool FunPayAPI::sendMessageNode(const ChatNodeInfo& info,
                                qint64 lastMsgId,
                                const QString& numericNodeId,
                                const QString& text,
                                const QList<ChatContact>& contacts,
                                QString& error)
{
    Q_UNUSED(lastMsgId)
    Q_UNUSED(contacts)

    if (!m_initialized) {
        error = "API не инициализирован";
        return false;
    }

    // Обновляем CSRF прямо перед отправкой — FunPay инвалидирует старый CSRF при любой загрузке страницы.
    {
        QString refreshErr;
        fetchPageInfo(refreshErr);
    }

    const qint64 nodeIdInt = numericNodeId.toLongLong();

    // ── request: action + текст сообщения ────────────────────────────────────
    QJsonObject reqData;
    reqData["node"]         = nodeIdInt;
    reqData["last_message"] = static_cast<qint64>(-1);
    reqData["content"]      = text;

    QJsonObject request;
    request["action"] = "chat_message";
    request["data"]   = reqData;

    // ── form body ─────────────────────────────────────────────────────────────
    QUrlQuery q;
    q.addQueryItem("objects", "[]");
    q.addQueryItem("request",
        QString::fromUtf8(QJsonDocument(request).toJson(QJsonDocument::Compact)));
    q.addQueryItem("csrf_token", m_userInfo.csrfToken);

    std::vector<std::pair<std::string, std::string>> hdrs = {
        {"X-Requested-With", "XMLHttpRequest"},
        {"Accept",   "application/json, text/javascript, */*; q=0.01"},
        {"Referer",  ("https://funpay.com/en/chat/?node=" + numericNodeId).toStdString()},
        {"Origin",   "https://funpay.com"}
    };

    HttpClient client;
    std::string respBody, httpErr;

    bool postOk = client.post("funpay.com", "/runner/",
                     m_cookieHeader.toStdString(),
                     q.toString(QUrl::FullyEncoded).toStdString(),
                     "application/x-www-form-urlencoded; charset=UTF-8",
                     respBody, httpErr, hdrs);

    if (!postOk) {
        error = QString::fromStdString(httpErr);
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(
        QByteArray(respBody.c_str(), static_cast<int>(respBody.size())));

    if (doc.isObject()) {
        QJsonObject obj = doc.object();

        if (obj.contains("token") && obj["token"].isString())
            m_userInfo.csrfToken = obj["token"].toString();

        if (obj.contains("error") && obj["error"].isDouble() &&
            static_cast<int>(obj["error"].toDouble()) != 0) {
            error = obj.value("msg").toString(QString::fromStdString(respBody));
            return false;
        }
    }

    error.clear();
    return true;
}
