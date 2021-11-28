#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
//    setWindowState(Qt::WindowMaximized);
    ui->setupUi(this);
    setWindowTitle("低频测控组合控制程序");

    QString file= QCoreApplication::applicationDirPath() + "/Config.ini";
    settings = new QSettings(file, QSettings::IniFormat);
    settings->setIniCodec("UTF8");

    tcpInit();
    modeCtrlInit();
    analogInit();
    stateInit();

}

MainWindow::~MainWindow()
{
    delete settings;
    delete ui;
}

void MainWindow::printToConsole(QString mess){
    ui->textBrowser_console->append(QTime::currentTime().toString()+" "+mess);
}






void MainWindow::on_pushButton_clicked()
{
    QByteArray mess(64,0);
    mess[0] = 0x43;
    mess[1] = 0x4F;
    mess[2] = 0x4D;
    mess[3] = 0x44;

    mess[7] = 0x01;

    mess[15] = 0x01;
    mess[17] = 0x02;
    mess[19] = 0x03;

    mess[21] = 0x04;
    mess[23] = 0x05;
    mess[25] = 0x06;
    mess[27] = 0x11;
    mess[29] = 0x12;
    mess[31] = 0x13;
    mess[33] = 0x15;
    mess[35] = 0x08;
    mess[37] = 0x09;

    mess[39] = 0x0A;
    mess[41] = 0x0B;
    mess[43] = 0x0C;
    mess[45] = 0x0D;
    mess[47] = 0x0E;
    mess[49] = 0x18;
    mess[51] = 0x19;

    mess[59] = 0x01;

    mess[60] = 0x4F;
    mess[61] = 0x56;
    mess[62] = 0x45;
    mess[63] = 0x52;


    sendMessage(mess);
}
