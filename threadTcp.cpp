#include "threadTcp.h"

bool isStart = false;

ThreadTcp::ThreadTcp()
{
    data_stream.setDevice(this);
    data_stream.setByteOrder(QDataStream::LittleEndian);
    connect(this, SIGNAL(readyRead()),this,SLOT(socket_Read_Data()));
    stop.fill(0,1024);

    stop[0] = 0xFA;
    stop[1] = 0xF3;
    stop[2] = 0x20;

    stop[7] = 0x7A;
    stop[6] = 0x02;
    stop[5] = 0x20;

    for(int i=3;i<1023;i++){
        stop[1023] = stop[i] ^ stop[1023];
    }
}


ThreadTcp::~ThreadTcp(){
    delete []  data;
}


void ThreadTcp::connectTo(QString ip,int port){
    //取消已有的连接
    abort();
    //连接服务器
    connectToHost(ip, port);
    //等待连接成功
    if(!waitForConnected(1000))
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
    QFile outFile(QCoreApplication::applicationDirPath() + "/sentData.dat");
    outFile.open(QIODevice::WriteOnly);
    outFile.write(message);
    outFile.close();
    write(message);
    flush();
}


// 4 Bytes reverse
// sent 1 2 3 4
// recieved 4 3 2 1
// total 1024B

void ThreadTcp::socket_Read_Data()
{
    if(bytesAvailable()>=1024*len && !isStart){

        for(int j=0;j<len;j++){
            for(int i=0;i<256;i++){
                data_stream>>data[j*256+i];
            }
        }
        isStart = true;
        receievedAllData.wakeAll();

        // send stop signal
        send(stop);

    }else{
        return;
    }
//    QFile outFile(QCoreApplication::applicationDirPath() + "/sentData222.dat");
//    outFile.open(QIODevice::WriteOnly);
//    outFile.write(readAll());
//    outFile.close();
}


void ThreadTcp::setLen(int x){
    len = x;
    data = new quint32[256*len];
    isStart = false;
    readAll();
}
