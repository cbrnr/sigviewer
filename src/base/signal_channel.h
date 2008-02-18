// signal_channel.h

#ifndef SIGNAL_CHANNEL_H
#define SIGNAL_CHANNEL_H

#include "user_types.h"

#include <QString>

namespace BioSig_
{

// SignalChannel
class SignalChannel
{
public:

    // from GDF format
    enum Type
    {
        CHAR    = 0,
        INT8    = 1,
        UINT8   = 2,
        INT16   = 3,
        UINT16  = 4,
        INT32   = 5,
        UINT32  = 6,
        INT64   = 7,
        FLOAT32 = 16,
        FLOAT64 = 17,

        // add N to code
        BITN    = 255,
        UBITN   = 511
    };

    SignalChannel(uint32 number, const QString& label,
                  uint32 samples_per_record, const QString& physical_dim,
                  float64 physical_minimum, float64 physical_maximum,
                  float64 digital_minimum, float64 digital_maximum,
                  uint32 data_type, uint32 data_offset,
                  const QString filter_label, float64 lowpass,
                  float64 highpass, bool notch);

    uint32 typeBitSize() const;
    QString typeString() const;
    uint32 getNumber() const;
    const QString& getLabel() const;
    uint32 getSamplesPerRecord() const;
    float64 getScale() const;
    float64 getOffset() const;
    const QString& getFilterLabel() const;
    float64 getLowpass() const;
    float64 getHighpass() const;
    bool getNotch() const;
    const QString& getPhysicalDim() const;
    float64 getPhysicalMaximum() const;
    int64 getDigitalMaximum() const;
    float64 getPhysicalMinimum() const;
    int64 getDigitalMinimum() const;
    uint32 getDataType() const;
    uint32 getDataOffset() const;

private:
    // not allowed
    SignalChannel();

    uint32 number_;
    QString label_;
    uint32 samples_per_record_;
    QString physical_dim_;
    float64 physical_maximum_;
    float64 digital_maximum_;
    float64 physical_minimum_;
    float64 digital_minimum_;
    uint32 data_type_;
    uint32 data_offset_;
    float64 scale_;
    float64 offset_;
    QString filter_label_;
    float64 lowpass_;
    float64 highpass_;
    bool notch_;
};

} // namespace BioSig_

#endif
