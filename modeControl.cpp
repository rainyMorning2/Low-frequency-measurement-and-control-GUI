#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chrono"
#include "thread"
#include <QFileDialog>

bool flag = false;

/*
    //TODO
    1.finish save and load
*/

void MainWindow::modeCtrlInit(){

    buttonGroup = new QButtonGroup();
    buttonGroup->addButton(ui->radioButton_high_speed_mode,HIGHSPEED);
    buttonGroup->addButton(ui->radioButton_normal_mode,NORMAL);
    buttonGroup->addButton(ui->radioButton_selfcheck_mode,SELFCHECK);
    buttonGroup->addButton(ui->radioButton_reset_mode,RESET);
    buttonGroup->addButton(ui->radioButton_selfcheck_en,SELFCHECK_EN);

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
            if(flag){
                sendMessage(QByteArray::fromHex("E9017A"));
                flag = !flag;
            }else{
                sendMessage(QByteArray::fromHex("E9817A"));
                flag = !flag;
            }
            currentMode = RESET;
            isReset = true;
            break;
        case SELFCHECK:
            if(flag){
                sendMessage(QByteArray::fromHex("E9697A"));
                flag = !flag;
            }else{
                sendMessage(QByteArray::fromHex("E9E97A"));
                flag = !flag;
            }
            currentMode = SELFCHECK;
            break;
        case SELFCHECK_EN:
            if(flag){
                sendMessage(QByteArray::fromHex("E96B7A"));
                flag = !flag;
            }else{
                sendMessage(QByteArray::fromHex("E9EB7A"));
                flag = !flag;
            }
            currentMode = SELFCHECK_EN;
            isSelfchecked = true;
            break;
        case NORMAL:
            if(flag){
                sendMessage(QByteArray::fromHex("E9737A"));
                flag = !flag;
            }else{
                sendMessage(QByteArray::fromHex("E9F37A"));
                flag = !flag;
            }
            currentMode = NORMAL;
            break;
        case HIGHSPEED:
        {
            QByteArray channel = QByteArray::fromHex("00");
            channel[0] = ui->lineEdit_high_speed_channel->text().toInt(nullptr,16);
            if(true){ // whether a legal input
                if(flag){
                    sendMessage(channel + QByteArray::fromHex("7A7A"));
                    flag = !flag;
                }else{
                    sendMessage(channel + QByteArray::fromHex("FA7A"));
                    flag = !flag;
                }
            }else{
                printToConsole("请输入合法的通道号");
            }
            currentMode = HIGHSPEED;
            break;
        }
        default:
            printToConsole("请选择一个模式");
    }
    emit stateChange();
}

void MainWindow::on_pushButton_stop_clicked(){
    switch(currentMode){
        case SELFCHECK_EN:
            if(flag){
                sendMessage(QByteArray::fromHex("E96D7A"));
                flag = !flag;
            }else{
                sendMessage(QByteArray::fromHex("E9ED7A"));
                flag = !flag;
            }
            currentMode = IDLE;
            break;
        case NORMAL:
            if(flag){
                sendMessage(QByteArray::fromHex("E9757A"));
                flag = !flag;
            }else{
                sendMessage(QByteArray::fromHex("E9F57A"));
                flag = !flag;
            }
            currentMode = IDLE;
            break;
        case HIGHSPEED:
            if(flag){
                sendMessage(QByteArray::fromHex("E97D7A"));
                flag = !flag;
            }else{
                sendMessage(QByteArray::fromHex("E9FD7A"));
                flag = !flag;
            }
            currentMode = IDLE;
            break;
        default:
            printToConsole("请选择一个模式");
    }
    emit stateChange();
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
