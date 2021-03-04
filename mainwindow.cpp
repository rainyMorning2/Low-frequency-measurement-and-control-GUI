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
    settings->setIniCodec("UTF8");

    tcpInit();
    modeCtrlInit();
    analogInit();
    stateInit();

//    ui->textBrowser_console->addAction(clear);
//    ui->menubar->addAction(clear);

}

MainWindow::~MainWindow()
{
    delete socket;
    delete settings;
    delete buttonGroup;
    delete checkGroup;
    delete ui;
}

void MainWindow::printToConsole(QString mess){
    ui->textBrowser_console->append(mess);
}

void MainWindow::clearConsole(){
    ui->textBrowser_console->clear();
}

void QTextEdit::contextMenuEvent(QContextMenuEvent *event){
    qDebug()<<"eh";
    QMenu *menu = createStandardContextMenu();
    QAction *clear = new QAction("clear");
    menu->addAction(clear);
    connect(this,SIGNAL(triggered()),this,SLOT(clearConsole()));
    //...
    menu->exec(event->globalPos());
    delete menu;
}
