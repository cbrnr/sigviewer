#include "gui_action_command.h"
#include "gui_action_factory.h"
#include "../application_context.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
GuiActionCommand::GuiActionCommand (QStringList const& action_titles)
{
    for (QStringList::const_iterator iter = action_titles.begin();
         iter != action_titles.end();
         ++iter)
    {
        actions_.push_back (new QAction (*iter, this));
        connectors_.push_back (new ActionConnector (this, *iter));
        Q_ASSERT(connectors_.last ()->connect (actions_.last(), SIGNAL(triggered()), SLOT(trigger())));
        Q_ASSERT(connect (connectors_.last (), SIGNAL(triggered(QString const&)), SLOT(trigger(QString const&))));
        Q_ASSERT(actions_.last()->connect (this, SIGNAL(qActionEnabledChanged(bool)), SLOT(setEnabled (bool))));
        Q_ASSERT(connect (ApplicationContext::getInstance().data(), SIGNAL(stateChanged(ApplicationState)),
                          SLOT(applicationStateChanged(ApplicationState))));
    }
}

//-----------------------------------------------------------------------------
QList<QAction*> GuiActionCommand::getQActions ()
{
    return actions_;
}

//-----------------------------------------------------------------------------
QAction* GuiActionCommand::getQAction (QString const& name)
{
    for (QList<QAction*>::iterator action_iter = actions_.begin();
         action_iter != actions_.end ();
         ++action_iter)
    {
        if ((*action_iter)->text () == name)
            return *action_iter;
    }
    return 0;
}



}
