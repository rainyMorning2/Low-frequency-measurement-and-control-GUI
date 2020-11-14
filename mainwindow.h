#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QButtonGroup>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QSplineSeries>
#include <QSettings>

using namespace QtCharts;

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

    /*
     * tcp/ip connection
    */
    void on_pushButton_Connect_clicked();
    void socket_Read_Data();
    void socket_Disconnected();

    /*
     * mode control
    */

    void on_radioButton_high_speed_mode_toggled(bool checked);
    void on_pushButton_start_clicked();
    void on_pushButton_save_clicked();
    void on_pushButton_load_clicked();
    void on_pushButton_stop_clicked();
    void check(int,bool);

private:
    Ui::MainWindow *ui;
    void printToConsole(QString);
    QSettings* settings;
    int maxDisplay;
    int currentNum;

    /*
     * tcp/ip connection
    */
    void tcpInit();
    void sendMessage(QString);
    QTcpSocket* socket;

    /*
     * mode control
    */
    void modeCtrlInit();
    QButtonGroup* buttonGroup;

    /*
     * analog display
    */
    void displayInit();


    /*
     * analog choose
    */
    void analogInit();
    QButtonGroup* checkGroup;

    /*
     * state display
    */
    void updateState();
};
#endif // MAINWINDOW_H
