/********************************************************************************
** Form generated from reading UI file 'addlotdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDLOTDIALOG_H
#define UI_ADDLOTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AddLotDialog
{
public:
    QLabel *lotTitleLabel;
    QLineEdit *gameSearchEdit;
    QComboBox *comboBox;
    QListWidget *gamesListWidget;
    QLineEdit *lotTitleEdit;
    QTextEdit *lotDescriptionEdit;
    QLineEdit *lotPriceEdit;
    QPushButton *saveLotButton;
    QPushButton *cancelButton;

    void setupUi(QWidget *AddLotDialog)
    {
        if (AddLotDialog->objectName().isEmpty())
            AddLotDialog->setObjectName("AddLotDialog");
        AddLotDialog->resize(860, 516);
        lotTitleLabel = new QLabel(AddLotDialog);
        lotTitleLabel->setObjectName("lotTitleLabel");
        lotTitleLabel->setGeometry(QRect(40, 100, 101, 21));
        gameSearchEdit = new QLineEdit(AddLotDialog);
        gameSearchEdit->setObjectName("gameSearchEdit");
        gameSearchEdit->setGeometry(QRect(150, 100, 113, 26));
        comboBox = new QComboBox(AddLotDialog);
        comboBox->setObjectName("comboBox");
        comboBox->setGeometry(QRect(290, 100, 86, 26));
        gamesListWidget = new QListWidget(AddLotDialog);
        gamesListWidget->setObjectName("gamesListWidget");
        gamesListWidget->setGeometry(QRect(530, 50, 256, 192));
        lotTitleEdit = new QLineEdit(AddLotDialog);
        lotTitleEdit->setObjectName("lotTitleEdit");
        lotTitleEdit->setGeometry(QRect(350, 290, 113, 26));
        lotDescriptionEdit = new QTextEdit(AddLotDialog);
        lotDescriptionEdit->setObjectName("lotDescriptionEdit");
        lotDescriptionEdit->setGeometry(QRect(480, 280, 104, 70));
        lotPriceEdit = new QLineEdit(AddLotDialog);
        lotPriceEdit->setObjectName("lotPriceEdit");
        lotPriceEdit->setGeometry(QRect(480, 360, 113, 26));
        saveLotButton = new QPushButton(AddLotDialog);
        saveLotButton->setObjectName("saveLotButton");
        saveLotButton->setGeometry(QRect(80, 370, 94, 26));
        cancelButton = new QPushButton(AddLotDialog);
        cancelButton->setObjectName("cancelButton");
        cancelButton->setGeometry(QRect(200, 370, 94, 26));

        retranslateUi(AddLotDialog);

        QMetaObject::connectSlotsByName(AddLotDialog);
    } // setupUi

    void retranslateUi(QWidget *AddLotDialog)
    {
        AddLotDialog->setWindowTitle(QCoreApplication::translate("AddLotDialog", "Form", nullptr));
        lotTitleLabel->setText(QCoreApplication::translate("AddLotDialog", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214 \320\273\320\276\321\202", nullptr));
        gameSearchEdit->setInputMask(QCoreApplication::translate("AddLotDialog", "\320\277\320\276\320\270\321\201\320\272 \320\270\320\263\321\200\321\213", nullptr));
        gameSearchEdit->setText(QCoreApplication::translate("AddLotDialog", "\320\277\320\276\320\270\321\201\320\272 \320\270\320\263\321\200\321\213", nullptr));
        lotTitleEdit->setText(QCoreApplication::translate("AddLotDialog", "\320\235\320\260\320\267\320\262\320\260\320\275\320\270\320\265 \320\273\320\276\321\202\320\260", nullptr));
        saveLotButton->setText(QCoreApplication::translate("AddLotDialog", "PushButton", nullptr));
        cancelButton->setText(QCoreApplication::translate("AddLotDialog", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AddLotDialog: public Ui_AddLotDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDLOTDIALOG_H
