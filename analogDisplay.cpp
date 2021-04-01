#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QtCharts/QValueAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QScatterSeries>
#include <QSplitter>
#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>

QStringList checkboxNames;
QStringList tabNames;
CustomTabWidget *ctb;
QGridLayout *vlayout;
QVector<QChartView*> inDisplay;
QTimer chartUpdate;
QList<QGridLayout*> tab_layouts;
QButtonGroup* checkGroup;

int timescale;
double highTimescale;
int maxDisplay;
double *warningLimitsLow;
double *warningLimitsHigh;
int *warningCnt;
bool isDebug;
int debugCnt;
bool packageEnable;
bool normalEnable;
bool highspeedEnable;
bool valueLabelEnable;
bool rs422Enable;

double *regressionK;
double *regressionB;

const int IN2OUTLUT[200] = {10,	9,	26,	8,	27,	28,	29,	30,	12,	11,	13,	20,	2,	19,	1,	3,	21,	4,	24,	23,	22,	5,	6,	7,	25,	37,	38,	39,	40,	36,	35,	18,	15,	34,	33,	17,	16,	32,	31,	14,	50,	49,	66,	48,	67,	68,	69,	70,	52,	51,	53,	60,	42,	59,	41,	43,	61,	44,	64,	63,	62,	45,	46,	47,	65,	77,	78,	79,	80,	76,	75,	58,	55,	74,	73,	57,	56,	72,	71,	54,	90,	89,	106,	88,	107,	108,	109,	110,	92,	91,	93,	100,	82,	99,	81,	83,	101,	84,	104,	103,	102,	85,	86,	87,	105,	117,	118,	119,	120,	116,	115,	98,	95,	114,	113,	97,	96,	112,	111,	94,	130,	129,	146,	128,	147,	148,	149,	150,	132,	131,	133,	140,	122,	139,	121,	123,	141,	124,	144,	143,	142,	125,	126,	127,	145,	157,	158,	159,	160,	156,	155,	138,	135,	154,	153,	137,	136,	152,	151,	134,	170,	169,	186,	168,	187,	188,	189,	190,	172,	171,	173,	180,	162,	179,	161,	163,	181,	164,	184,	183,	182,	165,	166,	167,	185,	197,	198,	199,	200,	196,	195,	178,	175,	194,	193,	177,	176,	192,	191,	174};
const int OUT2INLUT[201] = {-1, 14,	12,	15,	17,	21,	22,	23,	3,	1,	0,	9,	8,	10,	39,	32,	36,	35,	31,	13,	11,	16,	20,	19,	18,	24,	2,	4,	5,	6,	7,	38,	37,	34,	33,	30,	29,	25,	26,	27,	28,	54,	52,	55,	57,	61,	62,	63,	43,	41,	40,	49,	48,	50,	79,	72,	76,	75,	71,	53,	51,	56,	60,	59,	58,	64,	42,	44,	45,	46,	47,	78,	77,	74,	73,	70,	69,	65,	66,	67,	68,	94,	92,	95,	97,	101,	102,	103,	83,	81,	80,	89,	88,	90,	119,	112,	116,	115,	111,	93,	91,	96,	100,	99,	98,	104,	82,	84,	85,	86,	87,	118,	117,	114,	113,	110,	109,	105,	106,	107,	108,	134,	132,	135,	137,	141,	142,	143,	123,	121,	120,	129,	128,	130,	159,	152,	156,	155,	151,	133,	131,	136,	140,	139,	138,	144,	122,	124,	125,	126,	127,	158,	157,	154,	153,	150,	149,	145,	146,	147,	148,	174,	172,	175,	177,	181,	182,	183,	163,	161,	160,	169,	168,	170,	199,	192,	196,	195,	191,	173,	171,	176,	180,	179,	178,	184,	162,	164,	165,	166,	167,	198,	197,	194,	193,	190,	189,	185,	186,	187,	188};

