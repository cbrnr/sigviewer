#include "data_block_tests.h"

#include "base/data_block.h"

namespace BioSig_
{

namespace Tests_
{

//-----------------------------------------------------------------------------
DataBlockTests::DataBlockTests ()
    : Test (tr("DataBlock"))
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
QString DataBlockTests::run ()
{
    DataBlock data_block;
    VERIFY (data_block.size() == 0, "empty");

    QSharedPointer<std::vector<float32> > ten_data (new std::vector<float32>);
    for (unsigned i = 1; i < 10; i++)
        ten_data->push_back (i);

    DataBlock ten_block (ten_data, 10);
    VERIFY (ten_block.size() == ten_data->size(), "created with 10 elements");

    return "";
}

}

}
