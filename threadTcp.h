#ifndef THREADTCP_H
#define THREADTCP_H

#include <QThread>
#include <QTcpSocket>
#include <QDataStream>
#include <QTimer>
#include <QWaitCondition>
#include <QMutex>
#include <QFile>
#include <QCoreApplication>

extern QMutex mutex;
extern QWaitCondition receievedAllData;


class ThreadTcp : public QTcpSocket{
    Q_OBJECT
public:
    ThreadTcp();
    ~ThreadTcp();
    quint32* data;
    int len;

signals:
    void sig_connectSucceed();
    void sig_connectFailed();

private:
    QDataStream data_stream;
    QByteArray stop;

private slots:
    void connectTo(QString,int);
    void disconnectFrom();
    void send(QByteArray);
    void socket_Read_Data();
    void setLen(int);
};

#endif // THREADTCP_H
