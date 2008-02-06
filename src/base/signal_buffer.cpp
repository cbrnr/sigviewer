// signal_buffer.cpp

#include "file_signal_reader.h"
#include "signal_buffer.h"
#include "signal_data_block.h"

#include <QTextStream>

namespace BioSig_
{

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// constructor
SignalBuffer::SignalBuffer(FileSignalReader& reader)
 : signal_reader_(reader),
   log_stream_(0),
   state_(STATE_READY),
   next_event_id_(0)
{
    float64 rec_duration = signal_reader_.getRecordDuration();
    records_per_block_ = max((uint32)(1.0 / rec_duration + 0.5), 1);
    if (records_per_block_ % 2 != 0)
    {
        records_per_block_++;
    }
    block_duration_ = records_per_block_ * rec_duration;
    uint32 number_records = signal_reader_.getNumberRecords();
    number_blocks_ = (number_records + records_per_block_ - 1) /
                     records_per_block_;
}

// destructor
SignalBuffer::~SignalBuffer()
{
    Int2SubBuffersPtrMap::iterator buffer_iter;
    for (buffer_iter = channel_nr2sub_buffers_map_.begin();
         buffer_iter != channel_nr2sub_buffers_map_.end();
         buffer_iter++)
    {
        delete buffer_iter.value();
    }
    Int2SignalEventPtrMap::iterator event_iter;
    for (event_iter = id2signal_events_map_.begin();
         event_iter != id2signal_events_map_.end();
         event_iter++)
    {
        delete event_iter.value();
    }

}

// set log stream
void SignalBuffer::setLogStream(QTextStream* log_stream)
{
    log_stream_ = log_stream;
}

// init Done
void SignalBuffer::initDone(uint32, InitStatus)
{
    // nothing
}

// get block duration
float64 SignalBuffer::getBlockDuration() const
{
    return block_duration_;
}

// get number of blocks
uint32 SignalBuffer::getNumberBlocks() const
{
    return number_blocks_;
}

// get records per block
uint32 SignalBuffer::getRecordsPerBlock() const
{
    return records_per_block_;
}

// add channel
void SignalBuffer::addChannel(uint32 channel_nr)
{
    if (!checkReadyState("addChannel"))
    {
        return;
    }

    Int2SubBuffersPtrMap::iterator iter =
        channel_nr2sub_buffers_map_.find(channel_nr);
    if (iter != channel_nr2sub_buffers_map_.end())
    {
        iter.value()->active = true;
        return; // channel already buffered
    }

    uint32 samples_per_record = signal_reader_.getChannel(channel_nr)
                                    .getSamplesPerRecord();
    uint32 samples = signal_reader_.getNumberRecords() * samples_per_record;
    uint32 block_size = records_per_block_ * samples_per_record;

    SubBuffers* sub_buffers = new SubBuffers();
    sub_buffers->active = true;
    sub_buffers->min_max_calculated_ = false;
    sub_buffers->min_value_ = 0.0;
    sub_buffers->max_value_ = 0.0;
    for (uint32 sub = NO_SUBSAMPLING; sub <= MAX_SUBSAMPLING; sub++)
    {
        uint32 blocks;
        blocks = sub == WHOLE_SUBSAMPLING ? samples / (block_size << sub)
                                          : min(BUFFER_QUEUE_SIZE / block_size,
                                             samples / (block_size << sub));
        blocks = blocks < 2 ? 2 : blocks;
        sub_buffers->sub_queue[sub].reset(
            new SignalDataBlockQueue(channel_nr, samples_per_record,
                                     records_per_block_, blocks, 1 << sub));
    }
    channel_nr2sub_buffers_map_[channel_nr] = sub_buffers;
}

// remove channel
void SignalBuffer::removeChannel(uint32 channel_nr)
{
    if (!checkReadyState("removeChannel"))
    {
        return;
    }

    Int2SubBuffersPtrMap::iterator iter =
        channel_nr2sub_buffers_map_.find(channel_nr);
    if (iter == channel_nr2sub_buffers_map_.end())
    {
        return; // channel not buffered
    }
    delete iter.value();
    channel_nr2sub_buffers_map_.erase(iter);
}

// is channel buffered
bool SignalBuffer::isChannelBuffered(uint32 channel_nr) const
{
    Int2SubBuffersPtrMap::const_iterator iter =
        channel_nr2sub_buffers_map_.find(channel_nr);
    return iter != channel_nr2sub_buffers_map_.end();
}

// set channel active
void SignalBuffer::setChannelActive(uint32 channel_nr, bool active)
{
    if (!checkReadyState("setChannelActive"))
    {
        return;
    }

    Int2SubBuffersPtrMap::iterator iter =
        channel_nr2sub_buffers_map_.find(channel_nr);
    if (iter == channel_nr2sub_buffers_map_.end())
    {
        return; // channel not buffered
    }
    iter.value()->active = active;
}

// is channel active
bool SignalBuffer::isChannelActive(uint32 channel_nr) const
{
    if (!checkReadyState("isChannelActive"))
    {
        return false;
    }

    Int2SubBuffersPtrMap::const_iterator iter =
        channel_nr2sub_buffers_map_.find(channel_nr);
    if (iter == channel_nr2sub_buffers_map_.end())
    {
        return false ; // channel not buffered
    }
    return iter.value()->active;
}

// init buffer
void SignalBuffer::init()
{
    if (!checkReadyState("init"))
    {
        return;
    }

    // first ative sub buffers
    Int2SubBuffersPtrMap::iterator iter = channel_nr2sub_buffers_map_.begin();
    while (iter != channel_nr2sub_buffers_map_.end() && !iter.value()->active)
    {
        iter++;
    }

    // loads signals
    if (iter != channel_nr2sub_buffers_map_.end())
    {
        // load all whole subsampling blocks
        uint32 blocks = iter.value()->sub_queue[WHOLE_SUBSAMPLING]
                                                            ->getNumberBlocks();
        uint32 step = max(blocks / 50, 1);
        for (uint32 block_nr = 0; block_nr < blocks; block_nr++)
        {
            if (block_nr % step == 0)
            {
                initDone((block_nr + 1) * 100 / blocks, INIT_DOWNSAMPLE);
            }
            getSignalDataBlockImpl(iter.key(), WHOLE_SUBSAMPLING, block_nr);
        }

        // calculate min and max value
        int32 number_channels = channel_nr2sub_buffers_map_.size();
        for (iter = channel_nr2sub_buffers_map_.begin();
             iter != channel_nr2sub_buffers_map_.end();
             iter++)
        {
            SubBuffers* sub_buffers = iter.value();
            int32 channel_nr = iter.key();
            initDone((channel_nr + 1) * 100 / number_channels, INIT_MIN_MAX);
            if (sub_buffers->min_max_calculated_ || !sub_buffers->active)
            {
                continue;
            }
            for (uint32 block_nr = 0; block_nr < blocks; block_nr++)
            {
                bool valid;
                SignalDataBlock* data_block;
                data_block = &channel_nr2sub_buffers_map_[channel_nr]
                                ->sub_queue[WHOLE_SUBSAMPLING]
                                    ->getSignalDataBlock(block_nr, valid);
                if (!valid)
                {
                    if (log_stream_)
                    {
                        *log_stream_ << "SignalBuffer::init Error: "
                                     << "downsampling failed\n";
                    }
                    continue;
                }
                float64 min = data_block->getMinValue();
                float64 max = data_block->getMaxValue();
                sub_buffers->min_value_ = sub_buffers->min_value_ > min ?
                                            min : sub_buffers->min_value_;
                sub_buffers->max_value_ = sub_buffers->max_value_ < max ?
                                            max : sub_buffers->max_value_;
            }
            sub_buffers->min_max_calculated_ = true;
        }
    }

    // load events
    if (next_event_id_ == 0)
    {
        SignalEventVector signal_events;
        signal_reader_.loadEvents(signal_events);
        uint32 number_events = signal_events.size();
        uint32 step = max(number_events / 50, 1);
        FileSignalReader::SignalEventVector::iterator iter;
        uint32 event_nr = 0;
        for (iter = signal_events.begin();
             iter != signal_events.end();
             iter++, event_nr++)
        {
            if (event_nr % step == 0)
            {
                initDone((event_nr + 1) * 100 / number_events, INIT_EVENTS);
            }
            if (iter->getType() & SignalEvent::EVENT_END)
            {
                if (log_stream_)
                {
                    *log_stream_ << "SignalBuffer::init Error: End-Events not "
                                 << "suported\n";
                }
                continue;
            }
            id2signal_events_map_[next_event_id_] 
                = new SignalEvent(*iter, next_event_id_);
            next_event_id_++;
        }
    }
}

// get number events
uint32 SignalBuffer::getNumberEvents() const
{
    if (!checkReadyState("getNumberEvents"))
    {
        return 0;
    }
    return id2signal_events_map_.size();
}

// event number to ID
int32 SignalBuffer::eventNumber2ID(uint32 event_number) const
{
    if (!checkReadyState("eventNumber2ID"))
    {
        return -1;
    }
    if ((int32)event_number >= id2signal_events_map_.size())
    {
        return -1; // invalid event number
    }

    Int2SignalEventPtrMap::const_iterator iter = id2signal_events_map_.begin();
    for (uint32 nr = 0; nr < event_number; nr++, iter++);
    return iter.key();
}

// get event
SignalEvent* SignalBuffer::getEvent(uint32 event_id)
{
    if (!checkReadyState("getEvent"))
    {
        return 0;
    }
    Int2SignalEventPtrMap::iterator iter = id2signal_events_map_.find(event_id);
    if (iter == id2signal_events_map_.end())
    {
        return 0; // invalid event_id
    }
    return iter.value();
}

// add event
int32 SignalBuffer::addEvent(const SignalEvent& event)
{
    if (!checkReadyState("addEvent"))
    {
        return -1;
    }
    id2signal_events_map_[next_event_id_] 
        = new SignalEvent(event, next_event_id_);
    return next_event_id_++;
}

// remove event
void SignalBuffer::removeEvent(uint32 event_id)
{
    Int2SignalEventPtrMap::iterator iter = id2signal_events_map_.find(event_id);
    if (iter == id2signal_events_map_.end())
    {
        return;
    }
    delete iter.value();
    id2signal_events_map_.erase(iter);
}

// get event samplerate
uint32 SignalBuffer::getEventSamplerate() const
{
    return signal_reader_.getEventSamplerate();
}

// get signal data block
SignalDataBlock* SignalBuffer::getSignalDataBlock(uint32 channel_nr,
                                                  uint32 sub_sampl,
                                                  uint32 block_nr)
{
    if (!checkReadyState("getSignalDataBlock"))
    {
        return 0;
    }

    return getSignalDataBlockImpl(channel_nr, sub_sampl, block_nr);
}


// get signal data block implementation
SignalDataBlock* SignalBuffer::getSignalDataBlockImpl(uint32 channel_nr,
                                                      uint32 sub_sampl,
                                                      uint32 block_nr)
{
    if (channel_nr2sub_buffers_map_.find(channel_nr) ==
        channel_nr2sub_buffers_map_.end())
    {
        return 0; // illegal channel number
    }

    // set first blocks interesting
    Int2SubBuffersPtrMap::iterator channel_iter;
    for (channel_iter = channel_nr2sub_buffers_map_.begin();
         channel_iter != channel_nr2sub_buffers_map_.end();
         channel_iter++)
    {
        if (!channel_iter.value()->active)
        {
            continue; // channel not active
        }
        channel_iter.value()->next_data_block_nr = 0;
    }

    // all possible iteresting blocks
    FileSignalReader::SignalDataBlockPtrVector
        to_load_data_blocks(channel_nr2sub_buffers_map_.size());
    for (uint32 no_sub_block_nr = block_nr << sub_sampl;
         no_sub_block_nr < (block_nr + 1) << sub_sampl;
         no_sub_block_nr++)
    {
        to_load_data_blocks.clear();
        for (channel_iter = channel_nr2sub_buffers_map_.begin();
             channel_iter != channel_nr2sub_buffers_map_.end();
             channel_iter++)
        {
            SubBuffers* sub_buffers = channel_iter.value();
            if (!sub_buffers->active)
            {
                continue; // channel not active
            }
            if (sub_buffers->next_data_block_nr > no_sub_block_nr)
            {
                continue; // block not interesting
            }

            // search valid blocks
            int32 sub;
            uint32 sub_block_nr;
            for (sub = sub_sampl; sub >= NO_SUBSAMPLING; sub--)
            {
                sub_block_nr = no_sub_block_nr >> sub;
                bool valid;
                sub_buffers->actual_data_block[sub] =
                    &sub_buffers->sub_queue[sub]
                        ->getSignalDataBlock(sub_block_nr, valid);
                if (valid)
                {
                    break; // void valid block
                }
            }

            if (sub < NO_SUBSAMPLING)
            {
                // no valid block found
                SignalDataBlock* data_block;
                data_block = sub_buffers->actual_data_block[NO_SUBSAMPLING];
                to_load_data_blocks.push_back(data_block);
                data_block->clearMinMax();
            }
            else
            {
                // set next interesting block
                sub_buffers->next_data_block_nr = (sub_block_nr + 1) << sub;

                // downsampling
                for (uint32 up_sub = sub;
                     up_sub < sub_sampl;
                     up_sub++, sub_block_nr /= 2)
                {
                    uint32 part = sub_block_nr % 2;
                    sub_buffers->actual_data_block[up_sub]
                        ->downsample(*sub_buffers
                                        ->actual_data_block[up_sub + 1], part);
                    if (part == 0)
                    {
                        break; // more downsampling not possible at moment
                    }
                    sub_buffers->sub_queue[up_sub + 1]
                                        ->setBlockValid(sub_block_nr / 2);
                }
            }
        }

        // load blocks
        signal_reader_.loadSignals(to_load_data_blocks.begin(),
                                   to_load_data_blocks.end(),
                                   no_sub_block_nr * records_per_block_);

        // downsample loaded blocks
        for (channel_iter = channel_nr2sub_buffers_map_.begin();
             channel_iter != channel_nr2sub_buffers_map_.end();
             channel_iter++)
        {
            SubBuffers* sub_buffers = channel_iter.value();
            if (!sub_buffers->active)
            {
                continue; // channel not active
            }
            if (sub_buffers->next_data_block_nr > no_sub_block_nr)
            {
                continue; // not loaded
            }

            // set loaded block valid
            sub_buffers->sub_queue[NO_SUBSAMPLING]
                ->setBlockValid(no_sub_block_nr);

            uint32 sub_block_nr = no_sub_block_nr;
            for (uint32 up_sub = NO_SUBSAMPLING;
                 up_sub < sub_sampl;
                 up_sub++, sub_block_nr /= 2)
            {
                uint32 part = sub_block_nr % 2;
                sub_buffers->actual_data_block[up_sub]
                    ->downsample(*sub_buffers->actual_data_block[up_sub + 1],
                                 part);
                if (part == 0)
                {
                    break;  // more downsampling not possible at moment
                }
                sub_buffers->sub_queue[up_sub + 1]
                    ->setBlockValid(sub_block_nr / 2);
            }
        }
    }

    bool valid;
    return &channel_nr2sub_buffers_map_[channel_nr]
                ->sub_queue[sub_sampl]->getSignalDataBlock(block_nr, valid);
}

// get min value
float64 SignalBuffer::getMinValue(uint32 channel_nr) const
{
    if (!checkReadyState("getMinValue"))
    {
        return 10E20;
    }

    Int2SubBuffersPtrMap::const_iterator iter;
    iter = channel_nr2sub_buffers_map_.find(channel_nr);
    if ( iter != channel_nr2sub_buffers_map_.end())
    {
        return iter.value()->min_value_;
    }
    return 0.0;
}

// get max value
float64 SignalBuffer::getMaxValue(uint32 channel_nr) const
{
    if (!checkReadyState("getMaxValue"))
    {
        return -10E20;
    }

    Int2SubBuffersPtrMap::const_iterator iter;
    iter = channel_nr2sub_buffers_map_.find(channel_nr);
    if ( iter != channel_nr2sub_buffers_map_.end())
    {
        return iter.value()->max_value_;
    }
    return 0.0;
}

// check ready state
bool SignalBuffer::checkReadyState(const QString& function) const
{
    if (state_ != STATE_READY)
    {
        if (log_stream_)
        {
            *log_stream_ << "SignalBuffer::" << function
                         << " Error: illegal state\n";
        }
        return false;
    }
    return true;
}

} // namespace BioSig_