void MainWindow::analogInit(){
    maxDisplay = 4;
    normalTimeIndex = 16*60*60*1000;
    highTimeIndex = 16*60*60*1000;
    isSaveEnabled = false;

    warningCnt = new int[5];
    isOverflow = new bool[200];
    warningLimitsLow = new double[200];
    warningLimitsHigh = new double[200];
    regressionB = new double[200];
    regressionK = new double[200];

    memset(warningCnt,0,5*sizeof(int));
    memset(isOverflow,0,200*sizeof(bool));

    isDebug = settings->value("DebugMode/isDebug").toBool();
    debugCnt = settings->value("DebugMode/debugCnt").toInt();
    packageEnable = settings->value("DebugMode/packageEnable").toBool();
    normalEnable = settings->value("DebugMode/normalEnable").toBool();
    highspeedEnable =settings->value("DebugMode/highspeedEnable").toBool();
    valueLabelEnable = settings->value("DebugMode/valueLabelEnable").toBool();
    rs422Enable = settings->value("DebugMode/rs422Enable").toBool();

    xRange = settings->value("xRange").toInt();
    QVariant defaultLimit = settings->value("WarningLimit/default");
    QVariant defaultRes = settings->value("Regression/default");
    checkboxNames = settings->value("checkboxNames").toStringList();
    tabNames = settings->value("tabNames").toStringList();
    timescale = settings->value("timescale").toInt();
    highTimescale = timescale/32.0;

    begin.setMSecsSinceEpoch(0);
    begin = begin.addSecs(16*60*60);
    end.setMSecsSinceEpoch(xRange);
    end = end.addSecs(16*60*60);

    checkGroup = new QButtonGroup();
    ctb= new CustomTabWidget(this,5);
    QWidget *vw = new QWidget();
    vlayout = new QGridLayout(vw);
    vlayout->setColumnStretch(0,1);
    vlayout->setRowStretch(0,1);

    for(int i=0;i<5;i++){
        tab_layouts.append(new QGridLayout());
        for(int j=0;j<40;j++){
            analogData.append(QVector<QPointF>());
            QList<QVariant> limits = settings->value(QString("WarningLimit/index%1").arg(i*40+j+1), defaultLimit).toList();
            QList<QVariant> regression = settings->value(QString("Regression/index%1").arg(i*40+j+1), defaultRes).toList();
            warningLimitsLow[OUT2INLUT[i*40+j+1]] = limits[0].toDouble();
            warningLimitsHigh[OUT2INLUT[i*40+j+1]] = limits[1].toDouble();
            regressionK[OUT2INLUT[i*40+j+1]] = regression[0].toDouble();
            regressionB[OUT2INLUT[i*40+j+1]] = regression[1].toDouble();
            QCheckBox* checkbox = new QCheckBox(checkboxNames[i*40+j]);
            if(isDebug && valueLabelEnable){
                tab_layouts[i]->addWidget(checkbox,j-20<0?j:j-20,j-20>=0?2:0);
                tab_layouts[i]->addWidget(new QLabel("value: "),j-20<0?j:j-20,j-20>=0?3:1);
            }else{
                tab_layouts[i]->addWidget(checkbox,j-20<0?j:j-20,j-20>=0?1:0);
            }
            checkGroup->addButton(checkbox,i*40+j);
        }

        QWidget *qw = new QWidget();
        qw->setLayout(tab_layouts[i]);
        ctb->addTab(qw,tabNames[i]);
    }

    chartUpdate.setInterval(settings->value("refreshRate").toInt());
    chartUpdate.start();

    checkGroup->setExclusive(false);


    QSplitter * VSplitter = new QSplitter(Qt::Vertical);
    QSplitter * HSplitter = new QSplitter(Qt::Horizontal);//新建水平分割器

    HSplitter->setHandleWidth(1);//分割线的宽度
    HSplitter->setChildrenCollapsible(false);//不允许把分割出的子窗口拖小到0，最小值被限定为sizeHint或maxSize/minSize
    VSplitter->setChildrenCollapsible(false);//不允许把分割出的子窗口拖小到0，最小值被限定为sizeHint或maxSize/minSize

    HSplitter->insertWidget(0,ctb);
    HSplitter->insertWidget(1,vw);
    QList<int> temp = {256,768};
    HSplitter->setSizes(temp);

    VSplitter->insertWidget(0,HSplitter);
    VSplitter->insertWidget(1,ui->groupBox_3);
    VSplitter->setSizes(temp);
    ui->verticalLayout->addWidget(VSplitter);

    connect(checkGroup,SIGNAL(idToggled(int,bool)),this,SLOT(check(int,bool)));
    connect(&chartUpdate,SIGNAL(timeout()),this,SLOT(refreshChart()));
}

