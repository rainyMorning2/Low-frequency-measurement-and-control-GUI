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
