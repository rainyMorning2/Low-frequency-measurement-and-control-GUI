#include "settingwindow.h"
#include "ui_settingwindow.h"
#include <QButtonGroup>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

QButtonGroup* checkGroup2;
QSettings* settings;

settingWindow::settingWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settingWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("控制设置");

    QString file= QCoreApplication::applicationDirPath() + "/Config.ini";
    settings = new QSettings(file, QSettings::IniFormat);
    settings->setIniCodec("UTF8");


    checkGroup2 = new QButtonGroup();
    checkGroup2->setExclusive(false);
    for (int i=0;i<24;i++) {
        QLabel *label = new QLabel(QString("DA %1").arg(i+1));
        QLineEdit *lineEdit = new QLineEdit();
        QCheckBox* checkbox = new QCheckBox(QString("开关 %1").arg(i+1));
        ui->gridLayout_da->addWidget(label,i<12?i+1:i-11,i<12?0:2);
        ui->gridLayout_da->addWidget(lineEdit,i<12?i+1:i-11,i<12?1:3);
        ui->gridLayout_key->addWidget(checkbox,i<12?i+1:i-11,i<12?0:1);
        checkGroup2->addButton(checkbox,i);
    }

    ui->groupBox_da->setEnabled(false);
    ui->groupBox_key->setEnabled(false);
    ui->groupBox_rs422->setEnabled(false);
    ui->buttonGroup_2->setId(ui->checkBox_send,1);
    ui->buttonGroup_2->setId(ui->checkBox_data,2);
    ui->buttonGroup_2->setExclusive(false);
    ui->buttonGroup_3->setExclusive(false);
    ui->pushButton_select1->setEnabled(false);
    ui->comboBox->setEnabled(false);
    ui->radioButton_up->setChecked(true);
    ui->lineEdit_interval->setText("5");
    ui->lineEdit_interval->setEnabled(false);
    connect(ui->buttonGroup_2,SIGNAL(idToggled(int,bool)),this,SLOT(rs422_mode(int,bool)));
}

settingWindow::~settingWindow()
{
    delete ui;
}

void settingWindow::on_checkBox_da_toggled(bool checked)
{
    ui->groupBox_da->setEnabled(checked);
}

void settingWindow::on_checkBox_key_toggled(bool checked)
{
    ui->groupBox_key->setEnabled(checked);
}

void settingWindow::on_checkBox_rs422_toggled(bool checked)
{
    ui->groupBox_rs422->setEnabled(checked);
}

void settingWindow::on_pushButton_selectAll_clicked()
{
    foreach(QAbstractButton* button, checkGroup2->buttons()){
        button->setChecked(true);
    }
}

void settingWindow::on_pushButton_selectReverse_clicked()
{
    foreach(QAbstractButton* button, checkGroup2->buttons()){
        button->setChecked(!button->isChecked());
    }
}

void settingWindow::on_pushButton_fill_clicked()
{
    QString default_text = ui->lineEdit_default->text();
    for(int i=0;i<24;i++){
        int row = i<12?i+1:i-11;
        int col = i<12?1:3;
        auto lineEdit_temp = qobject_cast<QLineEdit *>(ui->gridLayout_da->itemAtPosition(row,col)->widget());
        lineEdit_temp->setText(default_text);
    }
}

void settingWindow::on_pushButton_clear_clicked()
{
    for(int i=0;i<24;i++){
        int row = i<12?i+1:i-11;
        int col = i<12?1:3;
        auto lineEdit_temp = qobject_cast<QLineEdit *>(ui->gridLayout_da->itemAtPosition(row,col)->widget());
        lineEdit_temp->clear();
    }
}

void settingWindow::rs422_mode(int id,bool checked){

    if(ui->checkBox_data->isChecked() && ui->checkBox_send->isChecked()){
        if(id==1){
            ui->checkBox_data->setChecked(false);
        }else if(id==2){
            ui->checkBox_send->setChecked(false);
        }

    }

    if(id==2){
        ui->pushButton_select1->setEnabled(checked);
        ui->comboBox->setEnabled(checked);
        ui->lineEdit_interval->setEnabled(checked);
    }
}

