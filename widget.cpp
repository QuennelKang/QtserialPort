#include "widget.h"
#include "ui_widget.h"
#include<QSerialPort>
#include<QSerialPortInfo>
#include<QMessageBox>
#include<QDebug>
//调试接收数据中出现 \u \x等字符
#include<QDateTime>
//#include <QtGlobal> quint32
#include<QTimer>
#include<QFileDialog>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowTitle("Qt serialPort");
    ui->progressBar->hide();
//    ui->checkBox_addnewLine->hide();
    ui->progressBar->setValue(0);


    // 发送、接收计数清零
    sendNum = 0;
    recvNum = 0;

    serial = new QSerialPort(this);
    connect(serial, &QSerialPort::readyRead, this, &Widget::SerialPortReadyRead_slot);

    //遍历可用串口设备
    QStringList serialportName;
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        serialportName<<info.portName();
    }
    //添加到设备串口显示框
    ui->comboBox_Com->addItems(serialportName);

    /*发送文本框信号槽*/
//    connect(ui->plainTextEdit_Send, &QPlainTextEdit::textChanged, this, [=](){
//        //获取发送框字符
//        QString SendTextEditStr = ui->plainTextEdit_Send->toPlainText();
//        if (SendTextEditStr.isEmpty())
//        {
//            return;
//        }
//        //勾选hex发送则判断是否有非法hex字符
//        if (ui->checkBox_Hexsend->isChecked())
//        {
//            char ch;
//            bool flag = false;
//            uint32_t i, len;
//            //去掉无用符号
//            SendTextEditStr = SendTextEditStr.replace(' ',"");
//            SendTextEditStr = SendTextEditStr.replace(',',"");
//            SendTextEditStr = SendTextEditStr.replace('\r',"");
//            SendTextEditStr = SendTextEditStr.replace('\n',"");
//            SendTextEditStr = SendTextEditStr.replace('\t',"");
//            SendTextEditStr = SendTextEditStr.replace("0x","");
//            SendTextEditStr = SendTextEditStr.replace("0X","");
//            //判断数据合法性
//            for(i = 0, len = SendTextEditStr.length(); i < len; i++)
//            {
//                ch = SendTextEditStr.at(i).toLatin1();
//                if (ch >= '0' && ch <= '9') {
//                    flag = false;
//                } else if (ch >= 'a' && ch <= 'f') {
//                    flag = false;
//                } else if (ch >= 'A' && ch <= 'F') {
//                    flag = false;
//                } else {
//                    flag = true;
//                }
//            }
//            if(flag) QMessageBox::warning(this,"警告","输入内容包含非法16进制字符");
//        }
//        //QString转QByteArray
//        QByteArray SendTextEditBa = SendTextEditStr.toUtf8();
//    });

    // 定时发送-定时器
      timeSend = new QTimer(this);
      connect(timeSend, &QTimer::timeout, this, [=](){
          on_pushButton_Send_clicked();
      });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::SerialPortReadyRead_slot()
{
        /*    QString buf;
        buf = QString(serial->readAll());
        qDebug()<<"收到数据："<<buf;
        ui->plainTextEdit_Recv->insertPlainText(buf);*/

        QString frameData;

        QByteArray recBuf = serial->readAll();
        qDebug()<<"收到数据:"<<recBuf;
        ui->plainTextEdit_Recv->insertPlainText(recBuf);

        //计算字节数
        recvNum += recBuf.size();
        ui->label_Recvtotal->setText(QString::number(recvNum));

        //判断数据是否为空
        if(!recBuf.isEmpty())
        {
            if(ui->checkBox_Hexrecv->isChecked())
            {
                //十六进制显示
                frameData = recBuf.toHex(' ').trimmed().toUpper();
                //ui->plainTextEdit_Recv->(QColor(Qt::green));
            }
            else
            {
                frameData = QString(recBuf);
                //ui->plainTextEdit_Recv->setTextColor(QColor(Qt::magenta));
            }
            //时间戳
            if(ui->checkBox_Timedisp->isChecked())
            {
                frameData = QString("[%1]:RX -> %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")).arg(frameData);
                ui->plainTextEdit_Recv->appendPlainText(frameData);
            }
            else
            {
                ui->plainTextEdit_Recv->insertPlainText(frameData);
            }

        }
        ui->plainTextEdit_Recv->moveCursor(QTextCursor::End);
