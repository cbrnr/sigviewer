#ifndef SIGNAL_PROCESSOR_H
#define SIGNAL_PROCESSOR_H

#include "../base/data_block.h"

#include <QSharedPointer>

namespace BioSig_
{

//-----------------------------------------------------------------------------
///
/// SignalProcessor
///
/// abstract base class for all signal processing units
class SignalProcessor
{
public:
    SignalProcessor ();

    virtual QSharedPointer<DataBlock> processData (QSharedPointer<const DataBlock> data) const = 0;
};

}

#endif // SIGNAL_PROCESSOR_H
