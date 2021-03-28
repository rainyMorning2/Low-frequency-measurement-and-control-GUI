#include "customtext.h"

customText::customText(QWidget *parent): QTextBrowser(parent)
{

}

void customText::contextMenuEvent(QContextMenuEvent *event){
    QMenu *menu = createStandardContextMenu();
    QAction *myAction = new QAction("Clear All",menu);
    connect(myAction,SIGNAL(triggered()),this,SLOT(clear()));
    menu->actions()[1]->setVisible(false);

    if(document()->isEmpty()){
        myAction->setEnabled(false);
    }else{
        myAction->setEnabled(true);
    }

    menu->addAction(myAction);
    menu->exec(event->globalPos());
    delete menu;
}
