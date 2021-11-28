#include "customdatatrans.h"

customDataTrans::customDataTrans()
{

}

void customDataTrans::adSample(int len){
    auto a = sendADSampleCommand(len);
    emit returnValue(a);
}

void customDataTrans::connetTo_slot(QString ip,int port){
    connetTo(ip,port);
}

void customDataTrans::reset(){
    sendResetCommand();
}

void customDataTrans::setChannel_slot(int* channels,int size, bool same){
    setChannel(channels,size,same);
}

void customDataTrans::setSingleChannel_slot(int channel){
    setSingleChannel(channel);
}

void customDataTrans::digitalEnable422(QByteArray src_mess,int boudRate,int stop,int check,int len, bool isOpenAD){
    auto a = sendDigitalCommand422(src_mess, boudRate, stop, check, len,  isOpenAD);
    emit returnValue(a);
}

void customDataTrans::digitalSet485(int channel_send, int channel_rev, QByteArray src_mess, int trigger){
    sendDigtalSet485(channel_send, channel_rev, src_mess, trigger);
}

void customDataTrans::digitalEnable485(int channel_send,int channel_rev, int len, bool isOpenAD){
    auto a = sendDigitalCommand485(channel_send, channel_rev, len, isOpenAD);
    emit returnValue(a);
}

void customDataTrans::impulse(int channel, int width, double height,int len,bool isOpenAD,bool rs422Enabled){
    auto a = sendImpulsiveCommand(channel,width,height,len,isOpenAD,rs422Enabled);
    emit returnValue(a);
}

void customDataTrans::selfcheck(bool state){
    auto a = sendSelfcheckCommand(state);
    emit returnValue(a);
}

