#ifndef DATA_BLOCK_H
#define DATA_BLOCK_H

#include "sigviewer_user_types.h"

#include <list>
#include <vector>
#include <string>

#include <QSharedPointer>
#include <QMap>

namespace SigViewer_ {

//-------------------------------------------------------------------------
/// DataBlock
///
/// basic class to store channel data and sections of it
class DataBlock
{
public:
    //-------------------------------------------------------------------------
    virtual ~DataBlock ();

    //-------------------------------------------------------------------------
    virtual QSharedPointer<DataBlock> createSubBlock (uint32 start, uint32 length) const = 0;

    //-------------------------------------------------------------------------
    virtual float32 const& operator[] (uint32 index) const = 0;

    //-------------------------------------------------------------------------
    virtual float32 getMin () const = 0;

    //-------------------------------------------------------------------------
    virtual float32 getMax () const = 0;

    //-------------------------------------------------------------------------
    /// length of the block
    uint32 size () const;

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

protected:
    // protected constructors here:
    DataBlock (unsigned length, float32 sample_rate_per_unit);
    DataBlock (DataBlock const& src, unsigned new_length);

private:
    uint32 length_;
    float32 sample_rate_per_unit_;

    std::string label_;
    std::string x_unit_label_;
    std::string y_unit_label_;

    static unsigned instance_count_;
};

}

#endif // DATA_BLOCK_H
