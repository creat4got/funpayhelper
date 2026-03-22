#ifndef GAMESPARSER_H
#define GAMESPARSER_H

#include "gamemodels.h"
#include <QString>
#include <QList>

class GamesParser
{
public:
    GamesParser();

    QList<GameInfo> parseGames(const QString& html);
};

#endif // GAMESPARSER_H