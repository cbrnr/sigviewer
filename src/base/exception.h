#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <string>

namespace SigViewer_
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