void MainWindow::resortCharts(bool isDel){

    if(isDel){
        for(int i=0;i<inDisplay.size();i++){
            vlayout->removeWidget(inDisplay[i]);
        }
    }

    switch (inDisplay.size()) {
        case 4:
            vlayout->addWidget(inDisplay[3],1,1);
        case 3:
            vlayout->addWidget(inDisplay[2],1,0);
            vlayout->setRowStretch(1,1);
        case 2:
            vlayout->addWidget(inDisplay[1],0,1);
            vlayout->setColumnStretch(1,1);
        case 1:
            vlayout->addWidget(inDisplay[0],0,0);
    }

    if(inDisplay.size()<3){
        vlayout->setRowStretch(1,0);
    }
    if(inDisplay.size()<2){
        vlayout->setColumnStretch(1,0);
    }

}

void MainWindow::check(int id, bool checked)
{
    if(checked){
        if(inDisplay.size() < maxDisplay){
            // add a new chart
            QChartView* qcv = addNewChart(id);
            inDisplay.append(qcv);
            resortCharts(false);
        }
    }else{
        for(int i=0;i<inDisplay.size();i++){
            if(inDisplay[i]->objectName()==QString::number(id)){
                vlayout->removeWidget(inDisplay[i]);
                inDisplay[i]->deleteLater();
                inDisplay.removeAt(i);
                resortCharts(true);
                break;
            }
        }
    }

    static bool isAllEnabled = true;
    if(inDisplay.size() >= maxDisplay){
        foreach(QAbstractButton* button, checkGroup->buttons()){
            if(!button->isChecked()){
                button->setEnabled(false);
            }
        }
        isAllEnabled = false;
    }else{
        if(!isAllEnabled){
            foreach(QAbstractButton* button, checkGroup->buttons()){
                button->setEnabled(true);
            }
            isAllEnabled = true;
        }
    }

}


QChartView* MainWindow::addNewChart(int id){

    QString title = checkboxNames[id];
    int yRange;
    if((id%40>=13 && id%40<=16) || (id%40>=30&&id%40<=33)){
        yRange = 60;
    }else{
        yRange = 5;
    }

    QChartView* mChart = new QChartView();
    mChart->setEnabled(true);
    mChart->chart()->setTitle(title);
    mChart->chart()->legend()->hide();
    mChart->chart()->setTheme(QtCharts::QChart::ChartTheme::ChartThemeDark);
    mChart->setObjectName(QString::number(id));

    QDateTimeAxis* vaX = new QDateTimeAxis();
    QValueAxis* vaY1 = new QValueAxis();
    QSplineSeries* spY1 = new QSplineSeries();

    vaX->setFormat("hh:mm:ss.zzz");
    vaX->setRange(begin,end);
    vaX->setTickCount(5);
    vaX->setLabelsColor(QColor(0, 255, 0));
    vaX->setTitleText("Sample-Time");

    vaY1->setRange(0, yRange);
    vaY1->setTickCount(6);
    vaY1->setLabelsColor(QColor(255,0, 0));
    vaY1->setTitleText("Voltage-V");

    mChart->chart()->addAxis(vaX, Qt::AlignBottom);
    mChart->chart()->addAxis(vaY1, Qt::AlignLeft);
    mChart->chart()->addSeries(spY1);

    spY1->attachAxis(vaX);
    spY1->attachAxis(vaY1);
    spY1->setColor(QColor(255, 0, 0));
    spY1->setUseOpenGL(true);// 开启openGL加速

    spY1->replace(analogData[OUT2INLUT[id+1]]);
    mChart->setRubberBand(QChartView::HorizontalRubberBand);
    return mChart;

}

