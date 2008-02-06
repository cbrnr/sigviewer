// signal_data_block_queue.h

#ifndef SIGNAL_DATA_BLOCK_QUEUE_H
#define SIGNAL_DATA_BLOCK_QUEUE_H

#include "user_types.h"

#include <QVector>

namespace BioSig_
{

class SignalDataBlock;

// buffer for signals
class SignalDataBlockQueue
{
public:
    SignalDataBlockQueue(uint32 channel_nr, uint32 sampl_per_rec, 
                         uint32 recs_per_block, uint32 blocks,
                         uint32 sub_sampl = 1);
    virtual ~SignalDataBlockQueue();

    SignalDataBlock& getSignalDataBlock(uint32 block_nr, bool& valid);
    void setBlockValid(uint32 block_nr);

    uint32 getSubSampling() const;
    uint32 getChannelNumber() const;
    uint32 getSamplesPerRecord() const;
    uint32 getRecordsPerBlock() const;
    uint32 getNumberBlocks() const;

private:
    struct DataBlockEntry
    {
        SignalDataBlock* data_block;
        uint32 block_nr;
        bool valid;
    };

    typedef QVector<DataBlockEntry> DataBlockEntryVector;

    uint32 channel_number_;
    uint32 samples_per_record_; // without subsampling
    uint32 records_per_block_;
    uint32 number_blocks_;
    uint32 sub_sampling_;
    DataBlockEntryVector data_block_entrys_;
};

} // namespace BioSig_

#endif
