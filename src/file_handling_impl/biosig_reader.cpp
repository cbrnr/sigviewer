/*

    $Id: biosig_reader.cpp,v 1.36 2009/03/03 11:57:07 cle1109 Exp $
    Copyright (C) Thomas Brunner  2005,2006,2007
    Copyright (C) Christoph Eibel 2007,2008
    Copyright (C) Clemens Brunner 2006,2007,2008
    Copyright (C) Alois Schloegl  2008,2009
    Copyright (C) Oliver Terbu    2008
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
#include "gdf_event.h"
#include "biosig_basic_header.h"
#include "../file_handling/file_signal_reader_factory.h"

#include "../../extern/biosig.h"

#include <QTextStream>
#include <QTranslator>
#include <QMutexLocker>
#include <QDebug>

#include <cmath>
#include <cassert>

using namespace std;

namespace BioSig_
{

BioSigReader BioSigReader::prototype_instance_ (true);

//-----------------------------------------------------------------------------
BioSigReader::BioSigReader() :
    basic_header_ (0),
    biosig_header_ (0),
    read_data_(0),
    read_data_size_(0),
    buffered_all_channels_ (false),
    events_loaded_ (false),
    is_open_ (false)
{
    qDebug () << "Constructed BioSigReader";
    // nothing to do here
}

//-----------------------------------------------------------------------------
BioSigReader::BioSigReader (bool) :
       basic_header_ (0),
       biosig_header_ (0),
       read_data_(0),
       read_data_size_(0),
       buffered_all_channels_ (false)
{
    qDebug () << "BioSigReader constructing prototypes";
    FileSignalReaderFactory::getInstance()->addPrototype(".gdf", new BioSigReader ());
    FileSignalReaderFactory::getInstance()->addPrototype(".evt", new BioSigReader ());
    FileSignalReaderFactory::getInstance()->addPrototype(".bdf", new BioSigReader ());
    FileSignalReaderFactory::getInstance()->addPrototype(".bkr", new BioSigReader ());
    FileSignalReaderFactory::getInstance()->addPrototype(".cnt", new BioSigReader ());
    FileSignalReaderFactory::getInstance()->addPrototype(".edf", new BioSigReader ());
    FileSignalReaderFactory::getInstance()->addPrototype(".eeg", new BioSigReader ());

    FileSignalReaderFactory::getInstance()->addPrototype(".acq", new BioSigReader ());	// Biopac
    FileSignalReaderFactory::getInstance()->addPrototype(".ahdr", new BioSigReader ());	// BrainVision file format
    FileSignalReaderFactory::getInstance()->addPrototype(".vhdr", new BioSigReader ());	// BrainVision file format
    FileSignalReaderFactory::getInstance()->addPrototype(".scp", new BioSigReader ());	// SCP-ECG: EN1064, ISO 11073-91064

    FileSignalReaderFactory::getInstance()->setDefaultPrototype (new BioSigReader ());
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

//-----------------------------------------------------------------------------
void BioSigReader::doClose () const
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
    biosig_header_ = 0;
}

//-----------------------------------------------------------------------------
QSharedPointer<DataBlock const> BioSigReader::getSignalData (ChannelID channel_id,
                                       unsigned start_sample,
                                       unsigned length) const
{
    QMutexLocker lock (&mutex_);

    if (!buffered_all_channels_)
        bufferAllChannels();

    if (!channel_map_.contains(channel_id))
        return QSharedPointer<DataBlock const> (0);

    if (length == basic_header_->getNumberOfSamples() &&
        start_sample == 0)
        return channel_map_[channel_id];
    else
        return channel_map_[channel_id]->createSubBlock (start_sample, length);
    /*
    start_sample = floor (static_cast<float32>(start_sample) / biosig_header_->SPR);
    start_sample *= biosig_header_->SPR;
    length = ceil (static_cast<float32>(length) / biosig_header_->SPR);
    length *= biosig_header_->SPR;

    if (read_data_ == 0 || read_data_size_ < length * biosig_header_->NS)
    {
        read_data_size_ = length * biosig_header_->NS;
        delete[] read_data_;
        read_data_ = new biosig_data_type[read_data_size_];
    }

    memset (read_data_, 0, read_data_size_ * sizeof(double));
    sread (read_data_, start_sample / biosig_header_->SPR,
           length / biosig_header_->SPR,
           biosig_header_);

    std::vector<float32> raw_data (length);
    for (unsigned index = 0; index < length; index++)
    {
        if (biosig_header_->FLAG.ROW_BASED_CHANNELS == 1)
        {
            raw_data[index] = read_data_[length + (index * channel_id)];
        }
        else
            raw_data[index] = read_data_[(length * channel_id) + index];
    }
    return DataBlock (raw_data, biosig_header_->SampleRate);*/
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
                        rev_iter->getDuration() == 0)
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
                event_vector.push_back(SignalEvent(iter->position, iter->type, biosig_header_->EVENT.SampleRate,
                                            (int32)iter->channel - 1,
                                            iter->duration));
            }
            else
            {
                event_vector.push_back(SignalEvent(iter->position, iter->type, biosig_header_->EVENT.SampleRate));
            }
        }
    }
    events_loaded_ = true;
    if (buffered_all_channels_)
        doClose();
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
        destructHDR(biosig_header_);
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
    if(biosig_header_==NULL)
    {
        biosig_header_ = constructHDR(0,0);
        biosig_header_->FLAG.UCAL = 0;
        biosig_header_->FLAG.OVERFLOWDETECTION = 0;
    }

    biosig_header_ = sopen(c_file_name, "r", biosig_header_ );
    if (!basic_header_.isNull())
        delete basic_header_;

    basic_header_ = new BiosigBasicHeader (biosig_header_);

    /// waldesel: REMOVE OLD STUFF from here downwards
    ///           and move it into BiosigBasicHeader!!!

