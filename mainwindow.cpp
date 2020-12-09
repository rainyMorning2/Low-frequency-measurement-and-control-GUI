#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setWindowState(Qt::WindowMaximized);
    ui->setupUi(this);

    QString file;
    file = QCoreApplication::applicationDirPath() + "/Config.ini";
    settings = new QSettings(file, QSettings::IniFormat);

    tcpInit();
    modeCtrlInit();
    analogInit();

}

MainWindow::~MainWindow()
{
    delete this->socket;
    delete ui;
}

void MainWindow::printToConsole(QString mess){
    ui->textBrowser_console->append(mess);
}



