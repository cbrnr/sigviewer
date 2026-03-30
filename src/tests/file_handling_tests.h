// © SigViewer developers
//
// License: GPL-3.0


#ifndef FILE_HANDLING_TESTS_H
#define FILE_HANDLING_TESTS_H

#include "test.h"

namespace sigviewer
{
namespace Tests_
{

class FileHandlingTests : public Test
{
public:
    FileHandlingTests() : Test (tr("File Handling Tests")) {}
    virtual void init ();
    virtual QString run ();
    virtual void cleanup ();
private:

    QString testEventExportingImporting ();

};

}

}

#endif // FILE_HANDLING_TESTS_H
