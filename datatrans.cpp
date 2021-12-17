#include "datatrans.h"

QWaitCondition receievedAllData;
QMutex mutex;
//11
const int CHANNELCODE[200] = {511,	9,	12,	14,	27,	28,	29,	3,	1,	0,	19,	18,	21,	47,	39,	36,	35,	54,	10,	8,	13,	26,	25,	24,	30,	2,	4,	5,	6,	17,	38,	37,	34,	33,	53,	52,	48,	49,	50,	51,	75,	73,	76,	78,	91,	92,	93,	67,	65,	64,	83,	82,	85,	111,	103,	100,	99,	118,	74,	72,	77,	90,	89,	88,	94,	66,	68,	69,	70,	81,	102,	101,	98,	97,	117,	116,	112,	113,	114,	115,	139,	137,	140,	142,	155,	156,	157,	131,	129,	128,	147,	146,	149,	175,	167,	164,	163,	182,	138,	136,	141,	154,	153,	152,	158,	130,	132,	133,	134,	145,	166,	165,	162,	161,	181,	180,	176,	177,	178,	179,	203,	201,	204,	206,	219,	220,	221,	195,	193,	192,	211,	210,	213,	239,	231,	228,	227,	246,	202,	200,	205,	218,	217,	216,	222,	194,	196,	197,	198,	209,	230,	229,	226,	225,	245,	244,	240,	241,	242,	243,	267,	265,	268,	270,	283,	284,	285,	259,	257,	256,	275,	274,	277,	303,	295,	292,	291,	310,	266,	264,	269,	282,	281,	280,	286,	258,	260,	261,	262,	273,	294,	293,	290,	289,	309,	308,	304,	305,	306,	307};
//const int CHANNELCODE[200] = {395,	393,	396,	398,	411,	412,	413,	387,	385,	384,	403,	402,	405,	431,	423,	420,	419,	438,	394,	392,	397,	410,	409,	408,	414,	386,	388,	389,	390,	401,	422,	421,	418,	417,	437,	436,	432,	433,	434,	435,	75,	73,	76,	78,	91,	92,	93,	67,	65,	64,	83,	82,	85,	111,	103,	100,	99,	118,	74,	72,	77,	90,	89,	88,	94,	66,	68,	69,	70,	81,	102,	101,	98,	97,	117,	116,	112,	113,	114,	115,	139,	137,	140,	142,	155,	156,	157,	131,	129,	128,	147,	146,	149,	175,	167,	164,	163,	182,	138,	136,	141,	154,	153,	152,	158,	130,	132,	133,	134,	145,	166,	165,	162,	161,	181,	180,	176,	177,	178,	179,	203,	201,	204,	206,	219,	220,	221,	195,	193,	192,	211,	210,	213,	239,	231,	228,	227,	246,	202,	200,	205,	218,	217,	216,	222,	194,	196,	197,	198,	209,	230,	229,	226,	225,	245,	244,	240,	241,	242,	243,	267,	265,	268,	270,	283,	284,	285,	259,	257,	256,	275,	274,	277,	303,	295,	292,	291,	310,	266,	264,	269,	282,	281,	280,	286,	258,	260,	261,	262,	273,	294,	293,	290,	289,	309,	308,	304,	305,	306,	307};
const int SELFCHECKCODE[30] = {7,	23,	15,	31,	55,	32,	71,	87,	79,	95,	119,	96,	135,	151,	143,	159,	183,	160,	199,	215,	207,	223,	247,	224,	263,	279,	271,	287,	311,	288};

