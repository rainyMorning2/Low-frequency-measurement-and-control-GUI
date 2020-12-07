#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QButtonGroup>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QSplineSeries>
#include <QSettings>
#include <QCheckBox>
#include <QSaveFile>
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
    QSettings* settings;
    void printToConsole(QString);

    /*
     * tcp/ip connection
    */
    QTcpSocket* socket;
    void tcpInit();
    void sendMessage(QString);

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
    QButtonGroup* checkGroup;
    QVector<QPointF> analogData[200];
    void analogInit();
    QChartView* addNewChart(int);
    QList<QGridLayout*> tab_layouts;

    /*
     * statse display
    */
};
#endif // MAINWINDOW_H
