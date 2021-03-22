#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "threadTcp.h"


void MainWindow::tcpInit(){

    ui->lineEdit_IP->setText(settings->value("IP").toString());
    ui->lineEdit_Port->setText(settings->value("port").toString());
    threadSocket = new ThreadTcp(settings->value("interval").toInt());

    // connect slots with ui
    connect(threadSocket,SIGNAL(sig_print(QString)),this,SLOT(printToConsole(QString)));
    connect(threadSocket,SIGNAL(sig_connectFailed()),this,SLOT(connectFailed()));
    connect(threadSocket,SIGNAL(sig_connectSucceed()),this,SLOT(connectSucceed()));
    connect(threadSocket, SIGNAL(disconnected()), this, SLOT(disconnectSucceed()));
    connect(threadSocket,SIGNAL(sig_updateAnalogData(quint32*)),this,SLOT(refreshAnalogData(quint32*)),Qt::BlockingQueuedConnection);

    connect(this,SIGNAL(sig_connect(QString,int)),threadSocket,SLOT(connectTo(QString,int)));
    connect(this,SIGNAL(sig_disconnect()),threadSocket,SLOT(disconnectFrom()));
    connect(this, SIGNAL(sig_sendMessage(QByteArray)), threadSocket, SLOT(send(QByteArray)));


    tcpWorkThread = new QThread();
    threadSocket->moveToThread(tcpWorkThread);
    tcpWorkThread->start();

}

void MainWindow::connectFailed(){
    printToConsole("连接失败");
}

void MainWindow::connectSucceed(){
    printToConsole("连接成功");
    isConnected = true;
    emit stateChange();
    //发送按键使能
    ui->pushButton_start->setEnabled(true);
    ui->pushButton_stop->setEnabled(true);
    //修改按键文字
    ui->pushButton_Connect->setText("断开连接");
}

void MainWindow::disconnectSucceed(){
    printToConsole("断开连接");
    isConnected = false;
    emit stateChange();
    //修改按键文字
    ui->pushButton_Connect->setText("连接");
    ui->pushButton_start->setEnabled(false);
    ui->pushButton_stop->setEnabled(false);
}

void MainWindow::on_pushButton_Connect_clicked()
{
    if(ui->pushButton_Connect->text() == tr("连接"))
    {
        QString ip;
        int port;

        //获取IP地址
        ip = ui->lineEdit_IP->text();
        //获取端口号
        port = ui->lineEdit_Port->text().toInt();

        emit sig_connect(ip,port);

    }else{
        emit sig_disconnect();
    }
}


void MainWindow::sendMessage(QByteArray message)
{
    emit sig_sendMessage(message);
}
