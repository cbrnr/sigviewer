// © SigViewer developers
//
// License: GPL-3.0


#ifndef DATA_BLOCK_TESTS_H
#define DATA_BLOCK_TESTS_H

#include "test.h"

namespace sigviewer
{
namespace Tests_
{

//-----------------------------------------------------------------------------
/// DataBlockTests
/// runs tests on the DataBlock class
class DataBlockTests : public Test
{
public:
    DataBlockTests ();
    virtual QString run ();

private:
    QString testMean ();
};
}
}

#endif // DATA_BLOCK_TESTS_H
