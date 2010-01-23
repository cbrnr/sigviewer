#ifndef FREQUENCY_SPECTRUM_PROCESSOR_H
#define FREQUENCY_SPECTRUM_PROCESSOR_H

#include "signal_processor.h"

namespace BioSig_
{

class FrequencySpectrumProcessor : public SignalProcessor
{
public:
    FrequencySpectrumProcessor();

    virtual QSharedPointer<DataBlock> processData (QSharedPointer<const DataBlock> data) const;
};

}

#endif // FREQUENCY_SPECTRUM_PROCESSOR_H
