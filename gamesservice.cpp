#include "gamesservice.h"
#include "httpclient.h"
#include "gamesparser.h"
#include <QFile>

GamesService::GamesService()
{
}

bool GamesService::loadGames(const QString& cookieHeader, QList<GameInfo>& games, QString& errorText)
{
    games.clear();

    HttpClient client;

    std::string responseBody;
    std::string httpError;

    bool ok = client.get("funpay.com",
                         "/",
                         cookieHeader.toStdString(),
                         responseBody,
                         httpError);

    if (!ok) {
        errorText = QString::fromStdString(httpError);
        return false;
    }

    if (responseBody.empty()) {
        errorText = "Пустой ответ от сервера";
        return false;
    }

    QFile file("games_page.html");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(responseBody.c_str(), static_cast<qint64>(responseBody.size()));
        file.close();
    }

    QString html = QString::fromUtf8(responseBody.c_str(),
                                     static_cast<int>(responseBody.size()));

    GamesParser parser;
    games = parser.parseGames(html);

    if (games.isEmpty()) {
        errorText = "Не удалось распарсить список игр";
        return false;
    }

    errorText.clear();
    return true;
}