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

    switch(buttonGroup->checkedId()){
        case RESET:
//            QString mess;
//            sendMessage(mess);
            break;
        case SELFCHECK:
            break;
        case NORMAL:
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
    std::this_thread::sleep_for(std::chrono::nanoseconds(2));
}


void MainWindow::on_pushButton_save_clicked()
{
    QSaveFile file(QCoreApplication::applicationDirPath()+"/data.txt");
    file.open(QIODevice::WriteOnly);
    for (int i=0;i<200 ;i++ ) {
        for(int j=0;j<analogData[i].size();j++){
//            file.write(analogData[i][j]);
        }

    }

    file.commit();
}

void MainWindow::on_pushButton_load_clicked()
{

}
