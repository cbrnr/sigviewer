/*

    $Id: biosig_reader.cpp,v 1.35 2009-03-02 21:01:41 brunnert Exp $
    Copyright (C) Thomas Brunner  2005,2006,2007
    		  Christoph Eibel 2007,2008,
		  Clemens Brunner 2006,2007,2008
    		  Alois Schloegl  2008,2009
    		  Oliver Terbu    2008
    This file is part of the "SigViewer" repository
    at http://biosig.sf.net/

    This program is free software; you can redistribute it and/or
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

#include "biosig_reader.h"
#include "../signal_data_block.h"
#include "../gdf_event.h"

#include "../../../extern/biosig.h"

#include <QTextStream>
#include <QTranslator>
#include <QMutexLocker>

#include <cmath>

using namespace std;

namespace BioSig_
{


// double const BioSigReader::SAMPLE_RATE_TOLERANCE_ = 1E-4;	// OBSOLETE (AS)

//-----------------------------------------------------------------------------
BioSigReader::BioSigReader() :
    basic_header_ (new BasicHeader ()),
    biosig_header_ (0),
    read_data_(0),
    read_data_size_(0)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
BioSigReader::~BioSigReader()
{
	doClose();
}

//-----------------------------------------------------------------------------
FileSignalReader* BioSigReader::clone()
{
    BioSigReader *new_instance = new BioSigReader ();
    return new_instance;
}

//-----------------------------------------------------------------------------
void BioSigReader::close()
{
	doClose();
}

void BioSigReader::doClose()
{
    if (biosig_header_)
    {
        sclose(biosig_header_);
        destructHDR(biosig_header_);
        biosig_header_ = 0;
    }
    delete[] read_data_;
    read_data_ = 0;
    read_data_size_ = 0;
}

//-----------------------------------------------------------------------------
void BioSigReader::loadEvents(SignalEventVector& event_vector)
{
    QMutexLocker lock (&mutex_);
    if (!biosig_header_)
        return;

    QVector<GDFEvent> gdf_events(biosig_header_->EVENT.N);
    QVector<GDFEvent>::iterator iter;

    uint32 event_nr = 0;
    for (iter = gdf_events.begin(); iter != gdf_events.end(); iter++, event_nr++)
    {
        iter->type = biosig_header_->EVENT.TYP[event_nr];
        iter->position = biosig_header_->EVENT.POS[event_nr];
        if (biosig_header_->EVENT.CHN)
        {
            iter->channel = biosig_header_->EVENT.CHN[event_nr];
            iter->duration = biosig_header_->EVENT.DUR[event_nr];
        }
    }

    // sort events by position, type and channel
    qSort(gdf_events);

    // store to signal events
    for (iter = gdf_events.begin(); iter != gdf_events.end(); iter++)
    {
        if (iter->type & SignalEvent::EVENT_END)
        {
            uint32 start_type = iter->type - SignalEvent::EVENT_END;
            bool start_found = false;
            FileSignalReader::SignalEventVector::iterator rev_iter = event_vector.end();
            while (rev_iter != event_vector.begin())
            {
                rev_iter--;
                if (rev_iter->getType() == start_type &&
                        rev_iter->getDuration() == SignalEvent::UNDEFINED_DURATION)
                {
                    rev_iter->setDuration(iter->position -
                                          rev_iter->getPosition());
                    start_found = true;
                    break;
                }
            }
            if (!start_found)
            {
//                if (log_stream_)
//                {
//                    *log_stream_ << "GDFReader::loadEvents Warning: unexpected "
//                                 << " end-event\n";
//                }
            }
        }
        else
        {
            if (biosig_header_->EVENT.CHN)
            {
                event_vector << SignalEvent(iter->position, iter->type,
                                            (int32)iter->channel - 1,
                                            iter->duration);
            }
            else
            {
                event_vector << SignalEvent(iter->position, iter->type);
            }
        }
    }
}

//-----------------------------------------------------------------------------
QString BioSigReader::open(const QString& file_name)
{

    QMutexLocker lock (&mutex_);
    return loadFixedHeader (file_name);
}

void BioSigReader::enableCaching()
{
    QMutexLocker lock (&mutex_);
    // caching - this is optional.
    // fileIO only done once, most useful for zipped files,
    // turn it off if ratio between size of available RAM and filesize is smaller than 2
    //if (biosig_header_->FILE.COMPRESSION)
    if (biosig_header_)
        cachingWholeFile(biosig_header_);
}

//-----------------------------------------------------------------------------
void BioSigReader::setFlagOverflow(const bool overflow_detection)
{
    biosig_header_->FLAG.OVERFLOWDETECTION = overflow_detection;
}

//-----------------------------------------------------------------------------
QString BioSigReader::open(const QString& file_name, const bool overflow_detection)
{

    QMutexLocker lock (&mutex_);

    if (biosig_header_)
    {
        sclose(biosig_header_);
        delete biosig_header_;
        biosig_header_ = 0;
    }

    biosig_header_ = constructHDR(0,0);
    biosig_header_->FLAG.UCAL = 0;
    /* TODO:
    - changing Overflow flag when file is already opened has no effect.
        Solution: the overflow_detection flag must always propagate to
        biosig_header_->FLAG.OVERFLOWDETECTION = overflow_detection;
    */
    biosig_header_->FLAG.OVERFLOWDETECTION = overflow_detection;
    biosig_header_->FLAG.ROW_BASED_CHANNELS = 0;

    return loadFixedHeader (file_name);
}

