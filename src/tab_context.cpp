#include "tab_context.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
TabContext::TabContext ()
    : state_ (NO_EVENTS_POSSIBLE)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
TabContext::~TabContext ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void TabContext::setState (TabContext::State state)
{
    state_ = state;
    emit stateChanged (state_);
}


} // namespace BioSig_
