#ifndef ADDLOTDIALOG_H
#define ADDLOTDIALOG_H

#include <QDialog>
#include "gamemodels.h"
namespace Ui {
    class AddLotDialog;
}

class AddLotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddLotDialog(const QString& cookieHeader, QWidget *parent = 0);
    ~AddLotDialog();

private:
    void loadGames();
    void fillGamesList();

private:
    Ui::AddLotDialog *ui;
    QString m_cookieHeader;
    QList<GameInfo> m_games;
};

#endif // ADDLOTDIALOG_H