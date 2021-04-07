#ifndef THREADTCP_H
#define THREADTCP_H

#include <QThread>
#include <QTcpSocket>
#include <QDataStream>
#include <QTimer>

class ThreadTcp : public QTcpSocket{
    Q_OBJECT
public:
    ThreadTcp();
    ThreadTcp(int,bool);
    ~ThreadTcp();


signals:
    void sig_print(QString);
    void sig_connectSucceed();
    void sig_connectFailed();
    void sig_updateAnalogData(quint32*);
    void sig_realTimeRevChanged(bool);

private:
    QDataStream data_stream;
    QTimer* timer;

private slots:
    void connectTo(QString,int);
    void disconnectFrom();
    void send(QByteArray);
    void socket_Read_Data();
    void updateAnalogData();
};

#endif // THREADTCP_H
