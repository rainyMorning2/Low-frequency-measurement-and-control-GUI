#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::tcpInit(){
    socket = new QTcpSocket();
    //连接信号槽
    connect(socket, SIGNAL(readyRead()),this,SLOT(socket_Read_Data()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(socket_Disconnected()));

    ui->lineEdit_IP->setText(settings->value("IP").toString());
    ui->lineEdit_Port->setText(settings->value("port").toString());
}


void MainWindow::on_pushButton_Connect_clicked()
{
    if(ui->pushButton_Connect->text() == tr("连接"))
    {
        QString IP;
        int port;

        //获取IP地址
        IP = ui->lineEdit_IP->text();
        //获取端口号
        port = ui->lineEdit_Port->text().toInt();

        //取消已有的连接
        socket->abort();
        //连接服务器
        socket->connectToHost(IP, port);

        //等待连接成功
        if(!socket->waitForConnected(30000))
        {
            qDebug() << "Connection failed!";
            return;
        }
        qDebug() << "Connect successfully!";

        //发送按键使能
        ui->pushButton_start->setEnabled(true);
        ui->pushButton_stop->setEnabled(true);
        //修改按键文字
        ui->pushButton_Connect->setText("断开连接");
    }
    else
    {
        //断开连接
        socket->disconnectFromHost();
        //修改按键文字
        ui->pushButton_Connect->setText("连接");
        ui->pushButton_start->setEnabled(false);
        ui->pushButton_stop->setEnabled(false);
    }
}

void MainWindow::socket_Read_Data()
{
    QByteArray buffer;
    //读取缓冲区数据
    buffer = socket->readAll();
    if(!buffer.isEmpty())
    {
        // 解析返回值，判断
//        qDebug() << tr(buffer);
        qDebug() << buffer.size();

        static int cnt = 0;
        // fill in the analogData vector array
        for (int i=0;i<200;i++) {
            analogData[i].append(QPointF(cnt,buffer[rand()%1024]));
            qDebug()<<analogData[i];
        }
        cnt++;
        // refresh all
        for (QChartView* qcv : this->findChildren<QChartView*>()) {
            auto q = (QSplineSeries*)qcv->chart()->series()[0];
            q->replace(analogData[qcv->objectName().toInt()]);
        }


    }
}


void MainWindow::sendMessage(QString message)
{
    socket->write(message.toLatin1());
    socket->flush();
}


void MainWindow::socket_Disconnected()
{
    //发送按键失能
    ui->pushButton_start->setEnabled(false);
    ui->pushButton_stop->setEnabled(false);
    //修改按键文字
    ui->pushButton_Connect->setText("连接");
    qDebug() << "断开连接!";
}