void MainWindow::refreshAnalogData(quint32* data){

    static int cnt = 1;
    QString tempData="\n";

    // parse data into rs422 and analogData display
    parseData(data);
    //  warning
    //checkWarningState();

    if(isDebug && cnt==debugCnt){

        if(packageEnable){
            tempData += "package:";
            for(int i=0;i<256;i++){
                if(i%8==0){
                    tempData += "\n";
                }
                tempData += QString("%1").arg(data[i],8,16,QLatin1Char('0'));
                tempData+=" ";
            }
            tempData+="\n";
        }

        if(normalEnable){
            tempData += "normal:";
            for(int i=0;i<200;i++){
                if(i%10==0){
                    tempData +="\n";
                }
                tempData += QString::number(analogData[i].last().y());
                tempData += " ";
            }
            tempData += "\n";
        }

        if(highspeedEnable && (currentMode == HIGHSPEED || (lastMode== HIGHSPEED && currentMode==IDLE ))){
            tempData += "highspeed:";
            for(int i=0;i<32;i++){
                if(i%8==0){
                    tempData+="\n";
                }
                tempData += QString::number(highSpeedData[highSpeedData.size()-32+i].y());
                tempData += " ";
            }
        }


        if(rs422Enable){
            tempData += "rs422_1:";
            for(int i=0;i<256;i++){
                if(i%8==0){
                    tempData +="\n";
                }
                tempData += QString("%1").arg(rs422_1_data[rs422_1_data.size()-256+i],2,16,QLatin1Char('0'));
                tempData += " ";
            }
            tempData += "\n";

            tempData += "rs422_2:";
            for(int i=0;i<256;i++){
                if(i%8==0){
                    tempData +="\n";
                }
                tempData += QString("%1").arg(rs422_2_data[rs422_2_data.size()-256+i],2,16,QLatin1Char('0'));
                tempData += " ";
            }
            tempData += "\n";
        }


        if(packageEnable || normalEnable || highspeedEnable || rs422Enable){
            printToConsole(tempData);
        }


        if(valueLabelEnable){
            for(int i=0;i<5;i++){
                for(int j=0;j<40;j++){
                    auto label_temp = qobject_cast<QLabel *>(tab_layouts[i]->itemAtPosition(j-20<0?j:j-20,j-20>=0?3:1)->widget());
                    label_temp->setText(QString("value:  ").append(QString().setNum(analogData[OUT2INLUT[i*40+j+1]].last().y())));
                }
            }
        }

        cnt = 0;

    }

    if(isDebug){
        cnt++;
    }

}

double MainWindow::data2Voltage(quint32 value, int index){
    return regressionK[index] * value + regressionB[index];
}

void MainWindow::parseData(quint32* data){
    // parse data into rs422 and analogData display
    int channelIndex = 0;
    for(int i=0;i<256;i++){

        if(isSaveEnabled){
            out<<data[i];
        }

        if(i%8==0){// normal and 422_1
            if(i!=0){
                analogData[channelIndex].append((QPointF(normalTimeIndex,data2Voltage(data[i]>>16,channelIndex))));
                channelIndex++;
            }
            rs422_1_data.append(data[i]>>8);
            rs422_1_data.append(data[i]);
        }else if(i%8==1){
            rs422_1_data.append(data[i]>>24);
            rs422_1_data.append(data[i]>>16);
            rs422_1_data.append(data[i]>>8);
            rs422_1_data.append(data[i]);
        }else if(i%8==2){
            rs422_1_data.append(data[i]>>24);
            rs422_1_data.append(data[i]>>16);
            rs422_2_data.append(data[i]>>8);
            rs422_2_data.append(data[i]);
        }else if (i%8==3) {
            rs422_2_data.append(data[i]>>24);
            rs422_2_data.append(data[i]>>16);
            rs422_2_data.append(data[i]>>8);
            rs422_2_data.append(data[i]);
        }else if(i%8==4){ // 422_2 and high speed

            if((currentMode == HIGHSPEED || (lastMode== HIGHSPEED && currentMode==IDLE ))){
                highSpeedData.append(QPointF(highTimeIndex,data2Voltage(data[i]&0x0000FFFF,channelIndex)));
                highTimeIndex += highTimescale;
            }
            rs422_2_data.append(data[i]>>24);
            rs422_2_data.append(data[i]>>16);
        }else if(i%8==5 || i%8==6 || (i<=71 && i%8==7)){ // normal
            analogData[channelIndex].append((QPointF(normalTimeIndex,data2Voltage(data[i]>>16,channelIndex))));
            channelIndex++;

            analogData[channelIndex].append((QPointF(normalTimeIndex,data2Voltage(data[i]&0x0000FFFF,channelIndex))));
            channelIndex++;
        }else if(i>72 && i%8==7){
            analogData[channelIndex].append((QPointF(normalTimeIndex,data2Voltage(data[i]>>16,channelIndex))));
            channelIndex++;
        }
    }

    normalTimeIndex += timescale;
}


