#include "file_context.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
FileContext::FileContext (EventManagerInterface& event_manager,
                          TabContext& tab_context)
    : state_ (FILE_STATE_UNCHANGED),
      event_manager_ (event_manager),
      tab_context_ (tab_context)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
FileContext::~FileContext ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
EventManagerInterface& FileContext::getEventManager ()
{
    return event_manager_;
}


//-------------------------------------------------------------------------
FileState FileContext::getState () const
{
    return state_;
}

//-------------------------------------------------------------------------
TabContext& FileContext::getMainTabContext ()
{
    return tab_context_;
}

//-----------------------------------------------------------------------------
void FileContext::setState (FileState state)
{
    state_ = state;
    emit stateChanged (state_);
}

} // namespace BioSig_