void settingWindow::on_pushButton_apply_clicked()
{

    // get settings
    bool isDAEnabled = ui->checkBox_da->isChecked();
    bool isKeyEnabled = ui->checkBox_key->isChecked();
    bool isRs422Enabled = ui->checkBox_rs422->isChecked();


    int a1 = ui->lineEdit->text().toInt();
    int a2 = ui->lineEdit_2->text().toInt();
    int a3 = ui->lineEdit_3->text().toInt();
    int a4 = ui->lineEdit_4->text().toInt();

    // generate 1K package
    QByteArray mess(1024,0);
    // package head
    mess[0] = 0xFA;
    mess[1] = 0xF3;
    mess[2] = 0x20;

    //mode : byte[0]=DA, byte[1]=Key, byte[2]=RS422
    mess[3] = isDAEnabled<<3;

    if(!isDAEnabled){
        mess[4] = mess[4] | a1;
        mess[5] = mess[5] | a2;
        mess[6] = mess[6] | a3;
        mess[7] = mess[7] | a4;
    }

    if(isDAEnabled){
        // get related parameters
        double *DAData = new double[24];
        for(int i=0;i<24;i++){
            int row = i<12?i+1:i-11;
            int col = i<12?1:3;
            auto lineEdit_temp = qobject_cast<QLineEdit *>(ui->gridLayout_da->itemAtPosition(row,col)->widget());
            double bias = settings->value(QString("DASet/index%1").arg(i+1), settings->value(QString("DASet/default"))).toDouble();
            DAData[i] = lineEdit_temp->text().toDouble();
            if(DAData[i]-0<1e-4){
                DAData[i]=0;
            }
            if(5-DAData[i]<1e-4){
                DAData[i]=5;
            }
            DAData[i] = (DAData[i]+bias)/5*1023;
        }

        // assign data
        // DA data mess[8]:low, mess[9]:high
        for(int i=0;i<24;i++){
            mess[8+i*2] = mess[8+i*2] | (unsigned int) DAData[i];
            mess[8+i*2+1] = mess[8+i*2+1] | ((unsigned int)DAData[i]>>8);
        }
    }

    if(isKeyEnabled){
        // get related parameters
        bool *key = new bool[24];
        for(int i=0;i<24;i++){
            int row = i<12?i+1:i-11;
            int col = i<12?0:1;
            auto checkbox_temp = qobject_cast<QCheckBox *>(ui->gridLayout_key->itemAtPosition(row,col)->widget());
            key[i] = checkbox_temp->isChecked();
        }

        // key map
        for(int i=0;i<24;i++){
            mess[60+i/8] = mess[60+i/8] | (key[i]<<(i%8));
        }
    }

    if(isRs422Enabled){
        // get related parameters
        bool trigger = ui->radioButton_up->isChecked();
        bool isSendEnabled = ui->checkBox_send->isChecked();
        bool isDataEnabled = ui->checkBox_data->isChecked();
        bool *isChannelEnabled = new bool[4];
        isChannelEnabled[0] = ui->checkBox_A->isChecked();
        isChannelEnabled[1] = ui->checkBox_B->isChecked();
        isChannelEnabled[2] = ui->checkBox_C->isChecked();
        isChannelEnabled[3] = ui->checkBox_D->isChecked();

        // set trigger mode
        mess[63] = trigger<<7;

        // send enable
        if(isSendEnabled){
            mess[63] = mess[63] | isChannelEnabled[0] | (isChannelEnabled[1]<<1) | (isChannelEnabled[2]<<2) | (isChannelEnabled[3]<<3);
            mess[64] = mess[64] | (isChannelEnabled[0]<<4) | (isChannelEnabled[1]<<5) | (isChannelEnabled[2]<<6) | (isChannelEnabled[3]<<7);
        }

        // data enable
        if(isDataEnabled){

            int interval = ui->lineEdit_interval->text().toInt();
            // Rs422 data from file
            QString filename= ui->comboBox->currentText();
            QFile file;
            if(!filename.isEmpty()){
                file.setFileName(filename);
            }else{
                QMessageBox::critical(NULL, "文件错误", "请先选择数据文件！", "是");
                return;
            }
            file.open(QIODevice::ReadOnly);
            QByteArray rs422Data = file.readAll();
            int fileSize = rs422Data.size();
            if(fileSize > 950){
                QMessageBox::critical(NULL, "文件错误", "文件大小不能超过950B，请重新选择。", "是");
                return;
            }


            mess[64] = mess[64] | isChannelEnabled[0] | (isChannelEnabled[1]<<1) | (isChannelEnabled[2]<<2) | (isChannelEnabled[3]<<3);

            //rs422 data length
            mess[65] = mess[65] | fileSize; //low
            mess[66] = mess[66] | (fileSize>>8); //high

            //rs422 interval
            interval %= 65535;
            mess[67] = mess[67] | interval; //low
            mess[68] = mess[68] | (interval>>8); //high

            // rs422 data
            mess.replace(207,fileSize,rs422Data);

        }

    }

    QByteArray mess2(64,0);
    mess2[0] = 0x43;
    mess2[1] = 0x4F;
    mess2[2] = 0x4D;
    mess2[3] = 0x44;

    mess2[7] = 0x01;

    mess2[15] = 0x01;
    mess2[17] = 0x02;
    mess2[19] = 0x03;

    mess2[21] = 0x04;
    mess2[23] = 0x05;
    mess2[25] = 0x06;
    mess2[27] = 0x11;
    mess2[29] = 0x12;
    mess2[31] = 0x13;
    mess2[33] = 0x15;
    mess2[35] = 0x08;
    mess2[37] = 0x09;

    mess2[39] = 0x0A;
    mess2[41] = 0x0B;
    mess2[43] = 0x0C;
    mess2[45] = 0x0D;
    mess2[47] = 0x0E;
    mess2[49] = 0x18;
    mess2[51] = 0x19;

    mess2[59] = 0x01;

    mess2[60] = 0x4F;
    mess2[61] = 0x56;
    mess2[62] = 0x45;
    mess2[63] = 0x52;

    mess.replace(79,64,mess2);

    // check byte as package end
    for(int i=3;i<1023;i++){
        mess[1023] = mess[i] ^ mess[1023];
    }

    QFile outFile(QCoreApplication::applicationDirPath() + "/DAdata.dat");
    outFile.open(QIODevice::WriteOnly);
    outFile.write(mess);
    outFile.close();

}

void settingWindow::on_pushButton_send_clicked()
{
    emit send_sig();
}

void settingWindow::on_pushButton_select1_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(QStringLiteral("打开"));
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

void settingWindow::on_pushButton_clicked()
{


}
