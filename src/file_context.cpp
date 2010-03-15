#include "file_context.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
FileContext::FileContext ()
    : state_ (FILE_STATE_UNCHANGED)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
FileContext::~FileContext ()
{
    // nothing to do here
}

//-------------------------------------------------------------------------
FileState FileContext::getState () const
{
    return state_;
}


//-----------------------------------------------------------------------------
void FileContext::setState (FileState state)
{
    state_ = state;
    emit stateChanged (state_);
}

} // namespace BioSig_
