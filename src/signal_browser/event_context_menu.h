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

namespace PortingToQT4_
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
    void addEvent (QSharedPointer<EventGraphicsItem> event_item);

    //-------------------------------------------------------------------------
    void finaliseAndShowMenu (QGraphicsSceneContextMenuEvent* context_event);


private:
    //-------------------------------------------------------------------------
    void addActionsToMenu (QMenu& menu);

    QVector<QSharedPointer<EventGraphicsItem> > event_items_;
    QVector<QMenu*> sub_menus_;
    SignalBrowserModel& signal_browser_model_;

    QIcon edit_to_all_channels_icon_;
    QIcon edit_copy_to_channels_icon_;
    QIcon edit_delete_icon_;
    QIcon edit_change_channel_icon_;
    QIcon edit_change_type_icon_;


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

}

#endif // EVENT_CONTEXT_MENU_H
