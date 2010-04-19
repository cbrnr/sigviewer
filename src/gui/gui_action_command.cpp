#include "gui_action_command.h"
#include "gui_action_factory.h"
#include "../application_context.h"

namespace BioSig_
{

//-------------------------------------------------------------------------
GuiActionCommand::GuiActionCommand (QString const& name)
        : action_ (new QAction (name, this))
{
    Q_ASSERT(connect (action_, SIGNAL(triggered()), SLOT(trigger())));
    Q_ASSERT(action_->connect (this, SIGNAL(qActionEnabledChanged(bool)), SLOT(setEnabled (bool))));
    Q_ASSERT(connect (ApplicationContext::getInstance().data(), SIGNAL(stateChanged(ApplicationState)),
                      SLOT(applicationStateChanged(ApplicationState))));
}

//-------------------------------------------------------------------------
QAction* GuiActionCommand::getQAction ()
{
    return action_;
}

}
