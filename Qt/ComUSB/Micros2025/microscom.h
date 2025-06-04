#ifndef MICROSCOM_H
#define MICROSCOM_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QTimer>
#include <QInputDialog>
// #include <dialog.h>
// #include <qpaintbox.h>
#include <QPainter>
#include <math.h>
#include <QMenuBar>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>
#include <QSlider>

typedef union{
    uint8_t     u8[4];
    int8_t      i8[4];
    uint16_t    u16[2];
    int16_t     i16[2];
    uint32_t    u32;
    int32_t     i32;
}_work;

typedef enum{
    START,
    HEADER_1,
    HEADER_2,
    HEADER_3,
    NBYTES,
    TOKEN,
    PAYLOAD
}_eProtocolo;

QT_BEGIN_NAMESPACE
namespace Ui {
class MicrosCom;
}
QT_END_NAMESPACE

class MicrosCom : public QMainWindow
{
    Q_OBJECT

public:
    MicrosCom(QWidget *parent = nullptr);
    ~MicrosCom();
private slots:

    void OnRxChar();

    void OnQTimer1();

    bool eventFilter(QObject *watched, QEvent *event) override;

    void on_pushButton_clicked();

    void on_Connect_pressed();

    void on_SendButton_pressed();

private:
    Ui::MicrosCom *ui;
    QTimer *QTimer1;
    QSerialPort *QSerialPort1;
    // QPaintBox *QPaintBox1;
    QMenuBar *menuBar;
    // Dialog *dialog1;
    uint8_t rxBuf[256], header, nBytes, cks, index, tmoRX,LedsValue,LedstoAct,j;
    _eProtocolo estadoProtocolo;
    _work w;
    void SendCMD(uint8_t *buf, uint8_t length);
    void DecodeCmd(uint8_t *rxBuf);

    typedef enum{
        ALIVE=0xF0,
        FIRMWARE=0xF1,
        ACKNOWLEDGE=0x0D,
        UNKNOWNCOMANND=0xFF
    }_eIDCommand;
};
#endif // MICROSCOM_H
