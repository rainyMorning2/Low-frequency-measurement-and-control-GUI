#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::stateInit(){
    isConnected = false;
    isReset = false;
    isSelfchecked = false;
    currentMode = IDLE;
    lastMode = IDLE;
    connect(this, SIGNAL(stateChange()), this, SLOT(updateState()));
    updateState();
}

void MainWindow::updateState(){
    QString mode;
    switch (currentMode) {
        case IDLE:
            mode = "空闲";
            break;
        case NORMAL:
            mode = "普通采样模式";
            break;
        case RESET:
            mode = "复位模式";
            break;
        case SELFCHECK:
            mode = "自检模式";
            break;
        case HIGHSPEED:
            mode = "高速采样模式";
            break;
    }
    QString str = QString("网络状态： %1 \n复位状态： %2 \n自检状态： %3 \n当前工作模式： %4 \n待处理帧数： %5").arg(isConnected?"已连接":"未连接",isReset?"已复位":"未复位",isSelfchecked?"已自检":"未自检",mode,QString::number(threadSocket->bytesAvailable()/1024));
    ui->label_state->setText(str);

}
