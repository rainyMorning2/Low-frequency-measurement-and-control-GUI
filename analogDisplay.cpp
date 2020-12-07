#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::analogInit(){
    currentNum = 0;
    maxDisplay = settings->value("MaxDisplay").toInt();
    checkGroup = new QButtonGroup();

    for(int i=0;i<200;i++){
        analogData[i] = QVector<QPointF>();
    }

    checkGroup->setExclusive(false);

    for(int i=0;i<5;i++){
        tab_layouts.append(new QGridLayout());
        for(int j=0;j<40;j++){
            QString check_name;
            if(i==0){
                check_name = QString("高压通道 ").append(QString().setNum(i*40+j+1));
            }else{
                check_name = QString("低压通道 ").append(QString().setNum((i-1)*40+j+1));
            }
            QCheckBox* checkbox = new QCheckBox(check_name);
            tab_layouts[i]->addWidget(checkbox,j-20<0?j:j-20,j-20>=0?2:0);
            tab_layouts[i]->addWidget(new QLabel("avg: "),j-20<0?j:j-20,j-20>=0?3:1);
            checkGroup->addButton(checkbox,i*40+j+1);
        }
    }

    ui->tab_1->setLayout(tab_layouts[0]);
    ui->tab_2->setLayout(tab_layouts[1]);
    ui->tab_3->setLayout(tab_layouts[2]);
    ui->tab_4->setLayout(tab_layouts[3]);
    ui->tab_5->setLayout(tab_layouts[4]);

    connect(checkGroup,SIGNAL(idToggled(int,bool)),this,SLOT(check(int,bool)));
}


void MainWindow::check(int id, bool checked)
{


    if(checked){
        if(currentNum < maxDisplay){
            // add a new chart
            QChartView* qcv;
            qcv = addNewChart(id);
            qcv->setObjectName(QString::number(id));
            ui->verticalLayout->addWidget(qcv);
            ui->verticalLayout->setStretchFactor(qcv,1);
            currentNum++;

        }
    }else{
        QChartView* view = this->findChild<QChartView *>(QString::number(id));
        ui->verticalLayout->removeWidget(view);
        view->deleteLater();
        currentNum--;
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


QChartView* MainWindow::addNewChart(int id){

    QString title;
    int xRange = settings->value("xRange").toInt();;
    int yRange;
    if(id <= 40){
        title = QString("高压通道 ").append(QString::number(id));
        yRange = 200;
    }else{
        title = QString("低压通道 ").append(QString::number(id-40));
        yRange = 100;
    }

    QChartView* mChart = new QChartView();
    mChart->setEnabled(true);
    mChart->chart()->setTitle(title);
    mChart->chart()->legend()->hide();
    mChart->chart()->setTheme(QtCharts::QChart::ChartTheme::ChartThemeDark);

    QValueAxis* vaX = new QValueAxis();
    QValueAxis* vaY1 = new QValueAxis();
    QSplineSeries* spY1 = new QSplineSeries();

    vaX->setRange(0, xRange);
    vaX->setTickCount(7);
    vaX->setLabelsColor(QColor(0, 255, 0));
    vaX->setTitleText("Sample-Points");

    vaY1->setRange(0, yRange);
    vaY1->setTickCount(6);
    vaY1->setLabelsColor(QColor(255,0, 0));
    vaY1->setTitleText("Voltage-Kv");

    mChart->chart()->addAxis(vaX, Qt::AlignBottom);
    mChart->chart()->addAxis(vaY1, Qt::AlignLeft);
    mChart->chart()->addSeries(spY1);

    spY1->attachAxis(vaX);
    spY1->attachAxis(vaY1);
    spY1->setColor(QColor(255, 0, 0));
    spY1->setUseOpenGL();// 开启openGL加速

    return mChart;

}




