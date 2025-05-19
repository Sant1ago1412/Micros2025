/********************************************************************************
** Form generated from reading UI file 'microscom.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MICROSCOM_H
#define UI_MICROSCOM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MicrosCom
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QPlainTextEdit *plainTextEdit;
    QComboBox *Comand;
    QComboBox *SerialPort;
    QPushButton *Connect;
    QPushButton *SendButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MicrosCom)
    {
        if (MicrosCom->objectName().isEmpty())
            MicrosCom->setObjectName("MicrosCom");
        MicrosCom->resize(488, 424);
        centralwidget = new QWidget(MicrosCom);
        centralwidget->setObjectName("centralwidget");
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName("gridLayout");
        plainTextEdit = new QPlainTextEdit(centralwidget);
        plainTextEdit->setObjectName("plainTextEdit");

        gridLayout->addWidget(plainTextEdit, 4, 0, 1, 2);

        Comand = new QComboBox(centralwidget);
        Comand->setObjectName("Comand");

        gridLayout->addWidget(Comand, 1, 1, 1, 1);

        SerialPort = new QComboBox(centralwidget);
        SerialPort->setObjectName("SerialPort");

        gridLayout->addWidget(SerialPort, 1, 0, 1, 1);

        Connect = new QPushButton(centralwidget);
        Connect->setObjectName("Connect");

        gridLayout->addWidget(Connect, 2, 0, 1, 1);

        SendButton = new QPushButton(centralwidget);
        SendButton->setObjectName("SendButton");

        gridLayout->addWidget(SendButton, 2, 1, 1, 1);

        MicrosCom->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MicrosCom);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 488, 21));
        MicrosCom->setMenuBar(menubar);
        statusbar = new QStatusBar(MicrosCom);
        statusbar->setObjectName("statusbar");
        MicrosCom->setStatusBar(statusbar);

        retranslateUi(MicrosCom);

        QMetaObject::connectSlotsByName(MicrosCom);
    } // setupUi

    void retranslateUi(QMainWindow *MicrosCom)
    {
        MicrosCom->setWindowTitle(QCoreApplication::translate("MicrosCom", "MicrosCom", nullptr));
        Connect->setText(QCoreApplication::translate("MicrosCom", "Connect", nullptr));
        SendButton->setText(QCoreApplication::translate("MicrosCom", "SendCMD", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MicrosCom: public Ui_MicrosCom {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MICROSCOM_H
