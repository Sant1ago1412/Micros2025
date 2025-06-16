#include "microscom.h"
#include "ui_microscom.h"

MicrosCom::MicrosCom(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MicrosCom)
{
    ui->setupUi(this);

    this->setWindowTitle("AUTO MICROS");

//    setStyleSheet( "QInputDialog {background-color: rgb(0,85,0); font: bold italic large Comic Sans Ms; font: bold 40px;}");

    QSerialPort1=new QSerialPort(this);

    ui->SerialPort->installEventFilter(this);

    ui->Comand->addItem("ALIVE", 0xF0);
    ui->Comand->addItem("FIRMWARE", 0xF1);
    ui->Comand->addItem("MOTORES", 0xF3);
    ui->Comand->addItem("MPUDATA", 0xF4);
    ui->Comand->addItem("ADCVALUES", 0xF5);
    header=01;
    connect(QSerialPort1,&QSerialPort::readyRead, this,&MicrosCom::OnRxChar);

    QTimer1 = new QTimer(this);
    connect(QTimer1, &QTimer::timeout, this, &MicrosCom::OnQTimer1);

    // connect(ui->actionconnectUDP, &QAction::triggered, this, &QForm1::ConectaUDP);
    // dialog1 = new Dialog(this);

    // QUdpSocket1 =new QUdpSocket(this);
    // connect(QUdpSocket1,&QUdpSocket::readyRead,this,&QForm1::onRxUDP);

    QTimer1->start(100);
}

MicrosCom::~MicrosCom()
{
    delete ui;
    // delete QPaintBox1;
    // delete QUdpSocket1;
    delete QSerialPort1;
}

bool MicrosCom::eventFilter(QObject *watched, QEvent *event){
    if(watched == ui->SerialPort) {
        if (event->type() == QEvent::MouseButtonPress) {
            ui->SerialPort->clear();
            QSerialPortInfo SerialPortInfo1;

            for(int i=0;i<SerialPortInfo1.availablePorts().count();i++)
                ui->SerialPort->addItem(SerialPortInfo1.availablePorts().at(i).portName());

            return QMainWindow::eventFilter(watched, event);
        }
        else {
            return false;
        }
    }
    else{
        return QMainWindow::eventFilter(watched, event);
    }
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
        strHex = strHex + QString("%1 ").arg(buf[a], 2, 16, QChar('0')).toUpper();
    }
    ui->plainTextEdit->appendPlainText(strHex);
    for (int i=0; i<count; i++) {
        strHex = strHex + QString("%1 ").arg(buf[i], 2, 16, QChar('0')).toUpper();

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

        ui->plainTextEdit->appendPlainText("I'M ALIVE");
        break;

    case UNKNOWNCOMANND:

        ui->plainTextEdit->appendPlainText("NO CMD");
        break;
    case MPUDATA:
        ui->plainTextEdit->appendPlainText("DatoMPU");
        w.u8[0]=rxBuf[1];
        w.u8[1]=rxBuf[2];
        ui->Ax->display(w.i16[0]/(float)16384);
        w.u8[0]=rxBuf[3];
        w.u8[1]=rxBuf[4];
        ui->Ay->display(w.i16[0]/(float)16384);
        w.u8[0]=rxBuf[5];
        w.u8[1]=rxBuf[6];
        ui->Az->display(w.i16[0]/(float)16384+1);

        w.u8[0]=rxBuf[7];
        w.u8[1]=rxBuf[8];
        ui->Gx->display(w.i16[0]/(float)131);
        w.u8[0]=rxBuf[9];
        w.u8[1]=rxBuf[10];
        ui->Gy->display(w.i16[0]/(float)131);
        w.u8[0]=rxBuf[11];
        w.u8[1]=rxBuf[12];
        ui->Gz->display(w.i16[0]/(float)131);
        break;

    case ADCVALUES:

        w.u8[0]=rxBuf[1];
        w.u8[1]=rxBuf[2];
        ui->ADC1->display(w.u16[0]);
        w.u8[0]=rxBuf[3];
        w.u8[1]=rxBuf[4];
        ui->ADC2->display(w.u16[0]);
        w.u8[0]=rxBuf[5];
        w.u8[1]=rxBuf[6];
        ui->ADC3->display(w.u16[0]);
        w.u8[0]=rxBuf[7];
        w.u8[1]=rxBuf[8];
        ui->ADC4->display(w.u16[0]);
        w.u8[0]=rxBuf[9];
        w.u8[1]=rxBuf[10];
        ui->ADC5->display(w.u16[0]);
        w.u8[0]=rxBuf[11];
        w.u8[1]=rxBuf[12];
        ui->ADC6->display(w.u16[0]);
        w.u8[0]=rxBuf[13];
        w.u8[1]=rxBuf[14];
        ui->ADC7->display(w.u16[0]);
        w.u8[0]=rxBuf[15];
        w.u8[1]=rxBuf[16];
        ui->ADC8->display(w.u16[0]);

        break;
    default:

        break;
    }
}

