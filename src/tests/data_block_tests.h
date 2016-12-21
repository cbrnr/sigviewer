// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


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
