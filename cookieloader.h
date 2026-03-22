#ifndef COOKIELOADER_H
#define COOKIELOADER_H

#include <QString>

class CookieLoader
{
public:
    CookieLoader();

    bool loadFromFile(const QString& filePath);
    QString cookieHeader() const;
    QString lastError() const;

private:
    QString m_cookieHeader;
    QString m_lastError;
};

#endif // COOKIELOADER_H