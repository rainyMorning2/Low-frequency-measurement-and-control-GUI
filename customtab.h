#ifndef CUSTOMTAB_H
#define CUSTOMTAB_H

#include <QTabBar>
#include <QStylePainter>
#include <QStyleOptionTab>

class CustomTab : public QTabBar
{
    Q_OBJECT
public:
    explicit CustomTab(QWidget *parent = nullptr,int num= 0);
    void setWarningState(int,bool);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    QVector<bool> hasWarning;

};

#endif // CUSTOMTAB_H
