#include "event_context_menu.h"
#include "signal_browser_model_4.h"
#include "../main_window_model.h"


#include <QGraphicsSceneContextMenuEvent>
#include <QAction>

namespace BioSig_
{

//-----------------------------------------------------------------------------
EventContextMenu::EventContextMenu (SignalBrowserModel& model)
 : signal_browser_model_ (model)
{
    edit_to_all_channels_icon_.addFile(":/images/to_all_channels_22x22.png");
    edit_copy_to_channels_icon_.addFile(":/images/copy_to_channels_22x22.png");
    edit_delete_icon_.addFile(":/images/delete_22x22.png");
    edit_change_channel_icon_.addFile(":/images/change_channel_22x22.png");
    edit_change_type_icon_.addFile(":/images/change_type_22x22.png");
}

//-----------------------------------------------------------------------------
EventContextMenu::~EventContextMenu ()
{

}

//-------------------------------------------------------------------------
void EventContextMenu::addEvent (QSharedPointer<EventGraphicsItem> event_item,
                                 QString const &type_name)
{
    QVector<QMenu*>::iterator it = sub_menus_.begin();
    while (it != sub_menus_.end())
    {
        (*it)->clear();
        delete (*it);
        ++it;
    }
    sub_menus_.clear();

    clear();
    QObject::disconnect(this, 0);
    event_items_.append (event_item);
    event_item_type_names_.append(type_name);
}

//-------------------------------------------------------------------------
unsigned EventContextMenu::getNumberOfEvents () const
{
    return event_items_.size();
}


//-------------------------------------------------------------------------
void EventContextMenu::finaliseAndShowContextMenu (QGraphicsSceneContextMenuEvent* context_event)
{
    QVector<QSharedPointer<EventGraphicsItem> >::iterator it = event_items_.begin();
    QVector<QString>::iterator it_name = event_item_type_names_.begin();
    if (event_items_.size() > 1)
    {
        while (it != event_items_.end())
        {
            QString text ("...");
            if (it_name != event_item_type_names_.end())
                text = *it_name;
            QMenu* submenu = new QMenu (text, this);
            sub_menus_.append(submenu);

            // context-menu actions
            addActionsToMenu (*submenu);

            QAction* action = addMenu (submenu);
            action->activate(QAction::Hover);
            action->setData((*it)->getId());
            addAction(action);
            ++it;
            ++it_name;
        }
    }
    else if (event_items_.size() == 1)
    {
        addActionsToMenu (*this);
        signal_browser_model_.selectEvent ((*it)->getId());
    }

    event_items_.clear();
    event_item_type_names_.clear();
    QObject::connect(this, SIGNAL(hovered(QAction*)), this, SLOT(selectEvent(QAction*)));
    exec (context_event->screenPos());
}

//-------------------------------------------------------------------------
void EventContextMenu::finaliseAndShowSelectionMenu (QGraphicsSceneMouseEvent* event)
{
    QVector<QSharedPointer<EventGraphicsItem> >::iterator it = event_items_.begin();
    QVector<QString>::iterator it_name = event_item_type_names_.begin();
    if (event_items_.size() > 1)
    {
        while (it != event_items_.end())
        {
            QString text ("...");
            if (it_name != event_item_type_names_.end())
                text = *it_name;

            QAction* action = addAction(text);
            //action->activate(QAction::Hover);
            action->setData((*it)->getId());
            //addAction(action);
            ++it;
            ++it_name;
        }
        QObject::connect(this, SIGNAL(triggered(QAction*)), this, SLOT(selectEvent(QAction*)));
        exec (event->screenPos());
    }
    else if (event_items_.size() == 1)
        signal_browser_model_.selectEvent ((*it)->getId());

    event_items_.clear();
    event_item_type_names_.clear();
}


//-------------------------------------------------------------------------
void EventContextMenu::selectEvent (QAction* q)
{
    bool ok = false;
    int32 event_id = q->data().toInt(&ok);
    if (q->data().isValid())
        signal_browser_model_.selectEvent (event_id);
}

//-------------------------------------------------------------------------
void EventContextMenu::addActionsToMenu (QMenu& menu)
{
    QAction* delete_action = menu.addAction(edit_delete_icon_, "Delete");
    delete_action->setObjectName("delete_action");
    delete_action->setStatusTip(tr("Delete the selected event"));
    QObject::connect(delete_action, SIGNAL(triggered()),
        &(signal_browser_model_.getMainWindowModel()), SLOT(editDeleteAction()));

    QAction* to_all_channels_action = menu.addAction(edit_to_all_channels_icon_, "To all channels");
    to_all_channels_action->setObjectName("to_all_channels_action");
    to_all_channels_action->setStatusTip(tr("Selected event to all channels"));
    QObject::connect(to_all_channels_action, SIGNAL(triggered()),
            &(signal_browser_model_.getMainWindowModel()), SLOT(editToAllChannelsAction()));

    QAction* edit_change_type_action = menu.addAction(edit_change_type_icon_, tr("Change &Type..."));
    edit_change_type_action->setObjectName("edit_change_type_action");
    edit_change_type_action->setStatusTip(tr("Change the type of the selected event"));
    QObject::connect(edit_change_type_action, SIGNAL(triggered()),
            &(signal_browser_model_.getMainWindowModel()), SLOT(editChangeTypeAction()));

    QAction* edit_change_channel_action = menu.addAction(edit_change_channel_icon_,
                                              tr("Change Cha&nnel..."));
    edit_change_channel_action->setObjectName("edit_change_channel_action_");
    edit_change_channel_action
        ->setStatusTip(tr("Change the channel of the selected event (\"Edit Events\"-Mode: Shift+LeftMousePress)"));
    QObject::connect(edit_change_channel_action, SIGNAL(triggered()),
            &(signal_browser_model_.getMainWindowModel()), SLOT(editChangeChannelAction()));

    menu.addSeparator();

    QAction* hide_events_of_other_type_action = menu.addAction(tr("Hide Events of other Type"));
    hide_events_of_other_type_action->setObjectName("view_show_events_of_selected_type_action_");
    hide_events_of_other_type_action
        ->setStatusTip(tr("Hide events which are not of the same type as the selected event."));
    QObject::connect(hide_events_of_other_type_action, SIGNAL(triggered()),
            &(signal_browser_model_.getMainWindowModel()), SLOT(viewShowEventsOfSelectedTypeAction()));

    QAction* fit_view_action = menu.addAction(tr("Fit View"));
    fit_view_action->setObjectName("view_fit_to_event_action_");
    fit_view_action
        ->setStatusTip(tr("Fit view to selected event."));
    QObject::connect(fit_view_action, SIGNAL(triggered()),
            &(signal_browser_model_.getMainWindowModel()), SLOT(viewFitToEventAction()));


}



}