DataTrans::DataTrans()
{
    TcpConnectionState = false;
    threadTcpSocket = new ThreadTcp();

    connect(threadTcpSocket,SIGNAL(sig_connectFailed()),this,SLOT(connectFailed()));
    connect(threadTcpSocket,SIGNAL(sig_connectSucceed()),this,SLOT(connectSucceed()));
    connect(threadTcpSocket, SIGNAL(disconnected()), this, SLOT(disconnectSucceed()));

    connect(this,SIGNAL(sig_connect(QString,int)),threadTcpSocket,SLOT(connectTo(QString,int)));
    connect(this,SIGNAL(sig_disconnect()),threadTcpSocket,SLOT(disconnectFrom()));
    connect(this, SIGNAL(sig_sendMessage(QByteArray)), threadTcpSocket, SLOT(send(QByteArray)));
    connect(this,SIGNAL(sig_setLen(int)),threadTcpSocket,SLOT(setLen(int)),Qt::BlockingQueuedConnection);

    tcpWorkThread = new QThread();
    threadTcpSocket->moveToThread(tcpWorkThread);
    tcpWorkThread->start();
}

void DataTrans::connetTo(QString ip, int port){
    emit sig_connect(ip,port);
}

void DataTrans::setSingleChannel(int channel){
    QByteArray mess(1024,0);
    // package head
    mess[0] = 0xFA;
    mess[1] = 0xF3;
    mess[2] = 0x20;

    mess[3] = 1<<3;

    QByteArray mess2(64,0);

    // head
    mess2[0] = 0x43;
    mess2[1] = 0x4f;
    mess2[2] = 0x4d;
    mess2[3] = 0x44;

    // mode
    mess2[7] = 0x02;  // 0x02 // single channel

    // channels

    //mess2[58] [59] // single channel
    mess2[58] = CHANNELCODE[channel-1]>>8;
    mess2[59] = CHANNELCODE[channel-1];

    mess2[60] = 0x4f;
    mess2[61] = 0x56;
    mess2[62] = 0x45;
    mess2[63] = 0x52;

    mess.replace(79,64,mess2);

    // check byte as package end
    for(int i=3;i<1023;i++){
        mess[1023] = mess[i] ^ mess[1023];
    }

    sendMessage(mess);
}

void DataTrans::setChannel(int* channels,int size,bool same){
    QByteArray mess(1024,0);
    // package head
    mess[0] = 0xFA;
    mess[1] = 0xF3;
    mess[2] = 0x20;

    mess[3] = 1<<3;

    QByteArray mess2(64,0);
    int list[20];
    for(int i=0;i<20;i++){
        if(same){
            list[i] = channels[0];
        }else{
            if(i<size){
                list[i] = channels[i];
            }else{
                list[i] = 1;
            }
        }
        qDebug()<<list[i];
    }

    // head
    mess2[0] = 0x43;
    mess2[1] = 0x4f;
    mess2[2] = 0x4d;
    mess2[3] = 0x44;

    // mode
    mess2[7] = 0x01;  // 0x02 // single channel

    // channels
    for(int i=0;i<20;i++){
        mess2[12+i*2] = CHANNELCODE[list[i]-1]>>8;
        mess2[13+i*2] = CHANNELCODE[list[i]-1];
    }


    //mess2[58] [59] // single channel

    mess2[60] = 0x4f;
    mess2[61] = 0x56;
    mess2[62] = 0x45;
    mess2[63] = 0x52;

    mess.replace(79,64,mess2);

    // check byte as package end
    for(int i=3;i<1023;i++){
        mess[1023] = mess[i] ^ mess[1023];
    }

    sendMessage(mess);
}

QByteArray DataTrans::sendADSampleCommand(int len){
    emit sig_setLen(len);

    QByteArray mess(1024,0);
    // package head
    mess[0] = 0xFA;
    mess[1] = 0xF3;
    mess[2] = 0x20;

    mess[6] = 0x01;
    mess[7] = 0x7A;

    // check byte as package end
    for(int i=3;i<1023;i++){
        mess[1023] = mess[i] ^ mess[1023];
    }


    mutex.lock();
    sendMessage(mess);
    bool isAllReceieved = receievedAllData.wait(&mutex,10000); //time out 10000 ms
    mutex.unlock();

    QByteArray res(1024*len,0);

    if(isAllReceieved){
        for(int j=0;j<len*256;j++){
            res[j*4+0] = res[j*4+0] | threadTcpSocket->data[j]>>24;
            res[j*4+1] = res[j*4+1] | threadTcpSocket->data[j]>>16;
            res[j*4+2] = res[j*4+2] | threadTcpSocket->data[j]>>8;
            res[j*4+3] = res[j*4+3] | threadTcpSocket->data[j];
        }
    }else{
        //return an empty ByteArray when time out
        res.clear();
    }

    return res;

}