void MicrosCom::OnQTimer1(){
    uint8_t buf=(uint8_t)ADCVALUES;

    SendCMD(&buf,1);
}

void MicrosCom::on_Connect_pressed()
{
    if(QSerialPort1->isOpen()){
        QSerialPort1->close();
        ui->Connect->setText("OPEN");
    }
    else{
        if(ui->SerialPort->currentText() == "")
            return;

        QSerialPort1->setPortName(ui->SerialPort->currentText());
        QSerialPort1->setBaudRate(115200);
        QSerialPort1->setParity(QSerialPort::NoParity);
        QSerialPort1->setDataBits(QSerialPort::Data8);
        QSerialPort1->setStopBits(QSerialPort::OneStop);
        QSerialPort1->setFlowControl(QSerialPort::NoFlowControl);

        if(QSerialPort1->open(QSerialPort::ReadWrite)){
            ui->Connect->setText("CLOSE");
        }
        else
            QMessageBox::information(this, "Serial PORT", "ERROR. Opening PORT");
    }
}


void MicrosCom::on_SendButton_pressed()
{
    uint8_t cmd,buf[24];
    int n;

    if(ui->Comand->currentText() == "")
        return;

    cmd = ui->Comand->currentData().toInt();
    ui->plainTextEdit->appendPlainText("0x" + (QString("%1").arg(cmd, 2, 16, QChar('0'))).toUpper());

    n = 0;
    switch (cmd) {
    case ALIVE://ALIVE   PC=>MBED 0xF0 ;  MBED=>PC 0xF0 0x0D
        n = 1;
        break;
    case FIRMWARE://FIRMWARE   PC=>MBED 0xF1 ;  MBED=>PC 0xF1 FIRMWARE
        n = 1;
        break;
    case MOTORES:
        n=4;
        w.i16[0]=ui->M1->value();
        w.i16[1]=ui->M2->value();
        break;
    default:
        break;
    }
    if(n){
        buf[0] = cmd;
        SendCMD(buf, n);
 //       SendCMDUDP(buf,n);
    }
}
void MicrosCom::SendCMD(uint8_t *buf, uint8_t length){
    uint8_t tx[24];
    uint8_t cks, i;
    QString strHex;

    if(!QSerialPort1->isOpen())
        return;

    tx[0] = 'U';
    tx[1] = 'N';
    tx[2] = 'E';
    tx[3] = 'R';
    tx[4] = length + 1;
    tx[5] = ':';

    memcpy(&tx[6], buf, length);

    cks = 0;
    for (i=0; i<(length+6); i++) {
        cks ^= tx[i];
    }

    tx[i] = cks;

    strHex = "S--> 0x";
    for (int i=0; i<length+7; i++) {
        strHex = strHex + QString("%1").arg(tx[i], 2, 16, QChar('0')).toUpper();
    }

    ui->plainTextEdit->appendPlainText(strHex);

    QSerialPort1->write((char *)tx, length+7);

}

