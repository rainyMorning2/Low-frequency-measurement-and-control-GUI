#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <threadTcp.h>
#include <customtabwidget.h>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void stateChange();
    void sig_connect(QString,int);
    void sig_disconnect();
    void sig_sendMessage(QByteArray);

private slots:
    void printToConsole(QString);

    /*
     * analog display
    */
    void refreshAnalogData(quint32*);
    void refreshChart();

    /*
     * tcp/ip connection
    */
    void connectSucceed();
    void connectFailed();
    void disconnectSucceed();
    void on_pushButton_Connect_clicked();

    void readPendingDatagrams();


    /*
     * mode control
    */

    void on_radioButton_high_speed_mode_toggled(bool checked);
    void on_pushButton_start_clicked();
    void on_pushButton_save_clicked();
    void on_pushButton_load_clicked();
    void on_pushButton_stop_clicked();
    void check(int,bool);
    void on_toolButton_clicked();

    /*
     * state display
    */
    void updateState();

private:
    Ui::MainWindow *ui;
    QSettings* settings;
    QThread* tcpWorkThread;

    /*
     * tcp/ip connection
    */
    ThreadTcp* threadSocket;
    void tcpInit();
    void sendMessage(QByteArray);
    void parse_data(QByteArray);

    void udpInit();

    /*
     * mode control
    */

    void modeCtrlInit();
    void resetDataIndex();
    bool isSaveEnabled;
    QFile file;
    QDataStream out;
    int highSpeedChannel;

    /*
     * analog display
    */

    int xRange;
    bool* isOverflow;
    double highTimeIndex;
    double normalTimeIndex;

    void resortCharts(bool);
    void checkWarningState();
    void parseData(quint32*);
    double data2Voltage(quint32, int);
    void changeToHighspeedData();

    void analogInit();
    QCustomPlot* addNewChart(int);


    /*
     * statse display
    */
    void stateInit();
    bool isConnected;
    bool isReset;
    bool isSelfchecked;
    enum ModeList {SELFCHECK,NORMAL,HIGHSPEED,RESET,IDLE};
    ModeList currentMode;
    ModeList lastMode;


};
#endif // MAINWINDOW_H
