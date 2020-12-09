#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chrono"
#include "thread"

const int SELFCHECK = 0;
const int NORMAL = 1;
const int HIGHSPEED = 2;
const int RESET = 3;


/*
    //TODO
    1.finish save and load including auto save
    2.finish control conmand
    3. print2consle
*/

void MainWindow::modeCtrlInit(){

    buttonGroup = new QButtonGroup();
    buttonGroup->addButton(ui->radioButton_high_speed_mode,HIGHSPEED);
    buttonGroup->addButton(ui->radioButton_normal_mode,NORMAL);
    buttonGroup->addButton(ui->radioButton_selfcheck_mode,SELFCHECK);
    buttonGroup->addButton(ui->radioButton_reset_mode,RESET);

    ui->lineEdit_high_speed_channel->setEnabled(false);
    ui->pushButton_start->setEnabled(false);
    ui->pushButton_stop->setEnabled(false);
}


void MainWindow::on_radioButton_high_speed_mode_toggled(bool checked)
{
    ui->lineEdit_high_speed_channel->setEnabled(checked);
}

void MainWindow::on_pushButton_start_clicked()
{
    QByteArray mess;
    switch(buttonGroup->checkedId()){
        case RESET:
//            mess = (QByteArray::fromHex("7A01E9"));
            mess = (QByteArray::fromHex("E9017A"));
            sendMessage(mess);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
//            mess = (QByteArray::fromHex("7A21E9"));
            mess = (QByteArray::fromHex("E9217A"));
//            sendMessage(mess);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
//            mess = (QByteArray::fromHex("7A41E9"));
            mess = (QByteArray::fromHex("E9417A"));
//            sendMessage(mess);
            break;
        case SELFCHECK:
            break;
        case NORMAL:
//            mess = (QByteArray::fromHex("7A73E9"));
            mess = (QByteArray::fromHex("E9737A"));
            sendMessage(mess);
            std::this_thread::sleep_for(std::chrono::nanoseconds(200));
//            mess = (QByteArray::fromHex("7A33E9"));
            mess = (QByteArray::fromHex("E9337A"));
//            sendMessage(mess);
            std::this_thread::sleep_for(std::chrono::nanoseconds(200));
//            mess = (QByteArray::fromHex("7A53E9"));
            mess = (QByteArray::fromHex("E9537A"));
//            sendMessage(mess);
            break;
        case HIGHSPEED:
            if (ui->lineEdit_high_speed_channel->text()==""){
                qDebug()<<"wrong !";
            }else{
                qDebug()<<"yes !";
            }
            break;
        default:
            qDebug()<<"nothing !";
    }
}

void MainWindow::on_pushButton_stop_clicked()
{
    QByteArray mess;
//    mess = (QByteArray::fromHex("7A75E9"));
    mess = (QByteArray::fromHex("E9757A"));
    sendMessage(mess);
    std::this_thread::sleep_for(std::chrono::nanoseconds(200));
//    mess = (QByteArray::fromHex("7A35E9"));
    mess = (QByteArray::fromHex("E9357A"));
//    sendMessage(mess);
    std::this_thread::sleep_for(std::chrono::nanoseconds(200));
//    mess = (QByteArray::fromHex("7A55E9"));
    mess = (QByteArray::fromHex("E9557A"));
//    sendMessage(mess);
}


void MainWindow::on_pushButton_save_clicked()
{
    QFile file("file.dat");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);   // we will serialize the data into the file
    out << QString("the answer is");   // serialize a string
    out << (qint32)42;        // serialize an integer
    analogData[0].append(QPointF(2,3));
    out << analogData;

}

void MainWindow::on_pushButton_load_clicked()
{
    QFile file("file.dat");
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);    // read the data serialized from the file
    QString str;
    qint32 a;
    QList<QVector<QPointF>> s;
    in >> str >> a >> s;
    qDebug()<<str;
    qDebug()<<a;
    qDebug()<<s;


}
