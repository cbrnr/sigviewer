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

namespace SigViewer_
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
