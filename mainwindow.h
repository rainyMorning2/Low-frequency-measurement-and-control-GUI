#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "customdatatrans.h"
#include "qcustomplot.h"
#include <QThread>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void parseData(QByteArray);

    void on_pushButton_2_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_8_clicked();

    void stateChange(bool);

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void check(int, bool);

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

signals:
    void sig_connect(QString,int);
    void sig_reset();
    void sig_setChannel(int*,int,bool);
    void sig_setSingleChannel(int);
    void sig_adSample(int);

    void sig_digitalEnable422(QByteArray src_mess,int boudRate,int stop,int check,int len, bool isOpenAD);
    void sig_digitalSet485(int channel_send, int channel_rev, QByteArray src_mess, int trigger);
    void sig_digitalEnable485(int channel_send,int channel_rev, int len, bool isOpenAD);

    void sig_impulse(int channel, int width, double height,int len,bool isOpenAD,bool rs422Enabled);
    void sig_selfcheck(bool);


private:
    Ui::MainWindow *ui;
    customDataTrans* dataTrans;
    QSettings* settings;
    void parseData(quint32* data,int channel_index,int channel);
    QCustomPlot* addNewChart();
    double data2Voltage(quint32 value, int index);
    void setChannels();

};
#endif // MAINWINDOW_H
