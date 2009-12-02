/*

    $Id: signal_buffer.cpp,v 1.10 2009/03/06 22:29:18 brunnert Exp $
    Copyright (C) Thomas Brunner  2006,2007
    Copyright (C) Christoph Eibel 2007,2008,
    Copyright (C) Clemens Brunner 2006,2007,2008
    Copyright (C) Alois Schloegl  2008,2009
    This file is part of the "SigViewer" repository
    at http://biosig.sf.net/

    SigViewer is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

// signal_buffer.cpp

#include "file_signal_reader.h"
#include "signal_buffer.h"
#include "signal_data_block.h"

#include <QTextStream>
#include <QMutexLocker>


namespace BioSig_
{

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// constructor
SignalBuffer::SignalBuffer(FileSignalReader& reader)
 : signal_reader_(reader),
   log_stream_(0),
   state_(STATE_READY),
   next_event_id_(0),
   whole_buffer_(WHOLE_BUFFER_256),
   max_sub_sampling_(0),
   downsampled_(false),
   minmax_found_(false),
   do_init_downsample_(true),
   do_init_minmax_search_(true),
   default_min_(-100),
   default_max_(100)
{
    float64 rec_duration = signal_reader_.getBasicHeader()->getRecordDuration();
    records_per_block_ = max((uint32)(1.0 / rec_duration + 0.5), 1);
    if (records_per_block_ % 2 != 0)
    {
        records_per_block_++;
    }
    block_duration_ = records_per_block_ * rec_duration;
    uint32 number_records = signal_reader_.getBasicHeader()->getNumberRecords();
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

//    SharedPointer must not be deleted by hand!
//
//    Int2SignalEventPtrMap::iterator event_iter;
//    for (event_iter = id2signal_events_map_.begin();
//         event_iter != id2signal_events_map_.end();
//         event_iter++)
//    {
//        delete event_iter.value();
//    }

}

// set log stream
void SignalBuffer::setLogStream(QTextStream* log_stream)
{
    log_stream_ = log_stream;
}

void SignalBuffer::setWholeDataBuffer(WHOLE_BUFFER whole_buffer)
{
    whole_buffer_ = whole_buffer;
}

void SignalBuffer::enableInitDownsampling(bool enabled)
{
	do_init_downsample_ = enabled;
}

void SignalBuffer::enableInitMinMaxSearch(bool enabled)
{
    do_init_minmax_search_ = enabled;
}

void SignalBuffer::setDefaultRange(float32 min, float32 max)
{
    default_min_ = min;
    default_max_ = max;
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

// get max subsampling
uint32 SignalBuffer::getMaxSubsampling() const
{
    return max_sub_sampling_;
}

// add channel
void SignalBuffer::addChannel(uint32 channel_nr)
{
    QMutexLocker lock (&mutex_);
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

    uint32 samples_per_record = signal_reader_.getBasicHeader()->getChannel(channel_nr)
                                    .getSamplesPerRecord();
    uint32 samples = signal_reader_.getBasicHeader()->getNumberRecords() * samples_per_record;
    uint32 block_size = records_per_block_ * samples_per_record;

    SubBuffers* sub_buffers = new SubBuffers();
    sub_buffers->active = true;
    sub_buffers->min_max_calculated_ = false;
    sub_buffers->min_value_ = 0.0;
    sub_buffers->max_value_ = 0.0;
    bool bGotWholeBuffer = false;
    for (uint32 sub = NO_SUBSAMPLING; sub <= MAX_SUBSAMPLING; sub++)
    {
        uint32 blocks;
        if (whole_buffer_ != NO_WHOLE_BUFFER
            && sub > NO_SUBSAMPLING && sub % whole_buffer_ == 0)
        {
            blocks = samples / (block_size << sub);
            if (blocks > 0)
                bGotWholeBuffer = true;
        }
        else
        {
            blocks = min(BUFFER_QUEUE_SIZE / block_size,
                         samples / (block_size << sub));
        }
        if (blocks == 0)
            break;
        sub_buffers->sub_queue[sub].reset(
            new SignalDataBlockQueue(channel_nr, samples_per_record,
                                     records_per_block_, blocks, 1 << sub));
        max_sub_sampling_ = sub;
    }
    channel_nr2sub_buffers_map_[channel_nr] = sub_buffers;
    if (!bGotWholeBuffer)
        whole_buffer_ = NO_WHOLE_BUFFER;
}

// remove channel
void SignalBuffer::removeChannel(uint32 channel_nr)
{
    QMutexLocker lock (&mutex_);
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
    QMutexLocker lock (&mutex_);
    Int2SubBuffersPtrMap::const_iterator iter =
        channel_nr2sub_buffers_map_.find(channel_nr);
    return iter != channel_nr2sub_buffers_map_.end();
}

// set channel active
void SignalBuffer::setChannelActive(uint32 channel_nr, bool active)
{
    QMutexLocker lock (&mutex_);
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
    QMutexLocker lock (&mutex_);
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

void SignalBuffer::initLoadEvents()
{
    if (next_event_id_ != 0)
        return;

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
            = QSharedPointer<SignalEvent>(new SignalEvent(*iter, next_event_id_));
        next_event_id_++;
    }
}

void SignalBuffer::initDownsample()
{
    if (downsampled_ || whole_buffer_ ==  NO_WHOLE_BUFFER)
        return;

    // first active sub buffers
    Int2SubBuffersPtrMap::iterator iter = channel_nr2sub_buffers_map_.begin();
    while (!iter.value()->active)
    {
        iter++;
        if (iter == channel_nr2sub_buffers_map_.end())
            return;
    }

    uint32 blocks = 0;
    for (uint32 sub = whole_buffer_;
         sub <= max_sub_sampling_;
         sub += max(whole_buffer_,1))
    {
        // load all whole subsampling blocks
        blocks = iter.value()->sub_queue[sub]->getNumberBlocks();
        uint32 step = max(blocks / 50, 1);
        for (uint32 block_nr = 0; block_nr < blocks; block_nr++)
        {
            if (block_nr % step == 0)
            {
                initDone((block_nr + 1) * 100 / blocks, INIT_DOWNSAMPLE);
            }
            getSignalDataBlockImpl(iter.key(), sub, block_nr);
        }
    }
    downsampled_ = true;
}

void SignalBuffer::initMinMax()
{
    if (minmax_found_)
        return;

    uint32 samples_per_record = signal_reader_.getBasicHeader()->getChannel(0)
                                    .getSamplesPerRecord();
    uint32 samples = signal_reader_.getBasicHeader()->getNumberRecords() * samples_per_record;
    uint32 block_size = records_per_block_ * samples_per_record;
    uint32 sub = downsampled_ ? (max_sub_sampling_ / whole_buffer_)
                                 * whole_buffer_
                              : static_cast<uint32>(SUBSAMPLING_2);
    uint32 blocks = samples / (block_size << sub);

    // calculate min and max value
    uint32 step = max(blocks / 50, 1);
    for (uint32 block_nr = 0; block_nr < blocks; block_nr++)
    {
        if (block_nr % step == 0)
        {
            initDone((block_nr + 1) * 100 / blocks, INIT_MIN_MAX);
        }

        Int2SubBuffersPtrMap::iterator iter;
        for (iter = channel_nr2sub_buffers_map_.begin();
             iter != channel_nr2sub_buffers_map_.end();
             iter++)
        {
            SubBuffers* sub_buffers = iter.value();
            int32 channel_nr = iter.key();
            if (/*sub_buffers->min_max_calculated_ ||*/ !sub_buffers->active)
            {
                continue;
            }

            bool valid;
            SignalDataBlock* data_block;
            if (downsampled_)
            {
                data_block = &channel_nr2sub_buffers_map_[channel_nr]
                                ->sub_queue[sub]
                                    ->getSignalDataBlock(block_nr, valid);
            }
            else
            {
                data_block = getSignalDataBlockImpl(iter.key(), sub, block_nr);
                valid = true;
            }
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

            if (sub_buffers->min_value_ > min) 
	            sub_buffers->min_value_ = min; 
            if (sub_buffers->max_value_ < max) 
	            sub_buffers->max_value_ = max; 
            sub_buffers->min_max_calculated_ = (block_nr == blocks - 1);
        }
    }
    minmax_found_ = true;
}