void MainWindow::checkWarningState(){
    for(int i=0;i<200;i++){
        if((currentMode == HIGHSPEED || (lastMode== HIGHSPEED && currentMode==IDLE )) && highSpeedChannel==IN2OUTLUT[i]){
            for(int j=highSpeedData.size()-32;j<highSpeedData.size();j++){

                if((highSpeedData[j].y() < warningLimitsLow[i] || highSpeedData[j].y() >= warningLimitsHigh[i]) && !isOverflow[i]){
                    isOverflow[i] = true;
                    checkGroup->button(i)->setStyleSheet("QCheckBox {background-color:rgb(249,244,0);}");
                    ctb->setWaringState(i/40,true);
                    warningCnt[i/40]++;
                    printToConsole(QString("警告：%1 超出阈值！").arg(checkboxNames[i]));
                }

                if(highSpeedData[j].y() >= warningLimitsLow[i] && highSpeedData[j].y() < warningLimitsHigh[i] && isOverflow[i]){
                    isOverflow[i] = false;
                    checkGroup->button(i)->setStyleSheet("QCheckBox {background-color:rgb(255,255,255);}");
                    warningCnt[i/40]--;
                    if(warningCnt[i/40]==0){
                        ctb->setWaringState(i/40,false);
                    }
                    printToConsole(QString("警告解除：%1 恢复正常范围。").arg(checkboxNames[i]));
                }

            }

            continue;
        }

        if((analogData[i].last().y() < warningLimitsLow[i] || analogData[i].last().y() >= warningLimitsHigh[i]) && !isOverflow[i]){
            isOverflow[i] = true;
            checkGroup->button(i)->setStyleSheet("QCheckBox {background-color:rgb(249,244,0);}");
            ctb->setWaringState(i/40,true);
            warningCnt[i/40]++;
            printToConsole(QString("警告：%1 超出阈值！").arg(checkboxNames[i]));
        }

        if(analogData[i].last().y() >= warningLimitsLow[i] && analogData[i].last().y() < warningLimitsHigh[i] && isOverflow[i]){
            isOverflow[i] = false;
            checkGroup->button(i)->setStyleSheet("QCheckBox {background-color:rgb(255,255,255);}");
            warningCnt[i/40]--;
            if(warningCnt[i/40]==0){
                ctb->setWaringState(i/40,false);
            }
            printToConsole(QString("警告解除：%1 恢复正常范围。").arg(checkboxNames[i]));
        }

    }
}

void MainWindow::refreshChart(){

    static int count = 0;
    if(count==20){
        count = 0;
        updateState();
    }else{
        count++;
    }

    bool isUpdate = false;
    double bias = 2/3.0;
    if(normalTimeIndex > end.toMSecsSinceEpoch()){
        begin = begin.addMSecs(xRange*bias);
        end = end.addMSecs(xRange*bias);
        isUpdate = true;
        for(int i=0;i<200;i++){
            analogData[i].remove(0,int(analogData[0].size()*bias));
        }
    }

    for (int i=0;i<inDisplay.size();i++) {
        auto q = (QSplineSeries*)inDisplay[i]->chart()->series()[0];
        if((currentMode == HIGHSPEED || (lastMode== HIGHSPEED && currentMode==IDLE )) && inDisplay[i]->objectName().toInt()+1==highSpeedChannel){
            q->replace(highSpeedData);
        }else{
            q->replace(analogData[OUT2INLUT[inDisplay[i]->objectName().toInt()+1]]);
        }

        if(isUpdate){
            inDisplay[i]->chart()->axes(Qt::Horizontal)[0]->setRange(begin, end);
        }
    }

}

void MainWindow::resetDataIndex(){

    normalTimeIndex = 16*60*60*1000;
    highTimeIndex = 16*60*60*1000;

    begin.setMSecsSinceEpoch(0);
    begin = begin.addSecs(16*60*60);
    end.setMSecsSinceEpoch(xRange);
    end = end.addSecs(16*60*60);

    memset(warningCnt,0,5*sizeof(int));
    memset(isOverflow,0,200*sizeof(bool));

    highSpeedData.clear();
    for(int i=0;i<200;i++){
        analogData[i].clear();
    }

    for (int i=0;i<inDisplay.size();i++) {
        auto q = (QSplineSeries*)inDisplay[i]->chart()->series()[0];
        if((currentMode == HIGHSPEED || (lastMode== HIGHSPEED && currentMode==IDLE )) && inDisplay[i]->objectName().toInt()+1==highSpeedChannel){
            q->replace(highSpeedData);
        }else{
            q->replace(analogData[OUT2INLUT[inDisplay[i]->objectName().toInt()+1]]);
        }
        inDisplay[i]->chart()->axes(Qt::Horizontal)[0]->setRange(begin, end);
    }

}

