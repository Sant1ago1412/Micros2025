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
    QComboBox *SerialPort;
    QComboBox *Comand;
    QPlainTextEdit *plainTextEdit;
    QPushButton *pushButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MicrosCom)
    {
        if (MicrosCom->objectName().isEmpty())
            MicrosCom->setObjectName("MicrosCom");
        MicrosCom->resize(800, 600);
        centralwidget = new QWidget(MicrosCom);
        centralwidget->setObjectName("centralwidget");
        SerialPort = new QComboBox(centralwidget);
        SerialPort->setObjectName("SerialPort");
        SerialPort->setGeometry(QRect(20, 30, 72, 24));
        Comand = new QComboBox(centralwidget);
        Comand->setObjectName("Comand");
        Comand->setGeometry(QRect(140, 30, 72, 24));
        plainTextEdit = new QPlainTextEdit(centralwidget);
        plainTextEdit->setObjectName("plainTextEdit");
        plainTextEdit->setGeometry(QRect(33, 189, 251, 201));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(20, 70, 80, 24));
        MicrosCom->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MicrosCom);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
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
        pushButton->setText(QCoreApplication::translate("MicrosCom", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MicrosCom: public Ui_MicrosCom {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MICROSCOM_H
