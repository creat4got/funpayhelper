/********************************************************************************
** Form generated from reading UI file 'dashboardwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DASHBOARDWINDOW_H
#define UI_DASHBOARDWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DashboardWindow
{
public:
    QPushButton *addLotButton;
    QPushButton *raiseLotsButton;
    QPushButton *allLotsButton;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *welcomeLabel;

    void setupUi(QWidget *DashboardWindow)
    {
        if (DashboardWindow->objectName().isEmpty())
            DashboardWindow->setObjectName("DashboardWindow");
        DashboardWindow->resize(813, 492);
        addLotButton = new QPushButton(DashboardWindow);
        addLotButton->setObjectName("addLotButton");
        addLotButton->setGeometry(QRect(170, 180, 94, 26));
        raiseLotsButton = new QPushButton(DashboardWindow);
        raiseLotsButton->setObjectName("raiseLotsButton");
        raiseLotsButton->setGeometry(QRect(170, 230, 94, 26));
        allLotsButton = new QPushButton(DashboardWindow);
        allLotsButton->setObjectName("allLotsButton");
        allLotsButton->setGeometry(QRect(170, 280, 94, 26));
        verticalLayoutWidget = new QWidget(DashboardWindow);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(590, 10, 211, 80));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        welcomeLabel = new QLabel(verticalLayoutWidget);
        welcomeLabel->setObjectName("welcomeLabel");
        welcomeLabel->setAlignment(Qt::AlignmentFlag::AlignHCenter|Qt::AlignmentFlag::AlignTop);

        verticalLayout->addWidget(welcomeLabel);


        retranslateUi(DashboardWindow);

        QMetaObject::connectSlotsByName(DashboardWindow);
    } // setupUi

    void retranslateUi(QWidget *DashboardWindow)
    {
        DashboardWindow->setWindowTitle(QCoreApplication::translate("DashboardWindow", "Form", nullptr));
        addLotButton->setText(QCoreApplication::translate("DashboardWindow", "PushButton", nullptr));
        raiseLotsButton->setText(QCoreApplication::translate("DashboardWindow", "PushButton", nullptr));
        allLotsButton->setText(QCoreApplication::translate("DashboardWindow", "PushButton", nullptr));
        welcomeLabel->setText(QCoreApplication::translate("DashboardWindow", "Welcome: username", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DashboardWindow: public Ui_DashboardWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DASHBOARDWINDOW_H
