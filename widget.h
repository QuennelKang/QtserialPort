#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QSerialPort>
#include<QString>
//#include <QtGlobal>
#include<QTimer>
#include<QByteArray>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pushButton_Open_clicked();

    void on_pushButton_Close_clicked();

    void on_pushButton_recvClear_clicked();

    void on_pushButton_sendClear_clicked();

    void SerialPortReadyRead_slot();

    void on_pushButton_Send_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_pushButton_chooseFile_clicked();

    void on_pushButton_sendFile_clicked();

private:
    Ui::Widget *ui;
    QSerialPort *serial;


    // 发送、接收字节计数
    long sendNum,recvNum;

    QTimer *timeSend;

    QByteArray binByteArray;
    int        dataLen=0;
};

#endif // WIDGET_H
