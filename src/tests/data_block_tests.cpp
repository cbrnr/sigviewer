#include "data_block_tests.h"
#include "base/data_block.h"

#include <cmath>

namespace SigViewer_
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
    for (unsigned i = 1; i <= 10; i++)
        ten_data->push_back (i);

    DataBlock ten_block (ten_data, 10);
    VERIFY (ten_block.size() == ten_data->size(), "created with 10 elements");
    VERIFY (ten_block.getMax() == 10, "get max");
    VERIFY (ten_block.getMin() == 1, "get min");
    for (unsigned i = 1; i <= 10; i++)
        VERIFY (ten_block[i-1] == i, "check content");

    QString error = testMean ();
    if (error.size() > 0)
        return error;

    return "";
}

//-----------------------------------------------------------------------------
QString DataBlockTests::testMean ()
{
    QSharedPointer<std::vector<float32> > ten_data (new std::vector<float32>);
    QSharedPointer<std::vector<float32> > ten_reverse_data (new std::vector<float32>);
    for (unsigned i = 1; i <= 10; i++)
    {
        ten_data->push_back (i);
        ten_reverse_data->push_back (11-i);
    }
    QSharedPointer<DataBlock const> ten_block1 (new DataBlock (ten_data, 10));
    QSharedPointer<DataBlock const> ten_block2 (new DataBlock (ten_data, 10));
    QSharedPointer<DataBlock const> ten_block_revers (new DataBlock (ten_reverse_data, 10));

    std::list<QSharedPointer<DataBlock const> > blocks;
    blocks.push_back (ten_block1);
    blocks.push_back (ten_block2);

    // Test 1
    DataBlock mean = DataBlock::calculateMean (blocks);
    VERIFY (mean.size() == 10, "mean number of elements")
    for (unsigned x = 0; x < 10; x++)
        VERIFY (mean[x] == (x + 1), "mean checking elements")

    QSharedPointer<DataBlock> std_dev = DataBlock::calculateStandardDeviation (blocks);
    VERIFY (std_dev->size() == 10, "std-dev number of elements")
    for (unsigned x = 0; x < 10; x++)
        VERIFY ((*std_dev)[x] == 0, "std-dev checking elements")

    // Test 2
    blocks.pop_back ();
    blocks.push_back (ten_block_revers);
    DataBlock mean_revers = DataBlock::calculateMean (blocks);
    VERIFY (mean_revers.size() == 10, "mean 2 number of elements")
    for (unsigned x = 0; x < 10; x++)
        VERIFY ((mean_revers[x] == 5.5f), "mean 2 checking elements")

    QSharedPointer<DataBlock> std_dev_revers = DataBlock::calculateStandardDeviation (blocks);
    VERIFY (std_dev_revers->size() == 10, "std-dev 2 number of elements")
    for (unsigned x = 0; x < 10; x++)
        VERIFY (((*std_dev_revers)[x]) == sqrt((pow((*ten_block1)[x] - 5.5f, 2) + pow((*ten_block_revers)[x] - 5.5f, 2)) / blocks.size()), "std-dev 2 checking elements")

    return "";
}

}

}
