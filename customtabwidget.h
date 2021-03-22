#ifndef CUSTOMTABWIDGET_H
#define CUSTOMTABWIDGET_H

#include <QTabWidget>
#include <customtab.h>

class CustomTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit CustomTabWidget(QWidget *parent = nullptr,int num=0);

    void setWaringState(int,bool);

private:
    CustomTab* tb;

};

#endif // CUSTOMTABWIDGET_H
