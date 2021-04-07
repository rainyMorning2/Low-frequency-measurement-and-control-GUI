#include "customscrollbar.h"

customScrollBar::customScrollBar(QWidget *parent, int id) : QScrollBar(parent){
    this->id = id;
    connect(this,SIGNAL(valueChanged(int)),this,SLOT(parentSigRev(int)));
}


void customScrollBar::parentSigRev(int value){
    emit customValueChanged(value,id);
}

