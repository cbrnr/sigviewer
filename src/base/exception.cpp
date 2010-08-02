#include "exception.h"

namespace SigViewer_
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
