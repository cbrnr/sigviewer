// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <string>


namespace sigviewer
{

class Exception : public std::exception
{
public:
    Exception (std::string const& what) throw ();
    virtual ~Exception () throw () {}

    virtual const char* what() const throw();
private:
    std::string what_;
};

}

#endif // EXCEPTION_H
