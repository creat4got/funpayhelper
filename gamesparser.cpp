#include "gamesparser.h"

#include <QRegularExpression>

GamesParser::GamesParser()
{
}

QList<GameInfo> GamesParser::parseGames(const QString& html)
{
    QList<GameInfo> games;

    QRegularExpression itemRegex(
        "<div class=\"promo-game-item\">(.*?)</div>\\s*</div>",
        QRegularExpression::DotMatchesEverythingOption
    );

    QRegularExpression visibleTitleRegex(
        "<div class=\"game-title\"[^>]*>\\s*<a href=\"([^\"]+)\">([^<]+)</a>\\s*</div>",
        QRegularExpression::DotMatchesEverythingOption
    );

    QRegularExpression visibleListRegex(
        "<ul class=\"list-inline\"[^>]*>(.*?)</ul>",
        QRegularExpression::DotMatchesEverythingOption
    );

    QRegularExpression categoryRegex(
        "<li>\\s*<a href=\"([^\"]+)\">([^<]+)</a>\\s*</li>",
        QRegularExpression::DotMatchesEverythingOption
    );

    QRegularExpressionMatchIterator itemIt = itemRegex.globalMatch(html);

    while (itemIt.hasNext()) {
        QRegularExpressionMatch itemMatch = itemIt.next();
        QString block = itemMatch.captured(1);

        QRegularExpressionMatch titleMatch = visibleTitleRegex.match(block);
        if (!titleMatch.hasMatch()) {
            continue;
        }

        GameInfo game;
        game.url = titleMatch.captured(1).trimmed();
        game.name = titleMatch.captured(2).trimmed();

        if (game.url.startsWith("/")) {
            game.url = "https://funpay.com" + game.url;
        }

        QRegularExpressionMatch listMatch = visibleListRegex.match(block);
        if (listMatch.hasMatch()) {
            QString categoriesHtml = listMatch.captured(1);

            QRegularExpressionMatchIterator catIt = categoryRegex.globalMatch(categoriesHtml);
            while (catIt.hasNext()) {
                QRegularExpressionMatch catMatch = catIt.next();

                GameCategory category;
                category.url = catMatch.captured(1).trimmed();
                category.name = catMatch.captured(2).trimmed();

                if (category.url.startsWith("/")) {
                    category.url = "https://funpay.com" + category.url;
                }

                game.categories.append(category);
            }
        }

        if (!game.name.isEmpty()) {
            games.append(game);
        }
    }

    return games;
}