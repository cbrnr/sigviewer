// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef DATA_BLOCK_H
#define DATA_BLOCK_H

#include "sigviewer_user_types.h"

#include <list>
#include <vector>
#include <string>

#include <QSharedPointer>
#include <QMap>


namespace sigviewer {

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
    virtual QSharedPointer<DataBlock> createSubBlock(size_t start, size_t length) const = 0;

    //-------------------------------------------------------------------------
    virtual float32 const& operator[] (size_t index) const = 0;

    //-------------------------------------------------------------------------
    virtual float32 getMin () const = 0;

    //-------------------------------------------------------------------------
    virtual float32 getMax () const = 0;

    //-------------------------------------------------------------------------
    /// length of the block
    size_t size () const;

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
    float64 getSampleRatePerUnit () const;

protected:
    // protected constructors here:
    DataBlock (size_t length, float64 sample_rate_per_unit);
    DataBlock (DataBlock const& src, size_t new_length);

private:
    size_t length_;
    float64 sample_rate_per_unit_;

    std::string label_;
    std::string x_unit_label_;
    std::string y_unit_label_;

    static size_t instance_count_;
};

}

#endif // DATA_BLOCK_H
