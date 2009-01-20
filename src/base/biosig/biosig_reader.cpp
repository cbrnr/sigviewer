/*

    $Id: biosig_reader.cpp,v 1.29 2009-01-20 12:13:42 schloegl Exp $
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

extern char FLAG_NOCACHE; 

namespace BioSig_
{


// double const BioSigReader::SAMPLE_RATE_TOLERANCE_ = 1E-4;	// OBSOLETE (AS)

//-----------------------------------------------------------------------------
BioSigReader::BioSigReader() :
    basic_header_ (new BasicHeader ()),
    biosig_header_ (0)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
BioSigReader::~BioSigReader()
{
    if (biosig_header_)
    {
        sclose(biosig_header_);
        delete biosig_header_;
    }
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
    if (biosig_header_)
    {
        sclose(biosig_header_);
    }
    delete biosig_header_;
    biosig_header_ = 0;
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

    // caching - this is optional. 	
    // fileIO only done once, most useful for zipped files, 
    // turn it off if ratio between size of available RAM and filesize is smaller than 2
    //if (biosig_header_->FILE.COMPRESSION) 
    if (!FLAG_NOCACHE) 
    	cachingWholeFile(biosig_header_);	

    //hdr2ascii(biosig_header_,stdout,4);

    basic_header_->setFullFileName(c_file_name);

    basic_header_->setType(GetFileTypeString(biosig_header_->TYPE));

    uint16_t NS=0;  // count number of selected channels - status channels are already converted to event table
    for (uint16_t k=0; k<biosig_header_->NS; k++) {
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

//	fprintf(stdout,"BioSigReader::loadSignals (%u %u %u)\n",begin,end,start_record);

    bool something_done = true;
    for (uint32 rec_nr = start_record; something_done; rec_nr++)
    {
        bool rec_out_of_range = (rec_nr >= basic_header_->getNumberRecords());
        something_done = false;

        uint32 samples = biosig_header_->SPR;
        static double *read_data = 0;

	// read each block only once - no need to do this inside the channel loop 
        // ### TODO: use HDR.CHANNEL[k].OnOff to load only requested channels 
        delete[] read_data;
        read_data = new double[samples * biosig_header_->NS];
        memset(read_data, 0, sizeof(read_data));
        sread(read_data, rec_nr, 1, biosig_header_);

        for (FileSignalReader::SignalDataBlockPtrIterator data_block = begin;
             data_block != end;
             data_block++)
        {
            if ((*data_block)->sub_sampling > 1 ||
                (*data_block)->channel_number >= basic_header_->getNumberChannels())
            {
////                if (log_stream_)
////                {
////                    *log_stream_ << "GDFReader::loadChannels Error: "
////                                 << "invalid SignalDataBlock\n";
////                }
                continue;
            }

            uint32 actual_sample = (rec_nr - start_record) * samples;

            if (actual_sample + samples > (*data_block)->number_samples)
            {
                if (actual_sample >= (*data_block)->number_samples)
                {
                    (*data_block)->setBufferOffset(start_record * samples);
                    continue;   // signal data block full
                }
                samples = (*data_block)->number_samples - actual_sample;
            }
            float32* data_block_buffer = (*data_block)->getBuffer();
            float32* data_block_upper_buffer = (*data_block)->getUpperBuffer();
            float32* data_block_lower_buffer = (*data_block)->getLowerBuffer();
            bool* data_block_buffer_valid = (*data_block)->getBufferValid();

            if (rec_out_of_range)
            {
            	/*
            		AS 2009-01-20: Is this really needed or can we remove this part ?? 
            	*/
                for (uint32 samp = actual_sample;
                     samp < actual_sample + samples;
                     samp++)
                {
                    data_block_buffer_valid[samp] = false;
                }
            }
            else
            {
                for (uint32 samp = actual_sample;
                     samp < actual_sample + samples;
                     samp++)
                {
                    data_block_buffer[samp] = read_data[((*data_block)->channel_number * samples) + (samp - actual_sample)];
                    data_block_upper_buffer[samp] = data_block_buffer[samp];
                    data_block_lower_buffer[samp] = data_block_buffer[samp];
                    data_block_buffer_valid[samp] = data_block_buffer[samp]==data_block_buffer[samp];	// test for NaN
                }
            }
            something_done = true;

        }
//    fprintf(stdout,"%f sample[0] %f %f %f %i\n",read_data[0],*((*begin)->getBuffer()),*((*begin)->getUpperBuffer()),*((*begin)->getLowerBuffer()),*((*begin)->getBufferValid()));
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
