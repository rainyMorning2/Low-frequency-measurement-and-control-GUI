#ifndef CUSTOMDATATRANS_H
#define CUSTOMDATATRANS_H

#include "datatrans.h"

class customDataTrans : public DataTrans
{
    Q_OBJECT
public:
    customDataTrans();

signals:
    void returnValue(QByteArray);

private slots:

    void connetTo_slot(QString,int);
    void adSample(int);
    void reset();
    void setChannel_slot(int*,int,bool);
    void setSingleChannel_slot(int);
    void digitalSet485(int channel_send, int channel_rev, QByteArray src_mess, int trigger);
    void digitalEnable422(QByteArray src_mess,int boudRate,int stop,int check,int len, bool isOpenAD);
    void digitalEnable485(int channel_send,int channel_rev, int len, bool isOpenAD);
    void impulse(int channel, int width, double height,int len,bool isOpenAD,bool rs422Enabled);
    void selfcheck(bool);



};

#endif // CUSTOMDATATRANS_H
