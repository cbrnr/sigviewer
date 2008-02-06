// signal_data_block_queue.cpp

#include "signal_data_block_queue.h"
#include "signal_data_block.h"

namespace BioSig_
{

// constructor
SignalDataBlockQueue::SignalDataBlockQueue(uint32 channel_nr,
                                           uint32 sampl_per_rec,
                                           uint32 recs_per_block, uint32 blocks,
                                           uint32 sub_sampl)
: channel_number_(channel_nr),
  samples_per_record_(sampl_per_rec),
  records_per_block_(recs_per_block),
  number_blocks_(blocks),
  sub_sampling_(sub_sampl),
  data_block_entrys_(blocks)
{
    for (uint32 block = 0; block < blocks; block++)
    {
        DataBlockEntry& entry = data_block_entrys_[block];
        entry.data_block 
            = new SignalDataBlock(sub_sampl, channel_nr, sampl_per_rec *
                                                         recs_per_block);
        entry.valid = false;
        entry.block_nr = 0xFFFFFFFF; // invalid block nr
    }
}

// destructor
SignalDataBlockQueue::~SignalDataBlockQueue()
{
    for (uint32 block = 0; block < number_blocks_; block++)
    {
        delete data_block_entrys_[block].data_block;
    }
}

// get subsampling
uint32 SignalDataBlockQueue::getSubSampling() const
{
    return sub_sampling_;
}

// get channel number
uint32 SignalDataBlockQueue::getChannelNumber() const
{
    return channel_number_;
}

// get records per block
uint32 SignalDataBlockQueue::getSamplesPerRecord() const
{
    return samples_per_record_;
}

// get records per block
uint32 SignalDataBlockQueue::getRecordsPerBlock() const
{
    return records_per_block_;
}

// get number of blocks
uint32 SignalDataBlockQueue::getNumberBlocks() const
{
    return number_blocks_;
}

// get signal data block
SignalDataBlock& SignalDataBlockQueue::getSignalDataBlock(uint32 block_nr,
                                                          bool& valid)
{
    DataBlockEntry& entry = data_block_entrys_[block_nr % number_blocks_];
    if (entry.block_nr != block_nr)
    {
        entry.block_nr = block_nr;
        entry.valid = false;
    }
    valid = entry.valid;
    return *entry.data_block;
}

// set block valid
void SignalDataBlockQueue::setBlockValid(uint32 block_nr)
{
    DataBlockEntry& entry = data_block_entrys_[block_nr % number_blocks_];
    if (entry.block_nr == block_nr)
    {
        entry.valid = true;
    }
}

} // namespace BioSig_
