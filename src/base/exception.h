// © SigViewer developers
//
// License: GPL-3.0

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <string>

namespace sigviewer {

class Exception : public std::exception {
   public:
    Exception(std::string const& what) throw();
    virtual ~Exception() throw() {}

    virtual const char* what() const throw();

   private:
    std::string what_;
};

}  // namespace sigviewer

#endif  // EXCEPTION_H
