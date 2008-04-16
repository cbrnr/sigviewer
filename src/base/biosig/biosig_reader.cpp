#include "biosig_reader.h" 
#include "../stream_utils.h"
#include "../signal_data_block.h"
#include "../math_utils.h"
#include "../gdf/gdf_event.h"

#include <biosig.h>

#include <QTextStream>
#include <QTranslator>
#include <QMutexLocker>

#include <cmath>

using namespace std;

namespace BioSig_
{


double const BioSigReader::SAMPLE_RATE_TOLERANCE_ = 1E-4;

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
void BioSigReader::loadSignals(SignalDataBlockPtrIterator begin,
                            SignalDataBlockPtrIterator end, uint32 start_record)
{
    QMutexLocker lock (&mutex_);
    if (!biosig_header_)
    {
        if (log_stream_) 
        {
            *log_stream_ << "GDFReader::loadChannels Error: not open\n";
        }
        return;
    }
    
//
    bool something_done = true;
    for (uint32 rec_nr = start_record; something_done; rec_nr++)
    {
        bool rec_out_of_range = (rec_nr >= basic_header_->getNumberRecords());
        if (!rec_out_of_range)
        {
            // TODO??? readStreamValues(buffer_, *file_, basic_header_->getRecordSize());
        }
        something_done = false;
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
            SignalChannel* sig = basic_header_->getChannelPointer((*data_block)->channel_number);
            uint32 samples = sig->getSamplesPerRecord();
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
            something_done = true;
            if (rec_out_of_range)
            {
                for (uint32 samp = actual_sample;
                     samp < actual_sample + samples;
                     samp++)
                {
                    data_block_buffer_valid[samp] = false; 
                }
            }
            else
            {
                static uint32 old_rec_nr = -1;
                static double *read_data = 0;
                if (rec_nr != old_rec_nr)
                {
                    delete read_data;
                    read_data = new double[samples * biosig_header_->NS];
                    memset(read_data, 0, sizeof(read_data));
                    sread(read_data, rec_nr, ceil(samples/biosig_header_->SPR), biosig_header_);
                }
                old_rec_nr = rec_nr;

                for (uint32 samp = actual_sample;
                     samp < actual_sample + samples;
                     samp++)
                {
                    data_block_buffer[samp] = read_data[(*data_block)->channel_number + (samp - actual_sample)];
                    data_block_upper_buffer[samp] = data_block_buffer[samp];
                    data_block_lower_buffer[samp] = data_block_buffer[samp];
                    data_block_buffer_valid[samp] = true; 
                          //data_block_buffer[samp] > sig->getPhysicalMinimum() &&
                          //data_block_buffer[samp] < sig->getPhysicalMaximum();
                }
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

//-----------------------------------------------------------------------------
QString BioSigReader::loadFixedHeader(const QString& file_name)
{
    QMutexLocker locker (&biosig_access_lock_);
    char *c_file_name = new char[file_name.length()];
    strcpy (c_file_name, file_name.toLocal8Bit ().data());
    
    tzset();
     biosig_header_ = sopen(c_file_name, "r", NULL);
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

    biosig_header_->FLAG.UCAL = 0;  
    
    
    basic_header_->setFullFileName (file_name);
    switch (biosig_header_->TYPE)
    {
    case GDF:
        basic_header_->setType ("GDF");
        break;
    case BKR:
        basic_header_->setType ("BKR");
        break;
    case CNT:
        basic_header_->setType ("CNT");
        break;
    default:
        basic_header_->setType ("...");
        break;
    }
    basic_header_->setNumberChannels(biosig_header_->NS);
    basic_header_->setVersion (QString::number(biosig_header_->VERSION));
    basic_header_->setNumberRecords (biosig_header_->NRec);
    basic_header_->setRecordSize (biosig_header_->CHANNEL[0].SPR); // TODO: different channels different sample rate!!
    basic_header_->setRecordsPosition (biosig_header_->HeadLen); 
    basic_header_->setRecordDuration (static_cast<double>(biosig_header_->Dur[0]) / biosig_header_->Dur[1]);
    double sample_rate_error = (static_cast<double>(biosig_header_->SPR) / biosig_header_->SampleRate) - (static_cast<double>(biosig_header_->Dur[0]) / static_cast<double>(biosig_header_->Dur[1]));
    if (sample_rate_error > SAMPLE_RATE_TOLERANCE_ || sample_rate_error < -SAMPLE_RATE_TOLERANCE_)
    {
      if (biosig_header_)
      {
        sclose (biosig_header_);
        delete biosig_header_;
        biosig_header_ = 0;
      }
      return "Data corrupted: SampleRate and Record Duration don't match!";
    }
    basic_header_->setRecordDuration (static_cast<double>(biosig_header_->SPR) / biosig_header_->SampleRate);
    basic_header_->setNumberEvents(biosig_header_->EVENT.N);
    if (biosig_header_->EVENT.SampleRate)
        basic_header_->setEventSamplerate(static_cast<uint32>(biosig_header_->EVENT.SampleRate));
    else
        basic_header_->setEventSamplerate(static_cast<uint32>(biosig_header_->SampleRate));
    
    for (uint32 channel_index = 0; channel_index < biosig_header_->NS; ++channel_index)
    {
        SignalChannel* channel = new SignalChannel(channel_index, QT_TR_NOOP(biosig_header_->CHANNEL[channel_index].Label), 
                                                   biosig_header_->CHANNEL[channel_index].SPR,
                                                   biosig_header_->CHANNEL[channel_index].PhysDim, 
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

}
