#include "event_context_menu.h"
#include "signal_browser_model_4.h"
#include "../application_context.h"
#include "../gui_action_manager.h"


#include <QGraphicsSceneContextMenuEvent>
#include <QAction>

namespace BioSig_
{

//-----------------------------------------------------------------------------
EventContextMenu::EventContextMenu (SignalBrowserModel& model,
                                    ApplicationContext& app_context)
 : signal_browser_model_ (model),
   app_context_ (app_context)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
EventContextMenu::~EventContextMenu ()
{
    event_items_.clear();
    event_item_type_names_.clear();
    sub_menus_.clear();
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
        QAction* title = addAction (*it_name);
        title->setEnabled (false);
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
    menu.addActions (app_context_.getGUIActionManager().getActionsOfGroup(GUIActionManager::EVENT_CONTEXT_ACTIONS));
}



}
