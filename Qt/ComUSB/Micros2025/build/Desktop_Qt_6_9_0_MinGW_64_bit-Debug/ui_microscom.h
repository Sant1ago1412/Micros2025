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
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MicrosCom
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *SerialPort;
    QPushButton *Connect;
    QHBoxLayout *horizontalLayout_3;
    QComboBox *Comand;
    QPushButton *SendButton;
    QPlainTextEdit *plainTextEdit;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_4;
    QHBoxLayout *horizontalLayout_7;
    QSlider *verticalSlider_2;
    QSlider *verticalSlider;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout_6;
    QLCDNumber *lcdNumber;
    QLCDNumber *lcdNumber_2;
    QVBoxLayout *verticalLayout_4;
    QSpacerItem *verticalSpacer_2;
    QLabel *label;
    QHBoxLayout *horizontalLayout_4;
    QLCDNumber *Gx;
    QLCDNumber *Gy;
    QLCDNumber *Gz;
    QVBoxLayout *verticalLayout_3;
    QSpacerItem *verticalSpacer;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_5;
    QLCDNumber *Ax;
    QLCDNumber *Ay;
    QLCDNumber *Az;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MicrosCom)
    {
        if (MicrosCom->objectName().isEmpty())
            MicrosCom->setObjectName("MicrosCom");
        MicrosCom->resize(815, 458);
        centralwidget = new QWidget(MicrosCom);
        centralwidget->setObjectName("centralwidget");
        centralwidget->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
        centralwidget->setMouseTracking(false);
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName("gridLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(10);
        horizontalLayout->setObjectName("horizontalLayout");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        SerialPort = new QComboBox(centralwidget);
        SerialPort->setObjectName("SerialPort");

        horizontalLayout_2->addWidget(SerialPort);

        Connect = new QPushButton(centralwidget);
        Connect->setObjectName("Connect");

        horizontalLayout_2->addWidget(Connect);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        Comand = new QComboBox(centralwidget);
        Comand->setObjectName("Comand");

        horizontalLayout_3->addWidget(Comand);

        SendButton = new QPushButton(centralwidget);
        SendButton->setObjectName("SendButton");

        horizontalLayout_3->addWidget(SendButton);


        verticalLayout->addLayout(horizontalLayout_3);

        plainTextEdit = new QPlainTextEdit(centralwidget);
        plainTextEdit->setObjectName("plainTextEdit");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(plainTextEdit->sizePolicy().hasHeightForWidth());
        plainTextEdit->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(plainTextEdit);


        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName("label_4");

        verticalLayout_2->addWidget(label_4);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        verticalSlider_2 = new QSlider(centralwidget);
        verticalSlider_2->setObjectName("verticalSlider_2");
        verticalSlider_2->setOrientation(Qt::Orientation::Vertical);

        horizontalLayout_7->addWidget(verticalSlider_2);

        verticalSlider = new QSlider(centralwidget);
        verticalSlider->setObjectName("verticalSlider");
        verticalSlider->setOrientation(Qt::Orientation::Vertical);

        horizontalLayout_7->addWidget(verticalSlider);


        verticalLayout_2->addLayout(horizontalLayout_7);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");

        verticalLayout_2->addWidget(label_3);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        lcdNumber = new QLCDNumber(centralwidget);
        lcdNumber->setObjectName("lcdNumber");

        horizontalLayout_6->addWidget(lcdNumber);

        lcdNumber_2 = new QLCDNumber(centralwidget);
        lcdNumber_2->setObjectName("lcdNumber_2");

        horizontalLayout_6->addWidget(lcdNumber_2);


        verticalLayout_2->addLayout(horizontalLayout_6);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_2);

        label = new QLabel(centralwidget);
        label->setObjectName("label");

        verticalLayout_4->addWidget(label);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        Gx = new QLCDNumber(centralwidget);
        Gx->setObjectName("Gx");

        horizontalLayout_4->addWidget(Gx);

        Gy = new QLCDNumber(centralwidget);
        Gy->setObjectName("Gy");

        horizontalLayout_4->addWidget(Gy);

        Gz = new QLCDNumber(centralwidget);
        Gz->setObjectName("Gz");

        horizontalLayout_4->addWidget(Gz);

        horizontalLayout_4->setStretch(0, 1);
        horizontalLayout_4->setStretch(1, 1);
        horizontalLayout_4->setStretch(2, 1);

        verticalLayout_4->addLayout(horizontalLayout_4);

        verticalLayout_4->setStretch(2, 1);

        verticalLayout_2->addLayout(verticalLayout_4);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");

        verticalLayout_3->addWidget(label_2);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        Ax = new QLCDNumber(centralwidget);
        Ax->setObjectName("Ax");

        horizontalLayout_5->addWidget(Ax);

        Ay = new QLCDNumber(centralwidget);
        Ay->setObjectName("Ay");

        horizontalLayout_5->addWidget(Ay);

        Az = new QLCDNumber(centralwidget);
        Az->setObjectName("Az");

        horizontalLayout_5->addWidget(Az);

        horizontalLayout_5->setStretch(0, 1);
        horizontalLayout_5->setStretch(1, 1);
        horizontalLayout_5->setStretch(2, 1);

        verticalLayout_3->addLayout(horizontalLayout_5);

        verticalLayout_3->setStretch(2, 1);

        verticalLayout_2->addLayout(verticalLayout_3);


        horizontalLayout->addLayout(verticalLayout_2);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 2);

        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        MicrosCom->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MicrosCom);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 815, 21));
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
        label_4->setText(QCoreApplication::translate("MicrosCom", "Motores", nullptr));
        label_3->setText(QCoreApplication::translate("MicrosCom", "ADC", nullptr));
        label->setText(QCoreApplication::translate("MicrosCom", "Giroscope Values", nullptr));
        label_2->setText(QCoreApplication::translate("MicrosCom", "Acelerometter Values", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MicrosCom: public Ui_MicrosCom {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MICROSCOM_H
