#ifndef GAMEMODELS_H
#define GAMEMODELS_H

#include <QString>
#include <QList>

struct GameCategory
{
    QString name;
    QString url;
};

struct GameInfo
{
    QString name;
    QString url;
    QList<GameCategory> categories;
};

#endif // GAMEMODELS_H