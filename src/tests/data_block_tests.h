#ifndef DATA_BLOCK_TESTS_H
#define DATA_BLOCK_TESTS_H

#include "test.h"

namespace SigViewer_
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
