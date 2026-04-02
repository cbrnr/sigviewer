// © SigViewer developers
//
// License: GPL-3.0

#ifndef SIGNAL_BROWSER_GRAPHICS_VIEW_H
#define SIGNAL_BROWSER_GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QPoint>

namespace sigviewer {

class SignalBrowserGraphicsView : public QGraphicsView {
    Q_OBJECT
   public:
    SignalBrowserGraphicsView(QGraphicsScene* scene, QWidget* parent = 0)
        : QGraphicsView(scene, parent) {}

   protected:
    virtual void resizeEvent(QResizeEvent* event) { emit resized(event); }
    // virtual void mouseMoveEvent (QMouseEvent* event) {this->moveEvent(event);}//event->ignore();}//emit sceneMouseMoved (mapToScene (event->pos()));}
   signals:
    void resized(QResizeEvent*);
    void sceneMouseMoved(QPointF scene_pos);
};

}  // namespace sigviewer

#endif  // SIGNAL_BROWSER_GRAPHICS_VIEW_H
