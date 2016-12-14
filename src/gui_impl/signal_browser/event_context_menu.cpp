#include "event_context_menu.h"
#include "signal_browser_model_4.h"
#include "gui/gui_action_factory.h"

#include <QGraphicsSceneContextMenuEvent>
#include <QAction>
#include <QDebug>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
EventContextMenu::EventContextMenu (SignalVisualisationModel& browser_model,
                                    QSharedPointer<EventManager> event_manager)
    : browser_model_ (browser_model),
      event_manager_ (event_manager)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
EventContextMenu::~EventContextMenu ()
{

}

//-------------------------------------------------------------------------
void EventContextMenu::addEvent (EventID event)
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
    event_ids_.append (event);
}

//-------------------------------------------------------------------------
unsigned EventContextMenu::getNumberOfEvents () const
{
    return event_ids_.size();
}


//-------------------------------------------------------------------------
void EventContextMenu::finaliseAndShowContextMenu (QGraphicsSceneContextMenuEvent* context_event,
                                                   QMenu* channel_menu)
{
    QVector<EventID>::iterator it = event_ids_.begin();
    if (event_ids_.size() > 1)
    {
        while (it != event_ids_.end())
        {
            QString text (event_manager_->getNameOfEvent(*it));
            QMenu* submenu = new QMenu (text);
            sub_menus_.append(submenu);

            // context-menu actions
            addActionsToMenu (*submenu, *it);

            QAction* action = addMenu (submenu);
            action->activate(QAction::Hover);
            action->setData(*it);
            addAction(action);
            ++it;
        }
    }
    else if (event_ids_.size() == 1)
    {
        QAction* title = addAction (event_manager_->getNameOfEvent(*it));
        title->setEnabled (false);
        addActionsToMenu (*this, *(event_ids_.begin ()));
        browser_model_.selectEvent (*it);
    }

    event_ids_.clear();
    QObject::connect(this, SIGNAL(hovered(QAction*)), this, SLOT(selectEvent(QAction*)));
    if (channel_menu)
    {
        addSeparator ();
        addMenu (channel_menu);
    }

    exec (context_event->screenPos());
}

//-------------------------------------------------------------------------
void EventContextMenu::finaliseAndShowSelectionMenu (QGraphicsSceneMouseEvent* event)
{
    QVector<EventID>::iterator it = event_ids_.begin();
    if (event_ids_.size() > 1)
    {
        while (it != event_ids_.end())
        {
            QString text (event_manager_->getNameOfEvent (*it));
            QAction* action = addAction(text);
            //action->activate(QAction::Hover);
            action->setData(*it);
            //addAction(action);
            ++it;
        }
        QObject::connect(this, SIGNAL(triggered(QAction*)), this, SLOT(selectEvent(QAction*)));
        exec (event->screenPos());
    }
    else if (event_ids_.size() == 1)
    {
        browser_model_.selectEvent (*it);
    }

    event_ids_.clear();
}


//-------------------------------------------------------------------------
void EventContextMenu::selectEvent (QAction* q)
{
    bool ok = false;
    int32 event_id = q->data().toInt(&ok);
    if (q->data().isValid())
        browser_model_.selectEvent (event_id);
}

//-------------------------------------------------------------------------
void EventContextMenu::addActionsToMenu (QMenu& menu, EventID event)
{
    QList<QAction*> actions;

    if (event_manager_->getEvent(event)->getChannel() != UNDEFINED_CHANNEL)
    {
        actions.append (GuiActionFactory::getInstance()->getQAction("To all Channels"));
        actions.append (GuiActionFactory::getInstance()->getQAction("Copy to Channels..."));
    }
    actions.append (GuiActionFactory::getInstance()->getQAction("Delete"));
    actions.append (GuiActionFactory::getInstance()->getQAction("Change Channel..."));
    actions.append (GuiActionFactory::getInstance()->getQAction("Change Type..."));
    actions.append (new QAction (this));
    actions.last()->setSeparator(true);
    actions.append (GuiActionFactory::getInstance()->getQAction("Insert Over"));
    actions.append (new QAction (this));
    actions.last()->setSeparator(true);
    if (event_manager_->getNextEventOfSameType (event) != UNDEFINED_EVENT_ID)
        actions.append (GuiActionFactory::getInstance()->getQAction("Goto and Select Next Event"));
    if (event_manager_->getPreviousEventOfSameType (event) != UNDEFINED_EVENT_ID)
        actions.append (GuiActionFactory::getInstance()->getQAction("Goto and Select Previous Event"));
    if (!actions.last()->isSeparator())
    {
        actions.append (new QAction (this));
        actions.last()->setSeparator(true);
    }
    if (browser_model_.getShownEventTypes().size() > 1)
        actions.append (GuiActionFactory::getInstance()->getQAction("Hide Events of other Type"));
    else
        actions.append (GuiActionFactory::getInstance()->getQAction("Show all Events"));
    actions.append (GuiActionFactory::getInstance()->getQAction("Fit View to Selected Event"));

    foreach (QAction* action, actions)
        menu.addAction (action);
}



}
