#include "tab_context.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
TabContext::TabContext ()
    : selection_state_ (TAB_STATE_NO_EVENT_SELECTED),
      edit_state_ (TAB_STATE_READONLY)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
TabContext::~TabContext ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void TabContext::setSelectionState (TabSelectionState state)
{
    selection_state_ = state;
    emit selectionStateChanged (selection_state_);
}

//-----------------------------------------------------------------------------
void TabContext::setEditState (TabEditState state)
{
    edit_state_ = state;
    emit editStateChanged (edit_state_);
}


} // namespace BioSig_
