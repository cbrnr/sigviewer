// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


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
