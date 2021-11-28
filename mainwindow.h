#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <threadTcp.h>
#include <customtabwidget.h>
#include "qcustomplot.h"
#include "settingwindow.h"

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
    void mouseWheel(QWheelEvent*);
    void horzScrollBarChanged(int,int);
    void xAxisChanged(QCPRange range);
    void realTimeRevChanged(bool);

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

    void pushButton_send_clicked();

    void on_pushButton_set_clicked();

    void on_radioButton_single_clicked();

    void on_radioButton_multi_clicked();

    void on_radioButton_selfcheck_mode_clicked();

    void on_radioButton_reset_mode_clicked();

    void on_pushButton_clicked();

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

    void udpInit(); // anbandoned

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
    void adaptiveRangeChange();
    void setupScroolBar();
    bool isRealTimeReceiving;
    bool isDataRemained;
    void autoSqueeze();
    void selfCheckConfirm();
    void analogInit();
    QCustomPlot* addNewChart(int);


    /*
     * statse display
    */
    void stateInit();
    bool isConnected;
    bool isReset;
    bool isSelfchecked;
    bool isNormalChecked;
    bool isRs422Checked;

    enum ModeList {SELFCHECK,NORMAL,HIGHSPEED,MULTI,SINGLE,RESET,IDLE};
    ModeList currentMode;
    ModeList lastMode;


};
#endif // MAINWINDOW_H
