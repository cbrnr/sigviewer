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


#ifndef EVENT_CONTEXT_MENU_H
#define EVENT_CONTEXT_MENU_H

#include "event_graphics_item.h"
#include "file_handling/event_manager.h"
#include "gui/signal_visualisation_model.h"

#include <QVector>
#include <QSharedPointer>
#include <QMenu>
#include <QObject>
#include <QIcon>

namespace sigviewer
{

//-----------------------------------------------------------------------------
///
/// EventContextMenu
///
/// enables editing of events... allows selection of events


class EventContextMenu : public QMenu
{
    Q_OBJECT

public:
    //-------------------------------------------------------------------------
    EventContextMenu (SignalVisualisationModel& browser_model,
                      QSharedPointer<EventManager> event_manager);

    //-------------------------------------------------------------------------
    virtual ~EventContextMenu ();

    //-------------------------------------------------------------------------
    void addEvent (EventID event);

    //-------------------------------------------------------------------------
    unsigned getNumberOfEvents () const;

    //-------------------------------------------------------------------------
    void finaliseAndShowContextMenu (QGraphicsSceneContextMenuEvent* context_event,
                                     QMenu* channel_menu = 0);

    //-------------------------------------------------------------------------
    void finaliseAndShowSelectionMenu (QGraphicsSceneMouseEvent* context_event);


private:
    //-------------------------------------------------------------------------
    void addActionsToMenu (QMenu& menu, EventID event);

    QVector<EventID> event_ids_;
    QVector<QMenu*> sub_menus_;
    SignalVisualisationModel& browser_model_;
    QSharedPointer<EventManager> event_manager_;

    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    EventContextMenu (EventContextMenu const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    EventContextMenu& operator= (EventContextMenu const &);

//signals:
//    void hovered (QAction* q);
public slots:
    void selectEvent (QAction* q);

};

}

#endif // EVENT_CONTEXT_MENU_H
