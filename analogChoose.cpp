#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QCheckBox"

void MainWindow::analogInit(){
    currentNum = 0;
    maxDisplay = settings->value("MaxDisplay").toInt();
    checkGroup = new QButtonGroup();
    QList<QGridLayout*> layouts;

    checkGroup->setExclusive(false);

    for(int i=0;i<5;i++){
        layouts.append(new QGridLayout());
        for(int j=0;j<40;j++){
            QCheckBox* checkbox = new QCheckBox();
            layouts[i]->addWidget(checkbox,j-20<0?j:j-20,j-20>=0?1:0);
            checkGroup->addButton(checkbox,i*40+j+1);
        }
    }

    ui->tab_1->setLayout(layouts[0]);
    ui->tab_2->setLayout(layouts[1]);
    ui->tab_3->setLayout(layouts[2]);
    ui->tab_4->setLayout(layouts[3]);
    ui->tab_5->setLayout(layouts[4]);

    connect(checkGroup,SIGNAL(idToggled(int,bool)),this,SLOT(check(int,bool)));
}


void MainWindow::check(int id, bool checked)
{


    if(checked){
        if(currentNum < maxDisplay){
            // add a new chart
            QChartView* qcv;
            qcv = addNewChart(QString::number(id));
            qcv->setObjectName(QString::number(id));
            ui->verticalLayout->addWidget(qcv);
            ui->verticalLayout->setStretchFactor(qcv,1);
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
        foreach(QAbstractButton* button, checkGroup->buttons()){
            if(!button->isChecked()){
                button->setEnabled(false);
            }
        }
        isAllEnabled = false;
    }else{
        // maybe can add a flag to optimize when the group size is big.
        // that is it.
        if(!isAllEnabled){
            foreach(QAbstractButton* button, checkGroup->buttons()){
                button->setEnabled(true);
            }
            isAllEnabled = true;
        }
    }

}
