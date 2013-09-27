// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


#ifndef SIGNAL_BROWSER_GRAPHICS_VIEW_H
#define SIGNAL_BROWSER_GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QPoint>
#include <QMouseEvent>

namespace SigViewer_
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
