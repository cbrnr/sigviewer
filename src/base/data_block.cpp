#include "data_block.h"

#include <algorithm>
#include <cmath>

namespace BioSig_ {

//-----------------------------------------------------------------------------
DataBlock::DataBlock()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
DataBlock::DataBlock (std::vector<float32> const &data)
    : data_ (data)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
DataBlock::DataBlock (DataBlock const &src)
    : data_ (src.data_)
{
    // nothing to do here
}


//-----------------------------------------------------------------------------
void DataBlock::setData (std::vector<float32> const &data)
{
    data_ = data;
}

//-----------------------------------------------------------------------------
std::vector<float32> const &DataBlock::getData () const
{
    return data_;
}

//-------------------------------------------------------------------------
std::string DataBlock::getLabel () const
{
    return label_;
}

//-------------------------------------------------------------------------
void DataBlock::setLabel (std::string const &label)
{
    label_ = label;
}

//-----------------------------------------------------------------------------
float32 DataBlock::getMin () const
{
    std::vector<float32>::const_iterator min_element = std::min_element (data_.begin(), data_.end());
    if (min_element != data_.end())
        return *min_element;
    else
        return 0;
}

//-----------------------------------------------------------------------------
float32 DataBlock::getMax () const
{
    std::vector<float32>::const_iterator max_element = std::max_element (data_.begin(), data_.end());
    if (max_element != data_.end())
        return *max_element;
    else
        return 0;
}


//-----------------------------------------------------------------------------
DataBlock DataBlock::calculateMean (std::list<DataBlock> const &data_blocks)
{
    DataBlock mean_block;
    if (data_blocks.size() == 0)
        return mean_block;

    std::list<DataBlock>::const_iterator it = data_blocks.begin();
    float32 tmp_mean = 0;
    for (unsigned index = 0; index < (*(data_blocks.begin())).data_.size(); index++)
    {
        it = data_blocks.begin();
        tmp_mean = 0;
        while (it != data_blocks.end())
        {
            tmp_mean += it->data_[index];
            ++it;
        }
        mean_block.data_.push_back(tmp_mean / data_blocks.size());
    }
    return mean_block;
}

//-------------------------------------------------------------------------
DataBlock DataBlock::calculateStandardDeviation (std::list<DataBlock> const &data_blocks)
{
    return calculateStandardDeviationImpl (data_blocks, calculateMean(data_blocks));
}

//-------------------------------------------------------------------------
DataBlock DataBlock::calculateStandardDeviation (std::list<DataBlock> const &data_blocks,
                                                 DataBlock const &means)
{
    return calculateStandardDeviationImpl (data_blocks, means);
}

//-------------------------------------------------------------------------
DataBlock DataBlock::calculateStandardDeviationImpl (std::list<DataBlock> const &data_blocks,
                                                     DataBlock const &means)
{
    DataBlock stddev_block;
    if (data_blocks.size() == 0)
        return stddev_block;

    std::list<DataBlock>::const_iterator it = data_blocks.begin();
    float32 tmp_stddev = 0;
    for (unsigned index = 0; index < (*(data_blocks.begin())).data_.size(); index++)
    {
        it = data_blocks.begin();
        tmp_stddev = 0;
        float32 mean = means.getData()[index];
        while (it != data_blocks.end())
        {
            tmp_stddev += pow((it->data_[index] - mean), 2);
            ++it;
        }
        stddev_block.data_.push_back(sqrt(tmp_stddev / data_blocks.size()));
    }
    return stddev_block;
}

//-------------------------------------------------------------------------
unsigned DataBlock::getLengthOfSmallestBlock (std::list<DataBlock> const &data_blocks)
{
    std::list<DataBlock>::const_iterator it = data_blocks.begin();
    //for (it = data_blocks.begin(); it != data_blocks.end(); ++it)
}



}
