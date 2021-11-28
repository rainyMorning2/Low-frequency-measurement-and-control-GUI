#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>

QSharedPointer<QCPDataContainer<QCPGraphData>> normalData(new QCPDataContainer<QCPGraphData>);

// 1us
double timeStartBias = 16*60*60;
double normalTimeIndex = 16*60*60;
double timescale = 1.0/1000000;

// channel number map
const int IN2OUTLUT[200] = {10,	9,	26,	8,	27,	28,	29,	30,	12,	11,	13,	20,	2,	19,	1,	3,	21,	4,	24,	23,	22,	5,	6,	7,	25,	37,	38,	39,	40,	36,	35,	18,	15,	34,	33,	17,	16,	32,	31,	14,	50,	49,	66,	48,	67,	68,	69,	70,	52,	51,	53,	60,	42,	59,	41,	43,	61,	44,	64,	63,	62,	45,	46,	47,	65,	77,	78,	79,	80,	76,	75,	58,	55,	74,	73,	57,	56,	72,	71,	54,	90,	89,	106,	88,	107,	108,	109,	110,	92,	91,	93,	100,	82,	99,	81,	83,	101,	84,	104,	103,	102,	85,	86,	87,	105,	117,	118,	119,	120,	116,	115,	98,	95,	114,	113,	97,	96,	112,	111,	94,	130,	129,	146,	128,	147,	148,	149,	150,	132,	131,	133,	140,	122,	139,	121,	123,	141,	124,	144,	143,	142,	125,	126,	127,	145,	157,	158,	159,	160,	156,	155,	138,	135,	154,	153,	137,	136,	152,	151,	134,	170,	169,	186,	168,	187,	188,	189,	190,	172,	171,	173,	180,	162,	179,	161,	163,	181,	164,	184,	183,	182,	165,	166,	167,	185,	197,	198,	199,	200,	196,	195,	178,	175,	194,	193,	177,	176,	192,	191,	174};
const int OUT2INLUT[201] = {-1, 14,	12,	15,	17,	21,	22,	23,	3,	1,	0,	9,	8,	10,	39,	32,	36,	35,	31,	13,	11,	16,	20,	19,	18,	24,	2,	4,	5,	6,	7,	38,	37,	34,	33,	30,	29,	25,	26,	27,	28,	54,	52,	55,	57,	61,	62,	63,	43,	41,	40,	49,	48,	50,	79,	72,	76,	75,	71,	53,	51,	56,	60,	59,	58,	64,	42,	44,	45,	46,	47,	78,	77,	74,	73,	70,	69,	65,	66,	67,	68,	94,	92,	95,	97,	101,	102,	103,	83,	81,	80,	89,	88,	90,	119,	112,	116,	115,	111,	93,	91,	96,	100,	99,	98,	104,	82,	84,	85,	86,	87,	118,	117,	114,	113,	110,	109,	105,	106,	107,	108,	134,	132,	135,	137,	141,	142,	143,	123,	121,	120,	129,	128,	130,	159,	152,	156,	155,	151,	133,	131,	136,	140,	139,	138,	144,	122,	124,	125,	126,	127,	158,	157,	154,	153,	150,	149,	145,	146,	147,	148,	174,	172,	175,	177,	181,	182,	183,	163,	161,	160,	169,	168,	170,	199,	192,	196,	195,	191,	173,	171,	176,	180,	179,	178,	184,	162,	164,	165,	166,	167,	198,	197,	194,	193,	190,	189,	185,	186,	187,	188};

int channels[20];
int checkBoxCount = 0;
bool isChannelSet = false;

double *regressionK;
double *regressionB;
double *warningLimitsLow;
double *warningLimitsHigh;

double avg;

