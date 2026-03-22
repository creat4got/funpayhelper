#include "cookieloader.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QSet>

CookieLoader::CookieLoader()
{
}

bool CookieLoader::loadFromFile(const QString& filePath)
{
    m_cookieHeader.clear();
    m_lastError.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        m_lastError = "Не удалось открыть cookie-файл";
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        m_lastError = "Ошибка JSON: " + parseError.errorString();
        return false;
    }

    // Поддерживаем два формата:
    //   1) Массив напрямую:  [{name,value,...}, ...]   (экспорт EditThisCookie / Cookie-Editor)
    //   2) Объект с ключом:  {"cookies": [{...}, ...]} (старый формат)
    QJsonArray cookies;

    if (doc.isArray()) {
        cookies = doc.array();
    } else if (doc.isObject()) {
        QJsonObject root = doc.object();
        if (root.contains("cookies") && root["cookies"].isArray()) {
            cookies = root["cookies"].toArray();
        } else {
            m_lastError = "Неизвестный формат JSON: ожидается массив или {\"cookies\":[...]}";
            return false;
        }
    } else {
        m_lastError = "Некорректный JSON";
        return false;
    }

    QStringList parts;
    QSet<QString> seen; // дубликаты не добавляем

    for (const QJsonValue& v : cookies) {
        if (!v.isObject()) continue;

        QJsonObject obj = v.toObject();
        QString name  = obj["name"].toString();
        QString value = obj["value"].toString();

        if (name.isEmpty() || seen.contains(name)) continue;
        seen.insert(name);

        parts << (name + "=" + value);
    }

    if (parts.isEmpty()) {
        m_lastError = "Cookie не найдены в файле";
        return false;
    }

    m_cookieHeader = parts.join("; ");
    return true;
}

QString CookieLoader::cookieHeader() const
{
    return m_cookieHeader;
}

QString CookieLoader::lastError() const
{
    return m_lastError;
}
