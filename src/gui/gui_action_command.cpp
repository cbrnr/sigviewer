#include "gui_action_command.h"
#include "gui_action_factory.h"

namespace BioSig_
{

//-------------------------------------------------------------------------
GuiActionCommand::GuiActionCommand (QString const& name)
        : action_ (new QAction (name, this))
{
    Q_ASSERT(connect (action_, SIGNAL(triggered()), SLOT(trigger())));
    Q_ASSERT(action_->connect (this, SIGNAL(qActionEnabledChanged(bool)), SLOT(setEnabled (bool))));
}

//-------------------------------------------------------------------------
QAction* GuiActionCommand::getQAction ()
{
    return action_;
}

}
