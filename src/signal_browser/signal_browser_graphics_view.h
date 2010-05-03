#ifndef SIGNAL_BROWSER_GRAPHICS_VIEW_H
#define SIGNAL_BROWSER_GRAPHICS_VIEW_H

#include <QGraphicsView>

namespace BioSig_
{

class SignalBrowserGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
     SignalBrowserGraphicsView (QGraphicsScene* scene, QWidget* parent = 0)
         : QGraphicsView (scene, parent) {}

protected:
    virtual void resizeEvent(QResizeEvent* event) {emit resized (event);}

signals:
    void resized (QResizeEvent*);
};

}

#endif // SIGNAL_BROWSER_GRAPHICS_VIEW_H
