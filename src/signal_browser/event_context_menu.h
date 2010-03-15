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
class ApplicationContext;

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
    EventContextMenu (SignalBrowserModel& model,
                      ApplicationContext& app_context);

    //-------------------------------------------------------------------------
    virtual ~EventContextMenu ();

    //-------------------------------------------------------------------------
    void addEvent (QSharedPointer<EventGraphicsItem> event_item,
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

    QVector<QSharedPointer<EventGraphicsItem> > event_items_;
    QVector<QString> event_item_type_names_;
    QVector<QMenu*> sub_menus_;
    SignalBrowserModel& signal_browser_model_;
    ApplicationContext& app_context_;

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
