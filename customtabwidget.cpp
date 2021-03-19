#include "customtabwidget.h"
#include<QDebug>
CustomTabWidget::CustomTabWidget(QWidget *parent,int num) : QTabWidget(parent)
{
    tb = new CustomTab(this,num);
    setTabBar(tb);
}

void CustomTabWidget::setWaringState(int index, bool state){
    tb->setWarningState(index,state);
}
