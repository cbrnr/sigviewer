#include "application_context.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
ApplicationContext::ApplicationContext (GUIActionManager& gui_action_manager,
                                        MainWindowModel& main_window_model)
    : gui_action_manager_ (gui_action_manager),
      main_window_model_ (main_window_model)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
GUIActionManager& ApplicationContext::getGUIActionManager ()
{
    return gui_action_manager_;
}

//-----------------------------------------------------------------------------
MainWindowModel& ApplicationContext::getMainWindowModel ()
{
    return main_window_model_;
}

//-----------------------------------------------------------------------------
void ApplicationContext::setState (State state)
{
    state_ = state;
    emit stateChanged (state_);
}




} // namespace BioSig_
