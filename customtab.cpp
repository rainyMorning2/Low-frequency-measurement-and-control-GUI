#include "customtab.h"
#include<QDebug>
CustomTab::CustomTab(QWidget *parent,int num) : QTabBar(parent)
{
    for(int i=0;i<num;i++){
        hasWarning.append(false);
    }
    setAutoFillBackground(true);
}

void CustomTab::setWarningState(int index,bool state){
    hasWarning[index] = state;
    update();
}

void CustomTab::paintEvent(QPaintEvent *event){
    QStylePainter painter(this);
    QStyleOptionTab opt;

    for(int index = 0; index < count(); index++)
    {
        initStyleOption(&opt,index);
        /*Here make the changes*/
        if(hasWarning[index]){
            opt.palette.setColor(QPalette::Button,QColor("#F9F400"));
        }
        painter.drawControl(QStyle::CE_TabBarTabShape, opt);
        painter.drawControl(QStyle::CE_TabBarTabLabel,opt);
    }

}