//-----------------------------------------------------------------------------
QString BioSigReader::loadFixedHeader(const QString& file_name)
{
    QMutexLocker locker (&biosig_access_lock_);
    char *c_file_name = new char[file_name.length() + 1];
    strcpy (c_file_name, file_name.toLocal8Bit ().data());
    c_file_name[file_name.length()] = '\0';

    tzset();

//  VERBOSE_LEVEL=8;

    // set flags
    if(!biosig_header_)
    {
        biosig_header_ = constructHDR(0,0);
        biosig_header_->FLAG.UCAL = 0;
        biosig_header_->FLAG.OVERFLOWDETECTION = 0;
        biosig_header_->FLAG.ROW_BASED_CHANNELS = 0;
    }

    biosig_header_ = sopen(c_file_name, "r", biosig_header_ );
    if (biosig_header_ == NULL || serror())
    {
        if (biosig_header_)
        {
            sclose (biosig_header_);
            delete biosig_header_;
            biosig_header_ = 0;
        }
        return "file not supported";
    }

    // (C) 2008 AS: EVENT.DUR and EVENT.CHN are optional in SOPEN, but SigViewer needs them.
    convert2to4_eventtable(biosig_header_);

    //hdr2ascii(biosig_header_,stdout,4);

    basic_header_->setFullFileName(c_file_name);

    basic_header_->setType(GetFileTypeString(biosig_header_->TYPE));

    uint16_t NS=0;  // count number of selected channels - status channels are already converted to event table
    for (uint16_t k=0; k<biosig_header_->NS; k++)
    {
        if (biosig_header_->CHANNEL[k].OnOff) NS++;
    }
    basic_header_->setNumberChannels(NS);
    basic_header_->setVersion (QString::number(biosig_header_->VERSION));
    basic_header_->setNumberRecords (biosig_header_->NRec);
    basic_header_->setRecordSize (biosig_header_->SPR);
    basic_header_->setRecordsPosition (biosig_header_->HeadLen);

    basic_header_->setRecordDuration (biosig_header_->SPR / biosig_header_->SampleRate);
    basic_header_->setNumberEvents(biosig_header_->EVENT.N);
    if (biosig_header_->EVENT.SampleRate)
        basic_header_->setEventSamplerate(biosig_header_->EVENT.SampleRate);
    else
        basic_header_->setEventSamplerate(biosig_header_->SampleRate);

    for (uint32 channel_index = 0; channel_index < biosig_header_->NS; ++channel_index)
    if (biosig_header_->CHANNEL[channel_index].OnOff)	// show only selected channels - status channels are not shown.
    {
        SignalChannel* channel = new SignalChannel(channel_index, QT_TR_NOOP(biosig_header_->CHANNEL[channel_index].Label),
                                                   biosig_header_->SPR,
                                                   biosig_header_->CHANNEL[channel_index].PhysDim,// depreciated, replace with PhysDim(physical_dimcode,...)
                                                   biosig_header_->CHANNEL[channel_index].PhysDimCode,
                                                   biosig_header_->CHANNEL[channel_index].PhysMin,
                                                   biosig_header_->CHANNEL[channel_index].PhysMax,
                                                   biosig_header_->CHANNEL[channel_index].DigMin,
                                                   biosig_header_->CHANNEL[channel_index].DigMax,
                                                   SignalChannel::FLOAT64,
                                                   1 / 8, // TODO: really don't know what that means!
                                                   "filter", // maybe useless
                                                   biosig_header_->CHANNEL[channel_index].LowPass,
                                                   biosig_header_->CHANNEL[channel_index].HighPass,
                                                   biosig_header_->CHANNEL[channel_index].Notch > 0.0);
        basic_header_->addChannel(channel);
    }
    return "";
}