QByteArray DataTrans::sendDigitalCommand422(QByteArray src_mess,int boudRate,int stop,int check,int len, bool isOpenAD){
    emit sig_setLen(len);
    // generate 1K package
    QByteArray mess(1024,0);
    // package head
    mess[0] = 0xFA;
    mess[1] = 0xF3;
    mess[2] = 0x20;


    mess[3] = 1<<2 | 1<<4;

    mess[5] = 0x10;
    mess[6] = isOpenAD;
    mess[7] = 0x7A;

    boudRate = 40000000/(boudRate*2)-1;
    mess[56] = boudRate;
    mess[57] = boudRate>>8;
    mess[58] = stop | check<<3;

    int fileSize = src_mess.size();

    //rs422 data length
    mess[65] = mess[65] | fileSize; //low
    mess[66] = mess[66] | (fileSize>>8); //high

    // rs422 data
    mess.replace(207,fileSize,src_mess);

    mess[63] = 1<<(3-1);
    mess[64] = 1<<(3+4-1) | 1<<2;

    mess[63] = mess[63] | 1<<(1-1) | 1<<1;
    mess[64] = mess[64] | 1<<(1+4-1) | 1<<(2+4-1);

    // check byte as package end
    for(int i=3;i<1023;i++){
        mess[1023] = mess[i] ^ mess[1023];
    }

    mutex.lock();
    sendMessage(mess);
    bool isAllReceieved = receievedAllData.wait(&mutex,10000); //time out 10000 ms
    mutex.unlock();

    QByteArray res(1024*len,0);

    if(isAllReceieved){
        for(int j=0;j<len*256;j++){
            res[j*4+0] = res[j*4+0] | threadTcpSocket->data[j]>>24;
            res[j*4+1] = res[j*4+1] | threadTcpSocket->data[j]>>16;
            res[j*4+2] = res[j*4+2] | threadTcpSocket->data[j]>>8;
            res[j*4+3] = res[j*4+3] | threadTcpSocket->data[j];
        }
    }else{
        //return an empty ByteArray when time out
        res.clear();
    }

    return res;
}

void DataTrans::sendDigtalSet485(int channel_send,int channel_rev, QByteArray src_mess,int trigger){
    // generate 1K package
    QByteArray mess(1024,0);
    // package head
    mess[0] = 0xFA;
    mess[1] = 0xF3;
    mess[2] = 0x20;

    mess[3] = 1<<2 | 1<<4;

    mess[7] = 0x7A;

    mess[63] = trigger<<7;

    // data enable

    int fileSize = src_mess.size();

    mess[64] = 1<<(channel_send-1) | 1<<(channel_rev-1);

    //rs422 data length
    mess[65] = mess[65] | fileSize; //low
    mess[66] = mess[66] | (fileSize>>8); //high

    // rs422 data
    mess.replace(207,fileSize,src_mess);

    if(fileSize==1 && (src_mess[0]^0x2C)==0){
        mess[59] = 0x01;
    }

    // check byte as package end
    for(int i=3;i<1023;i++){
        mess[1023] = mess[i] ^ mess[1023];
    }

    sendMessage(mess);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));


    mess[3] = 1<<4;
    mess[5] = 0x10;

    mess[63] = 1<<(1-1) | 1<<1 ;
    mess[64] = 1<<(1+4-1) | 1<<(2+4-1);

    mess[1023]=0x00;
    // check byte as package end
    for(int i=3;i<1023;i++){
        mess[1023] = mess[i] ^ mess[1023];
    }

    sendMessage(mess);
}

