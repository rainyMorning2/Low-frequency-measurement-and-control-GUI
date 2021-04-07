#include "threadTcp.h"

quint32* data;
bool isStart = false;

ThreadTcp::ThreadTcp(int interval)
{
    data_stream.setDevice(this);
    data_stream.setByteOrder(QDataStream::LittleEndian);
    timer = new QTimer(this);
    timer->setInterval(interval);
    data = new quint32[256];

    connect(this, SIGNAL(readyRead()),this,SLOT(socket_Read_Data()));
    connect(timer,SIGNAL(timeout()),this,SLOT(updateAnalogData()));

}


ThreadTcp::~ThreadTcp(){
    delete  timer;
    delete []  data;
    delete data;
}


void ThreadTcp::connectTo(QString ip,int port){
    //取消已有的连接
    abort();
    //连接服务器
    connectToHost(ip, port);
    //等待连接成功
    if(!waitForConnected(10000))
    {
        emit sig_connectFailed();
        return;
    }
    emit sig_connectSucceed();
}

void ThreadTcp::disconnectFrom(){
    //断开连接
    disconnectFromHost();
}

void ThreadTcp::send(QByteArray message){
    write(message);
    flush();
}


// 4 Bytes reverse
// sent 1 2 3 4
// recieved 4 3 2 1
// total 1024B

void ThreadTcp::socket_Read_Data()
{
    if(bytesAvailable()>=1024 && !isStart){
        timer->start();
        isStart = true;
        emit sig_realTimeRevChanged(isStart);
    }else{
        return;
    }
}

void ThreadTcp::updateAnalogData(){
    static int cnt = 0;
    if(bytesAvailable()<1024){
        timer->stop();
        isStart = false;
        emit sig_realTimeRevChanged(isStart);
        return;
    }

    for(int i=0;i<256;i++){
        data_stream>>data[i];
//        if(i==0){
//            while((data[i]&0xFFFF0000) != 0x7E7E0000){
//                data_stream>>data[i];
//                qDebug()<<QString("%1").arg(data[i],8,16,QLatin1Char('0'));
//            }
//        }
    }

//    if((data[255]&0x0000FFFF) != 0x00001A1A){
//        return;
//    }

    qDebug()<<cnt<<" socket left"<<bytesAvailable();
    cnt++;
    emit sig_updateAnalogData(data);
}
