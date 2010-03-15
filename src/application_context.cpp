#include "application_context.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
ApplicationContext::ApplicationContext (GUIActionManager& gui_action_manager)
    : gui_action_manager_ (gui_action_manager)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
GUIActionManager& ApplicationContext::getGUIActionManager ()
{
    return gui_action_manager_;
}

//-----------------------------------------------------------------------------
ApplicationState ApplicationContext::getState () const
{
    return state_;
}

//-----------------------------------------------------------------------------
void ApplicationContext::setState (ApplicationState state)
{
    state_ = state;
    emit stateChanged (state_);
}




} // namespace BioSig_