//-----------------------------------------------------------------------------
void BioSigReader::loadSignals(SignalDataBlockPtrIterator begin,
                            SignalDataBlockPtrIterator end, uint32 start_record)
{
    QMutexLocker lock (&mutex_);
    if (!biosig_header_)
    {
        if (log_stream_)
        {
            *log_stream_ << "BioSigReader::loadChannels Error: not open\n";
        }
        return;
    }

    // calculate sample count
    uint32 samples = 0;
    for (FileSignalReader::SignalDataBlockPtrIterator data_block = begin;
         data_block != end;
         data_block++)
    {
        if ((*data_block)->sub_sampling > 1 ||
            (*data_block)->channel_number >= basic_header_->getNumberChannels())
        {
        	return; // invalid data block
        }
    	samples = std::max<uint32>(samples, (*data_block)->number_samples);
    }

    // calculate record count
    int32 record_count = samples / biosig_header_->SPR;
    if (record_count * biosig_header_->SPR < samples)
    	++record_count;
    if (start_record + record_count > basic_header_->getNumberRecords())
    	record_count = basic_header_->getNumberRecords() - start_record;
    if (record_count < 0)
    	record_count = 0;

    // allocate buffer
    uint32 samples_in_read_data = record_count * biosig_header_->SPR;
    if (read_data_ == 0 || read_data_size_ < samples_in_read_data * biosig_header_->NS)
    {
        read_data_size_ = samples_in_read_data * biosig_header_->NS;
        delete[] read_data_;
        read_data_ = new double[read_data_size_];
    }

	// read data as block
    memset(read_data_, 0, samples_in_read_data * biosig_header_->NS*sizeof(double));
    size_t count = sread(read_data_, start_record, record_count, biosig_header_);

    // fill data blocks
    for (FileSignalReader::SignalDataBlockPtrIterator data_block = begin;
         data_block != end;
         data_block++)
    {
        float32* data_block_buffer = (*data_block)->getBuffer();
        float32* data_block_upper_buffer = (*data_block)->getUpperBuffer();
        float32* data_block_lower_buffer = (*data_block)->getLowerBuffer();
        bool* data_block_buffer_valid = (*data_block)->getBufferValid();

        (*data_block)->setBufferOffset(start_record * biosig_header_->SPR);
        for (uint32 samp = 0; samp < (*data_block)->number_samples; ++samp)
        {
        	if (samp < samples_in_read_data)
        	{
        		data_block_buffer[samp] = read_data_[(*data_block)->channel_number * samples_in_read_data + samp];
        		data_block_upper_buffer[samp] = data_block_buffer[samp];
        		data_block_lower_buffer[samp] = data_block_buffer[samp];
        		data_block_buffer_valid[samp] = data_block_buffer[samp]==data_block_buffer[samp];	// test for NaN
        	}
        	else
        	{
        		data_block_buffer_valid[samp] = false;
        	}
        }
    }
}

//-----------------------------------------------------------------------------
QPointer<BasicHeader> BioSigReader::getBasicHeader ()
{
    QMutexLocker lock (&mutex_);
    return QPointer<BasicHeader>(basic_header_);
}

//-----------------------------------------------------------------------------
HDRTYPE* BioSigReader::getRawHeader ()
{
    return biosig_header_;
}

}
