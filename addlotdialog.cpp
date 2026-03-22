#include "addlotdialog.h"
#include "gamesservice.h"
#include "ui_addlotdialog.h"

#include <QMessageBox>

AddLotDialog::AddLotDialog(const QString& cookieHeader, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddLotDialog)
    , m_cookieHeader(cookieHeader)
{
    ui->setupUi(this);
    loadGames();
}

AddLotDialog::~AddLotDialog()
{
    delete ui;
}

void AddLotDialog::loadGames()
{
    GamesService service;
    QString errorText;

    if (!service.loadGames(m_cookieHeader, m_games, errorText)) {
        QMessageBox::warning(this, "Ошибка", errorText);
        return;
    }

    fillGamesList();
}

void AddLotDialog::fillGamesList()
{
    ui->gamesListWidget->clear();

    for (int i = 0; i < m_games.size(); ++i) {
        ui->gamesListWidget->addItem(m_games[i].name);
    }
}