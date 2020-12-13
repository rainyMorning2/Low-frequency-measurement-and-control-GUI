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
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(QStringLiteral("保存到"));
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置文件过滤器
    fileDialog->setNameFilter(tr("File(*.dat)"));
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    fileDialog->setFileMode(QFileDialog::AnyFile);
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //设置默认拓展名
    fileDialog->setDefaultSuffix(".dat");
    fileDialog->selectFile("file");
    //打印所有选择的文件的路径
    QStringList fileNames;
    if (fileDialog->exec()) {
        fileNames = fileDialog->selectedFiles();
    }
    if(!fileNames.isEmpty()){
        QFile file(fileNames[0]);
        file.open(QIODevice::WriteOnly);
        QDataStream out(&file);   // we will serialize the data into the file
        out << analogData;
        printToConsole("成功保存到： "+fileNames[0]);
    }


}

void MainWindow::on_pushButton_load_clicked()
{
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(QStringLiteral("打开"));
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置文件过滤器
    fileDialog->setNameFilter(tr("File(*.dat)"));
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //打印所有选择的文件的路径
    QStringList fileNames;
    if (fileDialog->exec()) {
        fileNames = fileDialog->selectedFiles();
    }

    if(!fileNames.isEmpty()){
        QFile file(fileNames[0]);
        file.open(QIODevice::ReadOnly);
        QDataStream in(&file);    // read the data serialized from the file
        in >> analogData;
        printToConsole("已成功载入："+fileNames[0]);
    }
}
