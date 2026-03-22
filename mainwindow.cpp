#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "cookieloader.h"
#include "funpayapi.h"

#include <QFileDialog>
#include <QApplication>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("BotFunPay — Вход");

    connect(ui->browseCookieButton, &QPushButton::clicked,
            this, &MainWindow::onBrowseCookieClicked);
    connect(ui->loginButton, &QPushButton::clicked,
            this, &MainWindow::onLoginClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onBrowseCookieClicked()
{
    QString path = QFileDialog::getOpenFileName(
        this, "Выберите cookie-файл", "",
        "JSON Files (*.json);;All Files (*)");

    if (!path.isEmpty()) {
        ui->cookiePathEdit->setText(path);
        ui->statusLabel->setText("Файл выбран");
        ui->statusLabel->setStyleSheet("color: #27ae60;");
    }
}

void MainWindow::onLoginClicked()
{
    QString filePath = ui->cookiePathEdit->text().trimmed();
    if (filePath.isEmpty()) {
        ui->statusLabel->setText("Ошибка: файл cookie не выбран");
        ui->statusLabel->setStyleSheet("color: #c0392b;");
        return;
    }

    CookieLoader loader;
    if (!loader.loadFromFile(filePath)) {
        ui->statusLabel->setText("Ошибка cookie: " + loader.lastError());
        ui->statusLabel->setStyleSheet("color: #c0392b;");
        return;
    }

    ui->statusLabel->setText("Подключение к FunPay…");
    ui->statusLabel->setStyleSheet("color: #e67e22;");
    ui->loginButton->setEnabled(false);
    QApplication::processEvents();

    // FunPayAPI получает username, userId и csrfToken за один запрос
    FunPayAPI api(loader.cookieHeader());
    QString err;
    if (!api.init(err)) {
        ui->statusLabel->setText("Ошибка: " + err);
        ui->statusLabel->setStyleSheet("color: #c0392b;");
        ui->loginButton->setEnabled(true);
        return;
    }

    ui->loginButton->setEnabled(true);
    ui->statusLabel->setText(
        "Добро пожаловать, " + api.userInfo().username + "!");
    ui->statusLabel->setStyleSheet("color: #27ae60;");

    if (m_dashboard) {
        m_dashboard->close();
        m_dashboard->deleteLater();
        m_dashboard = nullptr;
    }

    m_dashboard = new DashboardWindow(api.userInfo(), loader.cookieHeader());
    m_dashboard->show();
    this->hide();
}
