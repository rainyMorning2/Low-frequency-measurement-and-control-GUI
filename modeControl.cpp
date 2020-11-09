#include "mainwindow.h"
#include "ui_mainwindow.h"

const int SELFCHECK = 0;
const int NORMAL = 1;
const int HIGHSPEED = 2;
const int RESET = 3;

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
            break;
        case SELFCHECK:
            printToConsole("hello1");
            break;
        case NORMAL:
            printToConsole("hello2");
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

}


void MainWindow::on_pushButton_save_clicked()
{

}

void MainWindow::on_pushButton_load_clicked()
{

}
