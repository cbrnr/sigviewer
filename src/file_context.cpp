#include "file_context.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
FileContext::FileContext ()
    : state_ (UNCHANGED)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
FileContext::~FileContext ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void FileContext::setState (State state)
{
    state_ = state;
    emit stateChanged (state_);
}

} // namespace BioSig_
