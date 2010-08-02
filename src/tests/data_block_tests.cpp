#include "data_block_tests.h"

#include "base/data_block.h"

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
    for (unsigned i = 1; i <= 10; i++)
        ten_data->push_back (i);

    QSharedPointer<DataBlock const> ten_block1 (new DataBlock (ten_data, 10));
    QSharedPointer<DataBlock const> ten_block2 (new DataBlock (ten_data, 10));

    std::list<QSharedPointer<DataBlock const> > blocks;
    blocks.push_back (ten_block1);
    blocks.push_back (ten_block2);

    DataBlock mean = DataBlock::calculateMean (blocks);
    VERIFY (mean.size() == 10, "mean number of elements")
    for (unsigned x = 0; x < 10; x++)
        VERIFY (mean[x] == (x + 1), "mean checking elements")

    QSharedPointer<DataBlock> std_dev = DataBlock::calculateStandardDeviation (blocks);
    VERIFY (std_dev->size() == 10, "std-dev number of elements")
    for (unsigned x = 0; x < 10; x++)
        VERIFY ((*std_dev)[x] == 0, "std-dev checking elements")


    return "";
}

}

}
