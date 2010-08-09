#ifndef FILE_HANDLING_TESTS_H
#define FILE_HANDLING_TESTS_H

#include "test.h"

namespace SigViewer_
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
