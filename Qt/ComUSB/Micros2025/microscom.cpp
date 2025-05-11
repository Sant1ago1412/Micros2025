#include "microscom.h"
#include "ui_microscom.h"

MicrosCom::MicrosCom(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MicrosCom)
{
    ui->setupUi(this);

    this->setWindowTitle("AUTO CP3");

    setStyleSheet( "QInputDialog {background-color: rgb(0,85,0); font: bold italic large Comic Sans Ms; font: bold 40px;}");

    QSerialPort1=new QSerialPort(this);

    ui->SerialPort->installEventFilter(this);

    ui->Comand->addItem("ALIVE", 0xF0);
    ui->Comand->addItem("FIRMWARE", 0xF1);
    header=01;
    connect(QSerialPort1,&QSerialPort::readyRead, this,&MicrosCom::OnRxChar);

    QTimer1 = new QTimer(this);
    connect(QTimer1, &QTimer::timeout, this, &MicrosCom::OnQTimer1);

    // connect(ui->actionconnectUDP, &QAction::triggered, this, &QForm1::ConectaUDP);
    // dialog1 = new Dialog(this);

    // QUdpSocket1 =new QUdpSocket(this);
    // connect(QUdpSocket1,&QUdpSocket::readyRead,this,&QForm1::onRxUDP);

    QTimer1->start(200);
}

MicrosCom::~MicrosCom()
{
    delete ui;
    // delete QPaintBox1;
    // delete QUdpSocket1;
    delete QSerialPort1;
}

void MicrosCom::OnRxChar(){
    int count;
    uint8_t *buf;
    QString strHex;

    count = QSerialPort1->bytesAvailable();
    if(count <= 0)
        return;

    buf = new uint8_t[count];
    QSerialPort1->read((char *)buf, count);

    strHex = "SERIAL<-- 0x";
    for (int a=0; a<count; a++) {
        strHex = strHex + QString("%1").arg(buf[a], 2, 16, QChar('0')).toUpper();
    }
    ui->plainTextEdit->appendPlainText(strHex);
    for (int i=0; i<count; i++) {
        strHex = strHex + QString("%1").arg(buf[i], 2, 16, QChar('0')).toUpper();

        switch(header){
        case 0:
            if(buf[i] == 'U'){
                header = 1;
                tmoRX = 5;
            }
            break;
        case 1:
            if(buf[i] == 'N')
                header = 2;
            else{
                header = 0;
                i--;
            }
            break;
        case 2:
            if(buf[i] == 'E')
                header = 3;
            else{
                header = 0;
                i--;
            }
            break;
        case 3:
            if(buf[i] == 'R')
                header = 4;
            else{
                header = 0;
                i--;
            }
            break;
        case 4:
            nBytes = buf[i];
            header = 5;
            break;
        case 5:
            if(buf[i] == ':'){
                header = 6;
                index = 0;
                cks = 'U' ^ 'N' ^ 'E' ^ 'R' ^ ':' ^ nBytes;
            }
            else{
                header = 0;
                i--;
            }
            break;
        case 6:
            nBytes--;
            if(nBytes > 0){
                rxBuf[index++] = buf[i];
                cks ^= buf[i];
            }
            else{
                header = 0;
                if(cks == buf[i])
                    DecodeCmd(rxBuf);
                else{
                    ui->plainTextEdit->appendPlainText("ERROR CHECKSUM");
                }
            }
            break;
        }
    }
    delete [] buf;
}
void MicrosCom::DecodeCmd(uint8_t *rxBuf){

    switch (rxBuf[0]) {
    case ALIVE:

        if(rxBuf[1] == ACKNOWLEDGE)
            ui->plainTextEdit->appendPlainText("I'M ALIVE");
        break;

    case UNKNOWNCOMANND:

        ui->plainTextEdit->appendPlainText("NO CMD");
        break;
    default:

        break;
    }
}

void MicrosCom::OnQTimer1(){

}

void MicrosCom::on_pushButton_clicked()
{
    if(QSerialPort1->isOpen()){
        QSerialPort1->close();
        ui->pushButton->setText("OPEN");
    }
    else{
        if(ui->comboBox->currentText() == "")
            return;

        QSerialPort1->setPortName(ui->comboBox->currentText());
        QSerialPort1->setBaudRate(115200);
        QSerialPort1->setParity(QSerialPort::NoParity);
        QSerialPort1->setDataBits(QSerialPort::Data8);
        QSerialPort1->setStopBits(QSerialPort::OneStop);
        QSerialPort1->setFlowControl(QSerialPort::NoFlowControl);

        if(QSerialPort1->open(QSerialPort::ReadWrite)){
            ui->pushButton->setText("CLOSE");
        }
        else
            QMessageBox::information(this, "Serial PORT", "ERROR. Opening PORT");
    }
}
