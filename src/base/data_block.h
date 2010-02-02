#ifndef DATA_BLOCK_H
#define DATA_BLOCK_H

#include "user_types.h"

#include <list>
#include <vector>

namespace BioSig_ {

class DataBlock
{
public:
    //-------------------------------------------------------------------------
    DataBlock ();

    //-------------------------------------------------------------------------
    DataBlock (std::vector<float32> const &data);

    //-------------------------------------------------------------------------
    DataBlock (DataBlock const &src);

    //-------------------------------------------------------------------------
    void setData (std::vector<float32> const &data);

    //-------------------------------------------------------------------------
    std::vector<float32> const &getData () const;

    //-------------------------------------------------------------------------
    float32 getMin () const;

    //-------------------------------------------------------------------------
    float32 getMax () const;

    //-------------------------------------------------------------------------
    static DataBlock calculateMean (std::list<DataBlock> const &data_blocks);

    //-------------------------------------------------------------------------
    static DataBlock calculateStandardDeviation (std::list<DataBlock> const &data_blocks);

    //-------------------------------------------------------------------------
    static DataBlock calculateStandardDeviation (std::list<DataBlock> const &data_blocks,
                                                 DataBlock const &means);


private:

    //-------------------------------------------------------------------------
    static DataBlock calculateStandardDeviationImpl (std::list<DataBlock> const &data_blocks,
                                                     DataBlock const &means);

    //-------------------------------------------------------------------------
    static unsigned getLengthOfSmallestBlock (std::list<DataBlock> const &data_blocks);

    std::vector<float32> data_;

};

}

#endif // DATA_BLOCK_H
