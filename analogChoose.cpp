#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::analogInit(){
    currentNum = 0;
    maxDisplay = settings->value("MaxDisplay").toInt();
    connect(ui->checkGroup,SIGNAL(idToggled(int,bool)),this,SLOT(check(int,bool)));
    int id = 1;
    foreach(QAbstractButton* button, ui->checkGroup->buttons()){
        ui->checkGroup->setId(button,id);
        id++;
    }
}


void MainWindow::check(int id, bool checked)
{

    if(checked){
        if(currentNum < maxDisplay){
            // add a new chart
            QChartView* qcv;
            qcv = new QChartView();
            qcv->setObjectName(QString::number(id));
            ui->verticalLayout->addWidget(qcv);
            qDebug()<<id<<" checked";
            currentNum++;
        }
    }else{
        QList<QChartView*> views = this->findChildren<QChartView *>();
        foreach(QChartView* view, views){
            if(view->objectName()==QString::number(id)){
                ui->verticalLayout->removeWidget(view);
                view->deleteLater();
                qDebug()<<id<<" deleted "<<view->objectName();
                currentNum--;
            }
        }
    }


    static bool isAllEnabled = true;
    if(currentNum >= maxDisplay){
        foreach(QAbstractButton* button, ui->checkGroup->buttons()){
            if(!button->isChecked()){
                button->setEnabled(false);
            }
        }
        isAllEnabled = false;
    }else{
        // maybe can add a flag to optimize when the group size is big.
        // that is it.
        if(!isAllEnabled){
            foreach(QAbstractButton* button, ui->checkGroup->buttons()){
                button->setEnabled(true);
            }
            isAllEnabled = true;
        }
    }

}
