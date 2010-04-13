#ifndef EVENT_CONTEXT_MENU_H
#define EVENT_CONTEXT_MENU_H

#include "event_graphics_item.h"

#include <QVector>
#include <QSharedPointer>
#include <QMenu>
#include <QObject>
#include <QIcon>

namespace BioSig_
{

class SignalBrowserModel;

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
    EventContextMenu (SignalBrowserModel& model);

    //-------------------------------------------------------------------------
    virtual ~EventContextMenu ();

    //-------------------------------------------------------------------------
    void addEvent (uint16 event_id,
                   QString const &type_name);

    //-------------------------------------------------------------------------
    unsigned getNumberOfEvents () const;

    //-------------------------------------------------------------------------
    void finaliseAndShowContextMenu (QGraphicsSceneContextMenuEvent* context_event);

    //-------------------------------------------------------------------------
    void finaliseAndShowSelectionMenu (QGraphicsSceneMouseEvent* context_event);


private:
    //-------------------------------------------------------------------------
    void addActionsToMenu (QMenu& menu);

    QVector<uint16> event_ids_;
    QVector<QString> event_item_type_names_;
    QVector<QMenu*> sub_menus_;
    SignalBrowserModel& signal_browser_model_;

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
