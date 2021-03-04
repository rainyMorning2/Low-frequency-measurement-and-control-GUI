#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QButtonGroup>
#include <QtCharts/QChartView>
#include <QSettings>
#include <QCheckBox>
#include <QGridLayout>

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

signals:
    void stateChange();

private slots:
    void clearConsole();
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

    /*
     * statse display
    */
    void updateState();

private:
    Ui::MainWindow *ui;
    QSettings* settings;
    void printToConsole(QString);
//    void contextMenuEvent(QContextMenuEvent*);
    /*
     * tcp/ip connection
    */
    QTcpSocket* socket;
    void tcpInit();
    void sendMessage(QByteArray);
    void parse_data(QByteArray);

    /*
     * mode control
    */
    QButtonGroup* buttonGroup;
    void modeCtrlInit();

    /*
     * analog display
    */
    int maxDisplay;
    int currentNum;
    int xRange;
    QButtonGroup* checkGroup;
    QVector<QVector<QPointF>> analogData;
    QList<QGridLayout*> tab_layouts;


    void analogInit();
    QChartView* addNewChart(int);
    void refreshChart(int);

    /*
     * statse display
    */
    void stateInit();
    bool isConnected;
    bool isReset;
    bool isSelfchecked;
    enum ModeList {SELFCHECK,NORMAL,HIGHSPEED,RESET,SELFCHECK_EN,IDLE};
    ModeList currentMode;


};
#endif // MAINWINDOW_H