QButtonGroup* checkGroup;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dataTrans = new customDataTrans();
    QThread* workThread = new QThread();
    dataTrans->moveToThread(workThread);
    workThread->start();

    warningLimitsLow = new double[200];
    warningLimitsHigh = new double[200];
    regressionB = new double[201];
    regressionK = new double[201];

    QString file= QCoreApplication::applicationDirPath() + "/config.ini";
    settings = new QSettings(file, QSettings::IniFormat);
    settings->setIniCodec("UTF8");

    QVariant defaultLimit = settings->value("WarningLimit/default");
    QVariant defaultRes = settings->value("Regression/default");
    QStringList checkboxNames = settings->value("checkboxNames").toStringList();
    regressionK[200] = defaultRes.toList()[0].toDouble();
    regressionB[200] = defaultRes.toList()[1].toDouble();
    for(int i=0;i<200;i++){
        QList<QVariant> limits = settings->value(QString("WarningLimit/index%1").arg(i+1), defaultLimit).toList();
        QList<QVariant> regression = settings->value(QString("Regression/index%1").arg(i+1), defaultRes).toList();
        warningLimitsLow[OUT2INLUT[i+1]] = limits[0].toDouble();
        warningLimitsHigh[OUT2INLUT[i+1]] = limits[1].toDouble();
        regressionK[OUT2INLUT[i+1]] = regression[0].toDouble();
        regressionB[OUT2INLUT[i+1]] = regression[1].toDouble();
    }

    QList<QGridLayout*> tab_layouts;
    checkGroup = new QButtonGroup();
    checkGroup->setExclusive(false);

    for(int i=0;i<5;i++){
        tab_layouts.append(new QGridLayout());
        for(int j=0;j<40;j++){
            QCheckBox* checkbox = new QCheckBox(checkboxNames[i*40+j]);
            tab_layouts[i]->addWidget(checkbox,j-20<0?j:j-20,j-20>=0?1:0);
            checkGroup->addButton(checkbox,i*40+j);
        }
    }

    ui->tab->setLayout(tab_layouts[0]);
    ui->tab_2->setLayout(tab_layouts[1]);
    ui->tab_3->setLayout(tab_layouts[2]);
    ui->tab_4->setLayout(tab_layouts[3]);
    ui->tab_5->setLayout(tab_layouts[4]);

    QCustomPlot* mchart = addNewChart();
    ui->verticalLayout->addWidget(mchart);


    connect(checkGroup,SIGNAL(idToggled(int,bool)),this,SLOT(check(int,bool)));


    connect(this,SIGNAL(sig_connect(QString,int)),dataTrans,SLOT(connetTo_slot(QString,int)));
    connect(this,SIGNAL(sig_reset()),dataTrans,SLOT(reset()));
    connect(this,SIGNAL(sig_setChannel(int*,int,bool)),dataTrans,SLOT(setChannel_slot(int*,int,bool)));
    connect(this,SIGNAL(sig_setSingleChannel(int)),dataTrans,SLOT(setSingleChannel_slot(int)));
    connect(this,SIGNAL(sig_adSample(int)),dataTrans,SLOT(adSample(int)));

    connect(this,SIGNAL(sig_digitalEnable422(QByteArray,int,int,int,int,bool)),dataTrans,SLOT(digitalEnable422(QByteArray,int,int,int,int,bool)));
    connect(this,SIGNAL(sig_digitalSet485(int, int, QByteArray, int )),dataTrans,SLOT(digitalSet485(int, int, QByteArray, int )));
    connect(this,SIGNAL(sig_digitalEnable485(int,int, int , bool )),dataTrans,SLOT(digitalEnable485(int,int, int , bool )));

    connect(this,SIGNAL(sig_impulse(int , int , double ,int ,bool ,bool )),dataTrans,SLOT(impulse(int , int , double ,int ,bool ,bool )));
    connect(this,SIGNAL(sig_selfcheck(bool)),dataTrans,SLOT(selfcheck(bool)));

    connect(dataTrans,SIGNAL(returnValue(QByteArray)),this,SLOT(parseData(QByteArray)));
    connect(dataTrans,SIGNAL(sig_connectionStateChanged(bool)),this,SLOT(stateChange(bool)));
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::check(int id, bool checked)
{
    if(checked){
        if(checkBoxCount < 20){
            checkBoxCount++;
        }
    }else{
        // remove from channels
        checkBoxCount--;
    }

    static bool isAllEnabled = true;
    if(checkBoxCount >= 20){
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

void MainWindow::setChannels(){
    int i = 0;
    QString text0 = "通道索引：";
    QString text1 = "当前通道：";
    foreach(QAbstractButton* button, checkGroup->buttons()){
        if(button->isChecked()){
            channels[i]=checkGroup->id(button)+1;

            text1 += QString::number(channels[i]);
            text1 += " ";

            if(channels[i]<10){
                text0 += QString::number(i+1);
                text0 += " ";
            }else if(channels[i]<100){
                if(i+1<10){
                    text0 += QString::number(i+1);
                    text0 += "  ";
                }else{
                    text0 += QString::number(i+1);
                    text0 += " ";
                }

            }else{
                if(i+1<10){
                    text0 += " ";
                    text0 += QString::number(i+1);
                    text0 += "  ";
                }else{
                    text0 += QString::number(i+1);
                    text0 += "  ";
                }

            }

            i++;
        }
    }

    if(i!=checkBoxCount){
        QMessageBox::warning(NULL, "Critical", "请重启以确保通道配置正确", QMessageBox::Yes);
        return;
    }

    ui->label_16->setText(text1);
    ui->label_17->setText(text0);

    if(i==0){
        isChannelSet = false;
    }else{
        isChannelSet = true;
    }

}

void MainWindow::stateChange(bool state){
    QString str = state?"断开连接":"连接";
    ui->pushButton_2->setText(str);
}

void MainWindow::parseData(QByteArray data){
    // 解析数据
    QFile outFile(QCoreApplication::applicationDirPath() + "/receivedData.dat");
    outFile.open(QIODevice::WriteOnly);
    outFile.write(data);
    outFile.close();
}

void MainWindow::on_pushButton_clicked()
{
    // ad sample
    emit sig_adSample(ui->lineEdit->text().toInt());
//    std::this_thread::sleep_for(std::chrono::milliseconds(100));
//    on_pushButton_10_clicked();
}

void MainWindow::on_pushButton_2_clicked()
{
    emit sig_connect("192.168.1.10",8080);
}

void MainWindow::on_pushButton_7_clicked()
{
    // reset
    emit sig_reset();
}

void MainWindow::on_pushButton_3_clicked()
{
    setChannels();
    if(checkBoxCount==0){
        QMessageBox::warning(NULL, "Warning", "请先至少选择一个通道（至多20个）", QMessageBox::Yes);
        return;
    }

    if(ui->checkBox_6->isChecked()){
        emit sig_setSingleChannel(channels[0]);
    }else{
        // set channel
        emit sig_setChannel(channels,checkBoxCount,ui->checkBox_2->isChecked());
    }

}

void MainWindow::on_pushButton_6_clicked()
{
    // impluse
    emit sig_impulse(ui->lineEdit_7->text().toInt(),ui->lineEdit_9->text().toInt(),ui->lineEdit_10->text().toDouble(),ui->lineEdit_11->text().toInt(),ui->checkBox_3->isChecked(),ui->checkBox_4->isChecked());
}

void MainWindow::on_pushButton_4_clicked()
{
    // 485 set
    QFile tempFile(QCoreApplication::applicationDirPath() + "/Rs485.dat");
    tempFile.open(QIODevice::ReadOnly);
    QByteArray mess = tempFile.readAll();
    emit sig_digitalSet485(ui->lineEdit_2->text().toInt(),ui->lineEdit_12->text().toInt(),mess,ui->lineEdit_6->text().toInt());
    tempFile.close();
}

void MainWindow::on_pushButton_5_clicked()
{
    // 485 enable
    emit sig_digitalEnable485(ui->lineEdit_2->text().toInt(),ui->lineEdit_12->text().toInt(),ui->lineEdit_8->text().toInt(),ui->checkBox->isChecked());
}

void MainWindow::on_pushButton_8_clicked()
{
    // self check
    emit sig_selfcheck(ui->checkBox_5->isChecked());
}

void MainWindow::on_pushButton_9_clicked()
{
    // 422 send
    QFile tempFile(QCoreApplication::applicationDirPath() + "/Rs422.dat");
    tempFile.open(QIODevice::ReadOnly);
    QByteArray mess = tempFile.readAll();
    emit sig_digitalEnable422(mess,ui->lineEdit_3->text().toInt(),ui->lineEdit_4->text().toInt(),ui->lineEdit_5->text().toInt(),ui->lineEdit_8->text().toInt(),ui->checkBox->isChecked());
}

QCustomPlot* MainWindow::addNewChart(){

    QString title = "采样曲线";
    int leftYRange = -1;
    int rightYRange = 6;

    QCustomPlot* mChart = new QCustomPlot();

//    customScrollBar *horizontalScrollBar = new customScrollBar(mChart,id);
//    horizontalScrollBar->setOrientation(Qt::Horizontal);
//    horizontalScrollBar->setVisible(!isRealTimeReceiving);
//    if(!isRealTimeReceiving){
//        QCPRange range = mChart->xAxis->range();
//        scroolBarEnd = normalTimeIndex*100;
//        horizontalScrollBar->setRange(scroolBarBegin,scroolBarEnd);
//        horizontalScrollBar->setValue(qRound(range.center()*100.0)); // adjust position of scroll bar slider
//        horizontalScrollBar->setPageStep(qRound(range.size()*100.0)); // adjust size of scroll bar slider
//    }

    mChart->setLayout(new QVBoxLayout(mChart));
//    mChart->layout()->addWidget(horizontalScrollBar);
    mChart->layout()->setAlignment(Qt::AlignBottom);

    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("hh:mm:ss.zzz");
    mChart->xAxis->setTicker(dateTicker);
    mChart->addGraph();

    mChart->graph(0)->setData(normalData);
    mChart->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    mChart->setInteraction(QCP::iRangeDrag,true);
    mChart->setInteraction(QCP::iRangeZoom,true);
    mChart->axisRect()->setRangeDrag(Qt::Horizontal);
    mChart->axisRect()->setRangeZoom(Qt::Horizontal);

    mChart->plotLayout()->insertRow(0);
    mChart->plotLayout()->addElement(0, 0, new QCPTextElement(mChart, title, QFont("sans", 10, QFont::Bold)));
    mChart->plotLayout()->insertRow(2);
    mChart->plotLayout()->addElement(2, 0, new QCPTextElement(mChart, "", QFont("sans", 14, QFont::Bold)));
    mChart->xAxis->setLabel("sample-time");
    mChart->yAxis->setLabel("Voltage-V");
    mChart->xAxis->setRange(timeStartBias,timeStartBias+5);
    mChart->yAxis->setRange(leftYRange, rightYRange);

    mChart->setOpenGl(true);
    mChart->graph(0)->setSmooth(ui->checkBox_7->isChecked());

//    connect(mChart,SIGNAL(mouseWheel(QWheelEvent*)),this,SLOT(mouseWheel(QWheelEvent*)));
//    connect(horizontalScrollBar, SIGNAL(customValueChanged(int,int)), this, SLOT(horzScrollBarChanged(int,int)));
//    connect(mChart->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));

    return mChart;

}


void MainWindow::on_pushButton_10_clicked()
{
    if(!isChannelSet){
        QMessageBox::warning(NULL, "Warning", "请先配置通道", QMessageBox::Yes);
        return;
    }

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

    QCustomPlot* mchart = addNewChart();
    if(ui->verticalLayout->count()!=0){
        normalData->clear();
        normalTimeIndex = timeStartBias;
        ui->verticalLayout->removeItem(ui->verticalLayout->takeAt(0));
    }
    ui->verticalLayout->addWidget(mchart);

    int channel_index = ui->lineEdit_13->text().toInt();

//    if(true){
//        QFile file(QCoreApplication::applicationDirPath() + "/receivedData.dat");
    if(!fileNames.isEmpty()){
        QFile file(fileNames[0]);
        file.open(QIODevice::ReadOnly);
        QDataStream in(&file);    // read the data serialized from the file
        quint32 **datas = new quint32*[file.size()/1024];

//        QFile outFile(QCoreApplication::applicationDirPath() + "/ChannelOneData.txt");
//        outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
//        outFile.resize(0);
//        outFile.close();
        avg = 0;

        for(int i=0;i<file.size()/1024;i++){
            datas[i] = new quint32[256];
            for(int j=0;j<256;j++){
                in>>datas[i][j];
            }
            parseData(datas[i],channel_index,channels[channel_index-1]);
        }
        avg /= file.size()/1024;
        ui->label_15->setText(QString::number(avg));

        mchart->xAxis->setRange(timeStartBias,timeStartBias+timescale*normalData->size());
        int id = channels[channel_index-1]-1;
        if((id%40>=13 && id%40<=16) || (id%40>=30&&id%40<=33)){
            mchart->yAxis->setRange(-5, 65);
        }
        mchart->replot();
    }
}

void MainWindow::parseData(quint32* data,int channel_index,int channel){
    // parse data into rs422 and analogData display
    if(ui->checkBox_6->isChecked()){
        for(int i=1;i<161;i++){
            normalData->add(QCPGraphData(normalTimeIndex, data2Voltage(data[i]>>16,OUT2INLUT[channel])));
            normalTimeIndex += timescale;
            normalData->add(QCPGraphData(normalTimeIndex, data2Voltage(data[i]&0x0000FFFF,OUT2INLUT[channel])));
            normalTimeIndex += timescale;
        }
    }else{

//        QFile outFile(QCoreApplication::applicationDirPath() + "/ChannelOneData.txt");
//        outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
//        QTextStream out(&outFile);

        double val = 0;
        double total = 0;
        for(int i=1;i<161;i++){
            if(i%10==(channel_index+1)/2){
                if(channel_index%2==1){
                    val = data2Voltage(data[i]>>16,OUT2INLUT[channel]);
                    normalData->add(QCPGraphData(normalTimeIndex, val));
                    normalTimeIndex += timescale;
//                    out << val << '\n';
                    total += val;
                }else{
                    val = data2Voltage(data[i]&0x0000FFFF,OUT2INLUT[channel]);
                    normalData->add(QCPGraphData(normalTimeIndex, val));
                    normalTimeIndex += timescale;
                    total += val;
                }

            }
        }
        avg += total/16.0;
//        outFile.close();
    }


}

double MainWindow::data2Voltage(quint32 value, int index){
    return regressionK[index] * value + regressionB[index];
}

void MainWindow::on_pushButton_11_clicked()
{
    setChannels();
}


void MainWindow::on_pushButton_12_clicked()
{
    int index = ui->tabWidget->currentIndex();
    foreach(QAbstractButton* button, checkGroup->buttons()){
        if(checkGroup->id(button)>=index*40 && checkGroup->id(button)<index*40+20){
            continue;
        }else{
            button->setChecked(false);
        }
    }

    foreach(QAbstractButton* button, checkGroup->buttons()){
        if(checkGroup->id(button)>=index*40 && checkGroup->id(button)<index*40+20){
            button->setChecked(!button->isChecked());
        }else{
            continue;
        }
    }

}


void MainWindow::on_pushButton_13_clicked()
{
    int index = ui->tabWidget->currentIndex();
    foreach(QAbstractButton* button, checkGroup->buttons()){
        if(checkGroup->id(button)>=index*40+20 && checkGroup->id(button)<index*40+40){
            continue;
        }else{
            button->setChecked(false);
        }
    }

    foreach(QAbstractButton* button, checkGroup->buttons()){
        if(checkGroup->id(button)>=index*40+20 && checkGroup->id(button)<index*40+40){
            button->setChecked(!button->isChecked());
        }else{
            continue;
        }
    }
}

