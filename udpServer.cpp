#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUdpSocket>
#include <QNetworkDatagram>

QUdpSocket* udpSocket;

void MainWindow::udpInit(){
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::LocalHost, 7755);

    connect(udpSocket, SIGNAL(QUdpSocket::readyRead()),this, SLOT(readPendingDatagrams()));
}

void MainWindow::readPendingDatagrams(){

    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(int(udpSocket->pendingDatagramSize()));
        udpSocket->readDatagram(datagram.data(), datagram.size());
        if(datagram == ""){ // match the magic code
            on_pushButton_save_clicked();
        }
    }
}
