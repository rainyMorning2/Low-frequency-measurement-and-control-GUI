#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUdpSocket>

QUdpSocket* udpSocket;
QByteArray udpCode;

void MainWindow::udpInit(){
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::LocalHost, settings->value("UDPServer/udpPort").toInt());
    udpCode = QByteArray::fromHex(settings->value("UDPServer/magicCode").toString().toLatin1());
    connect(udpSocket, SIGNAL(QUdpSocket::readyRead()),this, SLOT(readPendingDatagrams()));
}

void MainWindow::readPendingDatagrams(){

    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(int(udpSocket->pendingDatagramSize()));
        udpSocket->readDatagram(datagram.data(), datagram.size());
        if(datagram == udpCode){ // match the magic code
            on_pushButton_save_clicked();
        }
    }
}
