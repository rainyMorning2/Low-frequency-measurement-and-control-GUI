#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QButtonGroup>

bool flag = false;
QButtonGroup* buttonGroup;
void MainWindow::modeCtrlInit(){

    buttonGroup = new QButtonGroup();
//    buttonGroup->addButton(ui->radioButton_high_speed_mode,HIGHSPEED);
//    buttonGroup->addButton(ui->radioButton_normal_mode,NORMAL);
    buttonGroup->addButton(ui->radioButton_selfcheck_mode,SELFCHECK);
    buttonGroup->addButton(ui->radioButton_reset_mode,RESET);
    buttonGroup->addButton(ui->radioButton_multi,MULTI);
    buttonGroup->addButton(ui->radioButton_single,SINGLE);

    ui->pushButton_start->setEnabled(false);
    ui->pushButton_stop->setEnabled(false);
    ui->comboBox->setEditable(true);
    ui->lineEdit_high_speed_channel->setEnabled(false);
    ui->radioButton_high_speed_mode->setEnabled(false);
    ui->radioButton_normal_mode->setEnabled(false);
}

void MainWindow::on_radioButton_high_speed_mode_toggled(bool checked)
{
    ui->lineEdit_high_speed_channel->setEnabled(checked);
}

void MainWindow::on_pushButton_start_clicked()
{
    QByteArray mess;
    // reset
    if(flag){
        sendMessage(QByteArray::fromHex("E9017A"));
        flag = !flag;
    }else{
        sendMessage(QByteArray::fromHex("E9817A"));
        flag = !flag;
    }
    resetDataIndex();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    switch(buttonGroup->checkedId()){
        case RESET:
            lastMode = currentMode;
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
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if(flag){
                sendMessage(QByteArray::fromHex("E96B7A"));
                flag = !flag;
            }else{
                sendMessage(QByteArray::fromHex("E9EB7A"));
                flag = !flag;
            }
            lastMode = currentMode;
            currentMode = SELFCHECK;
            break;
        case SINGLE:
            // TODO
            break;
        case MULTI:
            if(ui->radioButton_normal_mode->isChecked()){
                if(flag){
                    sendMessage(QByteArray::fromHex("E9737A"));
                    flag = !flag;
                }else{
                    sendMessage(QByteArray::fromHex("E9F37A"));
                    flag = !flag;
                }
                lastMode = currentMode;
                currentMode = NORMAL;
            }

            if(ui->radioButton_high_speed_mode->isChecked()){
                highSpeedChannel = ui->lineEdit_high_speed_channel->text().toInt();
                if(highSpeedChannel>=1 && highSpeedChannel<=200){
                    QByteArray mess = QByteArray::fromHex(settings->value(QString("highspeedCtrl/index%1").arg(highSpeedChannel)).toString().toLatin1());
                    if(flag){
                        sendMessage(mess);
                        flag = !flag;
                    }else{
                        mess[1]=mess[1]|0x80;
                        sendMessage(mess);
                        flag = !flag;
                    }
                    lastMode = currentMode;
                    currentMode = HIGHSPEED;
                    changeToHighspeedData();
                }else{
                    printToConsole("请输入合法的通道号(1~200)");
                }
            }

            break;
        default:
            printToConsole("请选择一个模式");
    }
    emit stateChange();
}

void MainWindow::on_pushButton_stop_clicked(){
    switch(currentMode){
        case SELFCHECK:
            if(flag){
                sendMessage(QByteArray::fromHex("E96D7A"));
                flag = !flag;
            }else{
                sendMessage(QByteArray::fromHex("E9ED7A"));
                flag = !flag;
            }
            lastMode = currentMode;
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
            lastMode = currentMode;
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
            lastMode = currentMode;
            currentMode = IDLE;
            break;
        default:
            printToConsole("当前是空闲状态！");
    }
    emit stateChange();
}


void MainWindow::on_pushButton_save_clicked()
{
    QString static filename;
    if(ui->pushButton_save->text()=="保存"){
        filename = ui->comboBox->currentText();
        if(!filename.isEmpty()){
            file.setFileName(filename);
        }else{
            on_toolButton_clicked();
            return;
        }

        file.open(QIODevice::WriteOnly);
        out.setDevice(&file);
        isSaveEnabled = true;
        ui->pushButton_save->setText("停止保存");
    }else{
        file.close();
        isSaveEnabled = false;
        ui->pushButton_save->setText("保存");
        printToConsole("成功保存到："+ filename);
    }
}

void MainWindow::on_pushButton_load_clicked()
{
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(QStringLiteral("打开"));
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
        quint32 **datas = new quint32*[file.size()/1024];
        resetDataIndex();
        for(int i=0;i<file.size()/1024;i++){
            datas[i] = new quint32[256];
            for(int j=0;j<256;j++){
                in>>datas[i][j];
            }
            parseData(datas[i]);
        }
        checkWarningState();
        setupScroolBar();
        printToConsole("已成功载入："+fileNames[0]);
    }
}

void MainWindow::on_toolButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(QStringLiteral("保存到"));
    fileDialog->setNameFilter(tr("File(*.dat)"));
    fileDialog->setFileMode(QFileDialog::AnyFile);
    fileDialog->setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (fileDialog->exec()) {
        fileNames = fileDialog->selectedFiles();
    }

    if(!fileNames.isEmpty()){
         if(ui->comboBox->findText(fileNames[0])==-1){
             ui->comboBox->addItem(fileNames[0]);
         }
         ui->comboBox->setCurrentIndex(ui->comboBox->findText(fileNames[0]));
    }

}

void MainWindow::pushButton_send_clicked()
{
    QFile tempFile(QCoreApplication::applicationDirPath() + "/DAdata.dat");
    tempFile.open(QIODevice::ReadOnly);
    QByteArray mess = tempFile.readAll();
    sendMessage(mess);
    tempFile.close();
}

void MainWindow::on_pushButton_set_clicked()
{
    settingWindow *v = new settingWindow();
    connect(v,SIGNAL(send_sig()),this,SLOT(pushButton_send_clicked()));
    v->show();
}

void MainWindow::on_radioButton_single_clicked()
{
    ui->lineEdit_high_speed_channel->setEnabled(false);
    ui->radioButton_high_speed_mode->setEnabled(false);
    ui->radioButton_normal_mode->setEnabled(false);
}

void MainWindow::on_radioButton_multi_clicked()
{
    ui->lineEdit_high_speed_channel->setEnabled(true);
    ui->radioButton_high_speed_mode->setEnabled(true);
    ui->radioButton_normal_mode->setEnabled(true);
    if(!ui->radioButton_high_speed_mode->isChecked() && !ui->radioButton_normal_mode->isChecked()){
        ui->radioButton_normal_mode->setChecked(true);
    }

}

void MainWindow::on_radioButton_selfcheck_mode_clicked()
{
    ui->lineEdit_high_speed_channel->setEnabled(false);
    ui->radioButton_high_speed_mode->setEnabled(false);
    ui->radioButton_normal_mode->setEnabled(false);
}

void MainWindow::on_radioButton_reset_mode_clicked()
{
    ui->lineEdit_high_speed_channel->setEnabled(false);
    ui->radioButton_high_speed_mode->setEnabled(false);
    ui->radioButton_normal_mode->setEnabled(false);
}
