#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
    //TODO
    1.unpack data and store in analogData
    3.aoto-save when buffer is full
    4.print2consle
*/


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
            printToConsole("连接超时");
            return;
        }
        qDebug() << "Connect successfully!";
        printToConsole("连接成功");

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
    double avg[200];
    int xRange = settings->value("xRange").toInt();
    //读取缓冲区数据
    buffer = socket->readAll();
    if(!buffer.isEmpty())
    {
        // 解析返回值，判断
        qDebug() << buffer;
        qDebug() << buffer.size();

//        static int cnt = 0;
//        // fill in the analogData vector array
//        for (int i=0;i<200;i++) {
//            analogData[i].append(QPointF(cnt,buffer[rand()%1024]));
//            avg[i] = std::accumulate(analogData[i].begin(),analogData[i].end(),QPointF(0,0)).y();
//            avg[i] /= (cnt+1);
//        }
//        cnt++;

//        // refresh all
//        for (QChartView* qcv : this->findChildren<QChartView*>()) {
//            auto q = (QSplineSeries*)qcv->chart()->series()[0];
//            q->replace(analogData[qcv->objectName().toInt()-1]);
//            if(cnt>xRange){
//                qcv->chart()->axes(Qt::Horizontal)[0]->setRange(cnt-xRange, cnt);
//            }
//        }
////        update avg
//        for(int i=0;i<5;i++){
//            for(int j=0;j<40;j++){
//                auto label_temp = qobject_cast<QLabel *>(tab_layouts[i]->itemAtPosition(j-20<0?j:j-20,j-20>=0?3:1)->widget());
//                label_temp->setText(QString("avg:  ").append(QString().setNum(avg[i*40+j])));
//            }
//        }
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
}