// init buffer
void SignalBuffer::init()
{
    QMutexLocker lock (&mutex_);
    if (!checkReadyState("init"))
    {
        return;
    }

    initLoadEvents();

    if (do_init_downsample_)
        initDownsample();

    if (do_init_minmax_search_)
        initMinMax();
    else
    {
        Int2SubBuffersPtrMap::iterator iter;
        for (iter = channel_nr2sub_buffers_map_.begin();
             iter != channel_nr2sub_buffers_map_.end();
             iter++)
        {
            SubBuffers* sub_buffers = iter.value();
            sub_buffers->min_value_ = default_min_;
            sub_buffers->max_value_ = default_max_;
            sub_buffers->min_max_calculated_ = true;
        }
    }
}

// get number events
uint32 SignalBuffer::getNumberEvents() const
{
    QMutexLocker lock (&mutex_);
    if (!checkReadyState("getNumberEvents"))
    {
        return 0;
    }
    return id2signal_events_map_.size();
}

// event number to ID
int32 SignalBuffer::eventNumber2ID(uint32 event_number) const
{
    QMutexLocker lock (&mutex_);
    if (!checkReadyState("eventNumber2ID"))
    {
        return -1;
    }
    if ((int32)event_number >= id2signal_events_map_.size())
    {
        return -1; // invalid event number
    }

    Int2SignalEventPtrMap::const_iterator iter = id2signal_events_map_.begin();
    for (uint32 nr = 0; nr < event_number; nr++, iter++)
        ;
    return iter.key();
}

