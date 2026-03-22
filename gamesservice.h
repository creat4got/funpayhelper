#ifndef GAMESSERVICE_H
#define GAMESSERVICE_H

#include "gamemodels.h"
#include <QString>
#include <QList>


class GamesService
{
public:
    GamesService();

    bool loadGames(const QString& cookieHeader, QList<GameInfo>& games, QString& errorText);
};

#endif // GAMESSERVICE_H