//       ui->plainTextEdit_Recv->appendPlainText(recBuf);
//       ui->plainTextEdit_Recv->insertPlainText(recBuf);

}

void Widget::on_pushButton_Open_clicked()
{
        //设置波特率
        switch (ui->comboBox_Baud->currentIndex()) {
        case 0:
                serial->setBaudRate(QSerialPort::Baud1200);
                break;
            case 1:
                serial->setBaudRate(QSerialPort::Baud2400);
                break;
            case 2:
                serial->setBaudRate(QSerialPort::Baud4800);
                break;
            case 3:
                serial->setBaudRate(QSerialPort::Baud9600);
                break;
            case 4:
                serial->setBaudRate(QSerialPort::Baud19200);
                break;
            case 5:
                serial->setBaudRate(QSerialPort::Baud38400);
                break;
            case 6:
                serial->setBaudRate(QSerialPort::Baud115200);
                break;
            default:
                break;
        };

        //设置校验位
        switch (ui->comboBox_Check->currentIndex()) {
            case 0:
                serial->setParity(QSerialPort::NoParity);
                break;
            case 1:
                serial->setParity(QSerialPort::OddParity);
                break;
            case 2:
                serial->setParity(QSerialPort::EvenParity);
                break;
            case 3:
                serial->setParity(QSerialPort::MarkParity);
                break;
            case 4:
                serial->setParity(QSerialPort::SpaceParity);
                break;
            default:
                break;
        }

        //设置数据位
        switch (ui->comboBox_Data->currentIndex()) {
            case 0:
                serial->setDataBits(QSerialPort::Data5);
                break;
            case 1:
                serial->setDataBits(QSerialPort::Data6);
                break;
            case 2:
                serial->setDataBits(QSerialPort::Data7);
                break;
            case 3:
                serial->setDataBits(QSerialPort::Data8);
                break;
            default:
                break;
        }

        // 设置停止位
        switch (ui->comboBox_Stop->currentIndex()) {
            case 0:
                serial->setStopBits(QSerialPort::OneStop);
                break;
            case 1:
                serial->setStopBits(QSerialPort::OneAndHalfStop);
                break;
            case 2:
                serial->setStopBits(QSerialPort::TwoStop);
                break;
            default:
                break;
        }

          //串口无详细信息
          serial->setPortName(ui->comboBox_Com->currentText());

        if(serial->open(QIODevice::ReadWrite) == true)
        {
            QMessageBox::information(this,"提示","成功");
            ui->comboBox_Baud->setDisabled(true);
            ui->comboBox_Com->setDisabled(true);
            ui->comboBox_Check->setDisabled(true);
            ui->comboBox_Data->setDisabled(true);
            ui->comboBox_Stop->setDisabled(true);
        }else
        {
            QMessageBox::critical(this,"提示","失败");
        }

        qDebug()<<"打开！";

}


void Widget::on_pushButton_Close_clicked()
{
        serial->close();
        ui->comboBox_Baud->setDisabled(false);
        ui->comboBox_Com->setDisabled(false);
        ui->comboBox_Check->setDisabled(false);
        ui->comboBox_Data->setDisabled(false);
        ui->comboBox_Stop->setDisabled(false);
        //serial->deleteLater();
        qDebug()<<"关闭！";
}


void Widget::on_pushButton_recvClear_clicked()
{
    ui->plainTextEdit_Recv->clear();
    ui->label_Recvtotal->setText(QString::number(0));
    recvNum = 0;
}


