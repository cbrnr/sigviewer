#ifndef DATA_BLOCK_H
#define DATA_BLOCK_H

#include "user_types.h"

#include <list>
#include <vector>
#include <string>

namespace BioSig_ {

class DataBlock
{
public:
    //-------------------------------------------------------------------------
    DataBlock ();

    //-------------------------------------------------------------------------
    /// @param sample_rate_per_unit as a data_block must not represent data which
    ///                             is associated to time, the sample_rate is given
    ///                             in "per unit" (e.g. "s" or "hz", etc.)
    DataBlock (std::vector<float32> const &data, float32 sample_rate_per_unit);

    //-------------------------------------------------------------------------
    DataBlock (DataBlock const &src);

    //-------------------------------------------------------------------------
    void setData (std::vector<float32> const &data, float32 sample_rate_per_unit);

    //-------------------------------------------------------------------------
    std::vector<float32> const &getData () const;

    //-------------------------------------------------------------------------
    std::string getLabel () const;

    //-------------------------------------------------------------------------
    void setLabel (std::string const &label);

    //-------------------------------------------------------------------------
    /// sets the x-unit label (e.g. "hz" or "s")
    void setXUnitLabel (std::string const &unit_label);

    //-------------------------------------------------------------------------
    std::string getXUnitLabel () const;

    //-------------------------------------------------------------------------
    /// sets the y-unit label (e.g. "hz" or "s")
    void setYUnitLabel (std::string const &unit_label);

    //-------------------------------------------------------------------------
    std::string getYUnitLabel () const;


    //-------------------------------------------------------------------------
    float32 getSampleRatePerUnit () const;


    //-------------------------------------------------------------------------
    float32 getMin () const;

    //-------------------------------------------------------------------------
    float32 getMax () const;

    //-------------------------------------------------------------------------
    DataBlock getBandpassFilteredBlock (float32 lower_hz_boundary, float32 upper_hz_boundary) const;

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
    // static unsigned getLengthOfSmallestBlock (std::list<DataBlock> const &data_blocks);

    std::vector<float32> data_;
    float32 sample_rate_per_unit_;
    std::string label_;
    std::string x_unit_label_;
    std::string y_unit_label_;
};

}

#endif // DATA_BLOCK_H