//-----------------------------------------------------------------------------
// get event
QSharedPointer<SignalEvent> SignalBuffer::getEvent(uint32 event_id)
{
    QMutexLocker lock (&mutex_);
    if (!checkReadyState("getEvent"))
    {
        return QSharedPointer<SignalEvent>(0);
    }
    Int2SignalEventPtrMap::iterator iter = id2signal_events_map_.find(event_id);
    if (iter == id2signal_events_map_.end())
    {
        return QSharedPointer<SignalEvent>(0); // invalid event_id
    }
    return iter.value();
}

//-----------------------------------------------------------------------------
QMap<int32, QSharedPointer<SignalEvent> > SignalBuffer::getEvents (uint16 type)
{
    QMutexLocker lock (&mutex_);
    QMap<int32, QSharedPointer<SignalEvent> > events;

    for (Int2SignalEventPtrMap::iterator iter = id2signal_events_map_.begin();
         iter != id2signal_events_map_.end(); ++iter)
    {
        if (iter.value()->getType() == type)
            events.insert(iter.value()->getPosition(), iter.value());
    }

    return events;
}

//-----------------------------------------------------------------------------
// add event
int32 SignalBuffer::addEvent(QSharedPointer<SignalEvent> event)
{
    QMutexLocker lock (&mutex_);
    if (!checkReadyState("addEvent"))
    {
        return -1;
    }
    event->setId(next_event_id_);
    id2signal_events_map_[next_event_id_] = event;
    return next_event_id_++;
}

// remove event
void SignalBuffer::removeEvent(uint32 event_id)
{
    QMutexLocker lock (&mutex_);
    Int2SignalEventPtrMap::iterator iter = id2signal_events_map_.find(event_id);
    if (iter == id2signal_events_map_.end())
    {
        return;
    }
    // delete iter.value();
    id2signal_events_map_.erase(iter);
}

// get event samplerate
double SignalBuffer::getEventSamplerate() const
{
    QMutexLocker lock (&mutex_);
    return signal_reader_.getBasicHeader()->getEventSamplerate();
}

// get signal data block
SignalDataBlock* SignalBuffer::getSignalDataBlock(uint32 channel_nr,
                                                  uint32 sub_sampl,
                                                  uint32 block_nr)
{
    QMutexLocker lock (&mutex_);
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
//### TODO: simplify this function
// fprintf(stdout,"getSignalDataBlockImpl(%i, %i, %i)\n",channel_nr, sub_sampl, block_nr);
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
    SignalDataBlock* return_value
            = &(channel_nr2sub_buffers_map_[channel_nr]
                  ->sub_queue[sub_sampl]->getSignalDataBlock(block_nr, valid));
    return return_value;
}

// get min value
float64 SignalBuffer::getMinValue(uint32 channel_nr) const
{
    QMutexLocker lock (&mutex_);
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
    QMutexLocker lock (&mutex_);
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
