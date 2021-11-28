#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QWidget>

namespace Ui {
class settingWindow;
}

class settingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit settingWindow(QWidget *parent = nullptr);
    ~settingWindow();

signals:
    void send_sig();

private slots:
    void on_checkBox_da_toggled(bool checked);

    void on_checkBox_key_toggled(bool checked);

    void on_checkBox_rs422_toggled(bool checked);

    void on_pushButton_selectAll_clicked();

    void on_pushButton_selectReverse_clicked();

    void on_pushButton_fill_clicked();

    void on_pushButton_clear_clicked();

    void rs422_mode(int,bool);

    void on_pushButton_apply_clicked();

    void on_pushButton_send_clicked();

    void on_pushButton_select1_clicked();

    void on_pushButton_clicked();

private:
    Ui::settingWindow *ui;
};

#endif // SETTINGWINDOW_H