void Widget::on_pushButton_sendClear_clicked()
{
    ui->plainTextEdit_Send->clear();
    ui->label_Sendtotal->setText(QString::number(0));
    sendNum = 0;
}

void Widget::on_pushButton_Send_clicked()
{
//   serial->write(ui->plainTextEdit_Send->toPlainText().toLocal8Bit().data());

   QString senddataStr;

   QByteArray sendData;
   sendData = ui->plainTextEdit_Send->toPlainText().toLocal8Bit().data();
   qDebug()<<"发送数据:"<<sendData;


   //计算字节数
   sendNum += sendData.size();
   ui->label_Sendtotal->setText(QString::number(sendNum));

   if(ui->checkBox_addnewLine->isChecked())
   {
       sendData.append("\r\n");
   }

   if(ui->checkBox_Timedisp->isChecked())
    {
       senddataStr = QString("\n[%1]:TX -> %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")).arg(senddataStr);
       ui->plainTextEdit_Recv->insertPlainText(senddataStr);
   }

//   if(ui->checkBox_Hexsend->isChecked())
//    {
//        sendData = sendData.toHex(' ').toUpper();
//        ui->plainTextEdit_Send->document()->setPlainText(sendData);
//    }
//   else
//   {
//        sendData = ui->plainTextEdit_Send->toPlainText().toLocal8Bit().data();
//   }

    serial->write(ui->plainTextEdit_Send->toPlainText().toLocal8Bit().data());
}


void Widget::on_checkBox_stateChanged(int arg1)
{
   int time;

    if(arg1 == Qt::Checked)
    {
            time = ui->lineEdit_Timer->text().toInt();
            if (time > 0)
            {
                     timeSend->start(time);
             }
            else
            {
                     QMessageBox::warning(this, "警告", "时间必须大于0");
             }
                 ui->lineEdit_Timer->setEnabled(false);
     }
     else
     {
         /*停止发送*/
         timeSend->stop();
         ui->lineEdit_Timer->setEnabled(true);
     }

}

void Widget::on_pushButton_chooseFile_clicked()
{
    QString initialName;
    QString fileName =
            QFileDialog::getOpenFileName(this,tr("choose file"),initialName);  //at the path:initialName ,user choose which file to upload
    fileName = QDir::toNativeSeparators(fileName);
    if(!fileName.isEmpty())
    {

        ui->plainTextEdit_Send->appendPlainText(fileName);
        QFile file(fileName);
        ui->plainTextEdit_Send->appendPlainText("\n");
       if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {

           qDebug()<<"Can't open the file!"<<endl;
       }
     //  QDataStream in(&file);
    //   datalen = in.readRawData(dataBuf,10);
    //   serial->write(dataBuf,10);
       while(!file.atEnd())
       {

           //QByteArray buf = file.readLine(512);
           binByteArray.append(file.readLine(512));
       }
       dataLen = file.size();
       ui->plainTextEdit_Send->appendPlainText(QString("%1").arg((dataLen)));
       ui->plainTextEdit_Send->appendPlainText("\n");

//       ui->progressBar->setMinimum(0);
//       ui->progressBar->setMaximum(dataLen/1024);
//       ui->progressBar->setValue(0);

       //ui->textEdit->append(binByteArray.toHex());

       file.close();
     }
}

void Widget::on_pushButton_sendFile_clicked()
{
    int length=0;
    length = sizeof(binByteArray)>>10;
    ui->plainTextEdit_Send->appendPlainText(QString(length));

   ui->progressBar->setMinimum(0);
   ui->progressBar->setMaximum(length/1024);
   ui->progressBar->setValue(100);


    serial->write(binByteArray);
    //serial->write(InsyncData,sizeof(InsyncData));
    //plainTextEdit->insertPlainText(binByteArray.toHex());
    ui->plainTextEdit_Recv->appendPlainText(tr("\n send OK \n"));
}