#ifdef CHOLMOD_H
    char *REREF_FILE;
    REREF_FILE = "/home/as/trunk/biosig4c++/MM63.mmm";
    REREF_FILE = NULL; 
    /*
    	TODD: 
    	REREF_FILE must be the filename of the spatial filter in MarketMatrix format 
    */

    RerefCHANNEL(biosig_header_, REREF_FILE, 1);
#endif
    if (biosig_header_ == NULL || serror())
    {
            sclose (biosig_header_);
            destructHDR(biosig_header_);
            biosig_header_ = 0;
            delete c_file_name;
            return "file not supported";
    }

    // (C) 2008 AS: EVENT.DUR and EVENT.CHN are optional in SOPEN, but SigViewer needs them.
    convert2to4_eventtable(biosig_header_);

    //hdr2ascii(biosig_header_,stdout,4);

    basic_header_->setFullFileName(c_file_name);
    delete c_file_name;
    c_file_name = 0;

    basic_header_->setType(GetFileTypeString(biosig_header_->TYPE));

    uint16_t NS=0;  // count number of selected channels - status channels are already converted to event table
    for (uint16_t k=0; k<biosig_header_->NS; k++)
    {
        if (biosig_header_->CHANNEL[k].OnOff) NS++;
    }

#ifdef CHOLMOD_H
    if ((biosig_header_->Calib) && (biosig_header_->Calib->nrow==NS))
	NS = biosig_header_->Calib->ncol;
#endif
    basic_header_->setNumberChannels(NS);

    basic_header_->setVersion (QString::number(biosig_header_->VERSION));
    basic_header_->setNumberRecords (biosig_header_->NRec);
    basic_header_->setRecordSize (biosig_header_->SPR);
    basic_header_->setRecordsPosition (biosig_header_->HeadLen);

    basic_header_->setRecordDuration (biosig_header_->SPR / biosig_header_->SampleRate);
    basic_header_->setSampleRate (biosig_header_->SampleRate);
    basic_header_->setNumberEvents(biosig_header_->EVENT.N);
    if (biosig_header_->EVENT.SampleRate)
        basic_header_->setEventSamplerate(biosig_header_->EVENT.SampleRate);
    else
        basic_header_->setEventSamplerate(biosig_header_->SampleRate);

#ifdef CHOLMOD_H
    if (biosig_header_->Calib==NULL) {
#endif
    uint16_t chan=0;
    for (uint16_t channel_index = 0; channel_index < biosig_header_->NS; ++channel_index)
    if (biosig_header_->CHANNEL[channel_index].OnOff)	// show only selected channels - status channels are not shown.
    {
        SignalChannel* channel = new SignalChannel(chan++,
                                                   biosig_header_->SPR,
                                                   biosig_header_->CHANNEL[channel_index]);
        basic_header_->addChannel(channel);
    }
#ifdef CHOLMOD_H
    } else 
    for (uint16_t channel_index = 0; channel_index < biosig_header_->Calib->ncol; ++channel_index)
    {
        SignalChannel* channel = new SignalChannel(channel_index, 
                                                   biosig_header_->SPR,
                                                   biosig_header_->rerefCHANNEL[channel_index]);
        basic_header_->addChannel(channel);
    }
#endif
    is_open_ = true;
    return "";
}

//-----------------------------------------------------------------------------
QPointer<BasicHeader> BioSigReader::getBasicHeader ()
{
    QMutexLocker lock (&mutex_);
    return QPointer<BasicHeader>(basic_header_);
}

//-----------------------------------------------------------------------------
void BioSigReader::bufferAllChannels () const
{
    uint32 length = basic_header_->getNumberOfSamples ();

    if (read_data_ == 0 || read_data_size_ < length * biosig_header_->NS)
    {
        read_data_size_ = length * biosig_header_->NS;
        delete[] read_data_;
        read_data_ = new biosig_data_type[read_data_size_];
    }

    biosig_header_->FLAG.ROW_BASED_CHANNELS = 0;

    memset (read_data_, 0, read_data_size_ * sizeof(biosig_data_type));
    sread (read_data_, 0,
           length / biosig_header_->SPR,
           biosig_header_);

    for (ChannelID channel_id = 0; channel_id < basic_header_->getNumberChannels();
         ++channel_id)
    {
        std::vector<float32> raw_data (length);
        for (unsigned index = 0; index < length; index++)
        {
//            if (biosig_header_->FLAG.ROW_BASED_CHANNELS == 1)
//            {
//                raw_data[index] = read_data_[length + (index * channel_id)];
//            }
//            else
                raw_data[index] = read_data_[(length * channel_id) + index];
        }
        QSharedPointer<DataBlock> data_block (new DataBlock (raw_data,
                                                             basic_header_->getSampleRate()));
        channel_map_[channel_id] = data_block;
    }
    buffered_all_channels_ = true;
    if (events_loaded_)
        doClose();
}


}
