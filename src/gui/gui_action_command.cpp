#include "gui_action_command.h"
#include "gui_action_factory.h"
#include "../application_context.h"

#include <cassert>

namespace BioSig_
{

//-----------------------------------------------------------------------------
GuiActionCommand::GuiActionCommand (QStringList const& action_ids)
{
    for (QStringList::const_iterator iter = action_ids.begin();
         iter != action_ids.end();
         ++iter)
    {
        action_map_[*iter] = new QAction (*iter, this);
        connectors_.push_back (new ActionConnector (this, *iter));
        assert (connectors_.last ()->connect (action_map_[*iter], SIGNAL(triggered()), SLOT(trigger())));
        assert (connect (connectors_.last (), SIGNAL(triggered(QString const&)), SLOT(trigger(QString const&))));
        assert (action_map_[*iter]->connect (this, SIGNAL(qActionEnabledChanged(bool)), SLOT(setEnabled (bool))));
        assert (connect (ApplicationContext::getInstance().data(), SIGNAL(stateChanged(ApplicationState)),
                          SLOT(updateEnablednessToApplicationState(ApplicationState))));
        assert (connect (ApplicationContext::getInstance().data(), SIGNAL(currentTabSelectionStateChanged(TabSelectionState)),
                          SLOT(updateEnablednessToTabSelectionState (TabSelectionState))));
    }
}

//-----------------------------------------------------------------------------
QList<QAction*> GuiActionCommand::getQActions ()
{
    return action_map_.values ();
}

//-----------------------------------------------------------------------------
QList<QString> GuiActionCommand::getActionIDs () const
{
    return action_map_.keys ();
}


//-----------------------------------------------------------------------------
QAction* GuiActionCommand::getQAction (QString const& id)
{
    if (action_map_.contains (id))
        return action_map_[id];
    else
        return 0;
}

//-----------------------------------------------------------------------------
void GuiActionCommand::updateEnablednessToApplicationState (ApplicationState state)
{
    app_state_ = state;
    applicationStateChanged ();
    evaluateEnabledness ();
}

//-----------------------------------------------------------------------------
void GuiActionCommand::updateEnablednessToTabSelectionState (TabSelectionState state)
{
    tab_sec_state_ = state;
    evaluateEnabledness ();
}


//-----------------------------------------------------------------------------
void GuiActionCommand::resetActionTriggerSlot (QString const& action_id,
                                              const char* slot)
{
    assert (action_map_.contains (action_id));
    QAction* action = action_map_[action_id];
    action->disconnect (SIGNAL(triggered()));
    assert (connect (action, SIGNAL(triggered()), slot));
}

}
