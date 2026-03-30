// © SigViewer developers
//
// License: GPL-3.0


#include "exception.h"

namespace sigviewer
{

//-----------------------------------------------------------------------------
Exception::Exception (std::string const& what) throw () :
        what_ (what)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
const char* Exception::what() const throw()
{
    return what_.c_str ();
}

}
