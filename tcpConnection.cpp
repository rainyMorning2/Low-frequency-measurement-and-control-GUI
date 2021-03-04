#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
    //TODO
    1.unpack data and store in analogData
    3.aoto-save when buffer is full
    4.print2consle
*/

static int disNum = 0;

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
        if(!socket->waitForConnected(10000))
        {
            qDebug() << "Connection failed!";
            printToConsole("连接超时");
            return;
        }
        qDebug() << "Connect successfully!";
        printToConsole("连接成功");
        isConnected = true;
        emit stateChange();
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
        isConnected = false;
        emit stateChange();
        //修改按键文字
        ui->pushButton_Connect->setText("连接");
        ui->pushButton_start->setEnabled(false);
        ui->pushButton_stop->setEnabled(false);
    }
}

void MainWindow::parse_data(QByteArray buffer){

    if(buffer.size()==1024 && disNum<5){
        QDataStream data_stream(&buffer, QIODevice::ReadOnly);
        data_stream.setByteOrder(QDataStream::LittleEndian);
        quint32 data[256];
        QString temp="";
        for(int i=0;i<256;i++){
            data_stream>>data[i];
            temp += QString("%1").arg(data[i],8,16,QLatin1Char('0'));
            temp+=" ";
            if((i+1)%8==0){
                printToConsole(temp);
                temp = "";
            }
        }
        printToConsole(QString::number(buffer.size()));
        disNum++;
    }
}

// 4 Bytes reverse  
// sent 1 2 3 4 
// recieved 4 3 2 1 
// total 1024B 
void MainWindow::socket_Read_Data()
{
    QByteArray buffer;
    //读取缓冲区数据
    if(socket->bytesAvailable()>=1024){
        buffer = socket->read(1024);
    }else{
        return;
    }

    if(!buffer.isEmpty())
    {
        // 解析返回值，判断
//        qDebug() << buffer;
//        qDebug() << buffer.toHex();
//        qDebug() << buffer.size();
//        printToConsole(buffer.toHex());

//        static int cnt = 0;
        // fill in the analogData vector array

        parse_data(buffer);

//        for (int i=0;i<200;i++) {
//            analogData[i].append(QPointF(cnt,buffer[rand()%1024]));
//        }
//        cnt++;

//        refreshChart(cnt);
    }
}


void MainWindow::sendMessage(QByteArray message)
{
    socket->write(message);
    socket->flush();
}


void MainWindow::socket_Disconnected()
{
    //发送按键失能
    ui->pushButton_start->setEnabled(false);
    ui->pushButton_stop->setEnabled(false);
    //修改按键文字
    ui->pushButton_Connect->setText("连接");
    qDebug() << "disconnected！";
    printToConsole("断开连接");
    isConnected = false;
    emit stateChange();
}