QByteArray DataTrans::sendDigitalCommand485(int channel_send,int channel_rev,int len, bool isOpenAD){
    emit sig_setLen(len);
    // generate 1K package
    QByteArray mess(1024,0);
    // package head
    mess[0] = 0xFA;
    mess[1] = 0xF3;
    mess[2] = 0x20;


    mess[3] = 1<<2;

    mess[5] = 0x10;
    mess[6] = isOpenAD;
    mess[7] = 0x7A;


    mess[63] = 1<<(channel_send-1) | 1<<(channel_rev-1) | 1<<3;
    mess[64] = 1<<(channel_send+4-1) | 1<<(channel_rev+4-1);

    // check byte as package end
    for(int i=3;i<1023;i++){
        mess[1023] = mess[i] ^ mess[1023];
    }


    mutex.lock();
    sendMessage(mess);
    bool isAllReceieved = receievedAllData.wait(&mutex,10000); //time out 10000 ms
    mutex.unlock();

    QByteArray res(1024*len,0);

    if(isAllReceieved){
        for(int j=0;j<len*256;j++){
            res[j*4+0] = res[j*4+0] | threadTcpSocket->data[j]>>24;
            res[j*4+1] = res[j*4+1] | threadTcpSocket->data[j]>>16;
            res[j*4+2] = res[j*4+2] | threadTcpSocket->data[j]>>8;
            res[j*4+3] = res[j*4+3] | threadTcpSocket->data[j];
        }
    }else{
        //return an empty ByteArray when time out
        res.clear();
    }

    return res;
}

QByteArray DataTrans::sendImpulsiveCommand(int channel, int width, double height,int len,bool isOpenAD,bool rs422Enabled){
    emit sig_setLen(len);

    // generate 1K package
    QByteArray mess(1024,0);
    // package head
    mess[0] = 0xFA;
    mess[1] = 0xF3;
    mess[2] = 0x20;

    //mode : DA enabled
    mess[3] = 0x01;

    mess[5] = rs422Enabled<<5;
    mess[6] = isOpenAD;
    mess[7] = 0x7A;


    // get related parameters
    if(height-0<1e-4){
        height=0;
    }
    if(10-height<1e-4){
        height=10;
    }
    height = height/10.85*1023;

    // assign data
    // DA data mess[8]:low, mess[9]:high


    mess[8+(channel-1)*2] = mess[8+(channel-1)*2] | (unsigned int) height;
    mess[8+(channel-1)*2+1] = mess[8+(channel-1)*2+1] | ((unsigned int)height>>8);


//    for(int i=0;i<24;i++){
//        mess[8+(i)*2] = mess[8+(i)*2] | (unsigned int) height;
//        mess[8+(i)*2+1] = mess[8+(i)*2+1] | ((unsigned int)height>>8);
//    }


    mess[70] = width;
    mess[71] = width>>8;


    // check byte as package end
    for(int i=3;i<1023;i++){
        mess[1023] = mess[i] ^ mess[1023];
    }


    mutex.lock();
    sendMessage(mess);
    bool isAllReceieved = receievedAllData.wait(&mutex,10000); //time out 10000 ms
    mutex.unlock();

    QByteArray res(1024*len,0);

    if(isAllReceieved){
        for(int j=0;j<len*256;j++){
            res[j*4+0] = res[j*4+0] | threadTcpSocket->data[j]>>24;
            res[j*4+1] = res[j*4+1] | threadTcpSocket->data[j]>>16;
            res[j*4+2] = res[j*4+2] | threadTcpSocket->data[j]>>8;
            res[j*4+3] = res[j*4+3] | threadTcpSocket->data[j];
        }
    }else{
        //return an empty ByteArray when time out
        res.clear();
    }

    return res;
}

