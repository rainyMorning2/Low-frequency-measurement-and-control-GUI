#ifndef DATATRANS_H
#define DATATRANS_H

#include "threadTcp.h"
#include <QWaitCondition>
#include <QMutex>

extern QMutex mutex;
extern QWaitCondition receievedAllData;


class DataTrans: public QObject{
    Q_OBJECT
public:
    DataTrans();
    bool TcpConnectionState;
    void connetTo(QString ip, int port);
    void sendResetCommand();
    void setSingleChannel(int channel);
    void setChannel(int* channels,int size,bool same);
    QByteArray sendSelfcheckCommand(bool);
    QByteArray sendADSampleCommand(int len);
    QByteArray sendDigitalCommand422(QByteArray src_mess,int boudRate,int stop,int check,int len, bool isOpenAD);
    void sendDigtalSet485(int channel_send, int channel_rev, QByteArray src_mess, int trigger);
    QByteArray sendDigitalCommand485(int channel_send,int channel_rev, int len, bool isOpenAD);
    QByteArray sendImpulsiveCommand(int channel, int width, double height,int len,bool isOpenAD,bool rs422Enabled);


signals:
    void sig_connect(QString ip,int port);
    void sig_disconnect();
    void sig_sendMessage(QByteArray);
    void sig_connectionStateChanged(bool);
    void sig_setLen(int);

private slots:
    void connectSucceed();
    void connectFailed();
    void disconnectSucceed();

private:
    ThreadTcp* threadTcpSocket;
    QThread* tcpWorkThread;
    void sendMessage(QByteArray message);

};

#endif // DATATRANS_H
