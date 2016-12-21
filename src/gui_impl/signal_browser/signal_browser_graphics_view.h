// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef SIGNAL_BROWSER_GRAPHICS_VIEW_H
#define SIGNAL_BROWSER_GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QPoint>
#include <QMouseEvent>

namespace sigviewer
{

class SignalBrowserGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
     SignalBrowserGraphicsView (QGraphicsScene* scene, QWidget* parent = 0)
         : QGraphicsView (scene, parent) {}

protected:
    virtual void resizeEvent (QResizeEvent* event) {emit resized (event);}
    //virtual void mouseMoveEvent (QMouseEvent* event) {this->moveEvent(event);}//event->ignore();}//emit sceneMouseMoved (mapToScene (event->pos()));}
signals:
    void resized (QResizeEvent*);
    void sceneMouseMoved (QPointF scene_pos);
};

}

#endif // SIGNAL_BROWSER_GRAPHICS_VIEW_H
