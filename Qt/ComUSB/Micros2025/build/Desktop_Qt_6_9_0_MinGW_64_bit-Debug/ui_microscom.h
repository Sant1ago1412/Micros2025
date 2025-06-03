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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MicrosCom
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_7;
    QHBoxLayout *horizontalLayout_4;
    QLCDNumber *Gy;
    QLCDNumber *Gz;
    QLCDNumber *Gx;
    QHBoxLayout *horizontalLayout_5;
    QLCDNumber *Ay;
    QLCDNumber *Az;
    QLCDNumber *Ax;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_5;
    QComboBox *SerialPort;
    QPushButton *Connect;
    QVBoxLayout *verticalLayout_4;
    QComboBox *Comand;
    QPushButton *SendButton;
    QPlainTextEdit *plainTextEdit;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MicrosCom)
    {
        if (MicrosCom->objectName().isEmpty())
            MicrosCom->setObjectName("MicrosCom");
        MicrosCom->resize(491, 370);
        centralwidget = new QWidget(MicrosCom);
        centralwidget->setObjectName("centralwidget");
        centralwidget->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
        centralwidget->setMouseTracking(false);
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName("verticalLayout_7");
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        Gy = new QLCDNumber(centralwidget);
        Gy->setObjectName("Gy");

        horizontalLayout_4->addWidget(Gy);

        Gz = new QLCDNumber(centralwidget);
        Gz->setObjectName("Gz");

        horizontalLayout_4->addWidget(Gz);

        Gx = new QLCDNumber(centralwidget);
        Gx->setObjectName("Gx");

        horizontalLayout_4->addWidget(Gx);


        verticalLayout_7->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        Ay = new QLCDNumber(centralwidget);
        Ay->setObjectName("Ay");

        horizontalLayout_5->addWidget(Ay);

        Az = new QLCDNumber(centralwidget);
        Az->setObjectName("Az");

        horizontalLayout_5->addWidget(Az);

        Ax = new QLCDNumber(centralwidget);
        Ax->setObjectName("Ax");

        horizontalLayout_5->addWidget(Ax);


        verticalLayout_7->addLayout(horizontalLayout_5);


        gridLayout->addLayout(verticalLayout_7, 2, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName("verticalLayout_5");
        SerialPort = new QComboBox(centralwidget);
        SerialPort->setObjectName("SerialPort");

        verticalLayout_5->addWidget(SerialPort);

        Connect = new QPushButton(centralwidget);
        Connect->setObjectName("Connect");

        verticalLayout_5->addWidget(Connect);


        horizontalLayout_2->addLayout(verticalLayout_5);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName("verticalLayout_4");
        Comand = new QComboBox(centralwidget);
        Comand->setObjectName("Comand");

        verticalLayout_4->addWidget(Comand);

        SendButton = new QPushButton(centralwidget);
        SendButton->setObjectName("SendButton");

        verticalLayout_4->addWidget(SendButton);


        horizontalLayout_2->addLayout(verticalLayout_4);


        gridLayout->addLayout(horizontalLayout_2, 0, 1, 1, 1);

        plainTextEdit = new QPlainTextEdit(centralwidget);
        plainTextEdit->setObjectName("plainTextEdit");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(plainTextEdit->sizePolicy().hasHeightForWidth());
        plainTextEdit->setSizePolicy(sizePolicy);

        gridLayout->addWidget(plainTextEdit, 2, 1, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);

        MicrosCom->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MicrosCom);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 491, 21));
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
