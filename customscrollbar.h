#ifndef CUSTOMSCROLLBAR_H
#define CUSTOMSCROLLBAR_H

#include <QScrollBar>

class customScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    customScrollBar(QWidget *parent,int id);

signals:
    void customValueChanged(int value,int id);

private:
    int id;

private slots:
    void parentSigRev(int value);
};

#endif // CUSTOMSCROLLBAR_H
