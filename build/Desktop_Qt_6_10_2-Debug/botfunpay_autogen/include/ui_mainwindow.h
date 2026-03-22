/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *titleLabel;
    QLineEdit *cookiePathEdit;
    QPushButton *loginButton;
    QLabel *statusLabel;
    QPushButton *browseCookieButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(864, 494);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        titleLabel = new QLabel(centralwidget);
        titleLabel->setObjectName("titleLabel");
        titleLabel->setGeometry(QRect(15, 190, 151, 31));
        titleLabel->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        titleLabel->setTextFormat(Qt::TextFormat::AutoText);
        titleLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        cookiePathEdit = new QLineEdit(centralwidget);
        cookiePathEdit->setObjectName("cookiePathEdit");
        cookiePathEdit->setGeometry(QRect(170, 190, 501, 31));
        cookiePathEdit->setClearButtonEnabled(true);
        loginButton = new QPushButton(centralwidget);
        loginButton->setObjectName("loginButton");
        loginButton->setGeometry(QRect(350, 280, 94, 26));
        statusLabel = new QLabel(centralwidget);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setGeometry(QRect(150, 230, 521, 31));
        browseCookieButton = new QPushButton(centralwidget);
        browseCookieButton->setObjectName("browseCookieButton");
        browseCookieButton->setGeometry(QRect(170, 150, 94, 26));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 864, 23));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        titleLabel->setText(QCoreApplication::translate("MainWindow", "\320\277\321\203\321\202\321\214 \320\272 cookie-\321\204\320\260\320\271\320\273\321\203", nullptr));
        cookiePathEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "push cookie", nullptr));
        loginButton->setText(QCoreApplication::translate("MainWindow", "Done", nullptr));
        statusLabel->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        browseCookieButton->setText(QCoreApplication::translate("MainWindow", "Chose", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