QByteArray DataTrans::sendSelfcheckCommand(bool state){
    QByteArray mess(1024,0);
    // package head
    mess[0] = 0xFA;
    mess[1] = 0xF3;
    mess[2] = 0x20;

    mess[3] = 1<<3;


    QByteArray mess2(64,0);
    // head
    mess2[0] = 0x43;
    mess2[1] = 0x4f;
    mess2[2] = 0x4d;
    mess2[3] = 0x44;

    // mode
    mess2[7] = 0x01;

    // channels
    for(int i=0;i<20;i++){
        if(state){
            mess2[12+i*2] = SELFCHECKCODE[i]>>8;
            mess2[13+i*2] = SELFCHECKCODE[i];
        }else{
            mess2[12+i*2] = SELFCHECKCODE[i+10]>>8;
            mess2[13+i*2] = SELFCHECKCODE[i+10];
        }
    }

    mess2[60] = 0x4f;
    mess2[61] = 0x56;
    mess2[62] = 0x45;
    mess2[63] = 0x52;

    mess.replace(79,64,mess2);



    mess[63] = 1<<6 | 1<<1 |1<<2 |1;
    mess[64] = 0x70;

    // check byte as package end
    for(int i=3;i<1023;i++){
        mess[1023] = mess[i] ^ mess[1023];
    }

    sendMessage(mess);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));


    emit sig_setLen(100);

    QByteArray mess3(1024,0);
    // package head
    mess3[0] = 0xFA;
    mess3[1] = 0xF3;
    mess3[2] = 0x20;

    mess3[3] = 1<<4;

    mess3[5] = 0x10;
    mess3[6] = 0x01;
    mess3[7] = 0x7A;


    int boudRate = 40000000/(115200*2)-1;
    mess3[56] = boudRate;
    mess3[57] = boudRate>>8;
    mess3[58] = 2 | 2<<3;



    mess3[63] = 1<<6 | 1<<1 |1<<2 |1;
    mess3[64] = 0x70;


    // check byte as package end
    for(int i=3;i<1023;i++){
        mess3[1023] = mess3[i] ^ mess3[1023];
    }

    mutex.lock();
    sendMessage(mess3);
    bool isAllReceieved = receievedAllData.wait(&mutex,10000); //time out 10000 ms
    mutex.unlock();

    QByteArray res(1024*100,0);

    if(isAllReceieved){
        for(int j=0;j<100*256;j++){
            res[j*4+0] = res[j*4+0] | threadTcpSocket->data[j]>>24;
            res[j*4+1] = res[j*4+1] | threadTcpSocket->data[j]>>16;
            res[j*4+2] = res[j*4+2] | threadTcpSocket->data[j]>>8;
            res[j*4+3] = res[j*4+3] | threadTcpSocket->data[j];
        }
    }else{
        //return an empty ByteArray when time out
        res.clear();
    }

    return res;

}

void DataTrans::sendResetCommand(){
    QByteArray mess(1024,0);
    // package head
    mess[0] = 0xFA;
    mess[1] = 0xF3;
    mess[2] = 0x20;

    mess[3] = 0x16;


    mess[6] = 0x10;
    mess[7] = 0x7A;

    mess[63] = 1<<5;

    // check byte as package end
    for(int i=3;i<1023;i++){
        mess[1023] = mess[i] ^ mess[1023];
    }

    sendMessage(mess);
}

void DataTrans::connectFailed(){
    TcpConnectionState = false;
    emit sig_connectionStateChanged(TcpConnectionState);
}

void DataTrans::connectSucceed(){
    TcpConnectionState = true;
    emit sig_connectionStateChanged(TcpConnectionState);
}

void DataTrans::disconnectSucceed(){
    TcpConnectionState = false;
    emit sig_connectionStateChanged(TcpConnectionState);
}

void DataTrans::sendMessage(QByteArray message)
{
    emit sig_sendMessage(message);
}

