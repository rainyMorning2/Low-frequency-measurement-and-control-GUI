#ifndef CUSTOMTEXT_H
#define CUSTOMTEXT_H
#include <QTextBrowser>
#include <QMenu>
#include <QContextMenuEvent>

class customText : public QTextBrowser
{
    Q_OBJECT
public:
    explicit customText(QWidget *parent = nullptr);
    customText();

private slots:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif // CUSTOMTEXT_H
