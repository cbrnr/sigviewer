// gdf_reader.cpp

#include "gdf_reader.h"
#include "gdf_signal_header.h"
#include "../file_signal_reader_factory.h"
#include "../math_utils.h"
#include "../stream_utils.h"
#include "../signal_data_block.h"

#include <algorithm>

#include <QTextStream>

// read character array from string
// dest must be an array e.g. char text[10];
#ifndef readStreamString
#define readStreamString(dest, src_stream) \
readStreamChars(dest, src_stream, sizeof(dest))
#endif

// read array data from stream
// dest must be an array e.g. int a[10]
#define readStreamArray(dest, src_stream) \
readStreamValues(dest, src_stream, sizeof(dest))

namespace BioSig_
{

// constructor
GDFReader::GDFReader()
: buffer_(0)
{
    // nothing
}

// destructor
GDFReader::~GDFReader()
{
    if (file_.isOpen())
    {
        file_.close();
    }
    if (buffer_)
    {
        delete[] buffer_;
    }
}

// clone
FileSignalReader* GDFReader::clone()
{
    FileSignalReader* new_instance = new GDFReader;
    new_instance->setLogStream(log_stream_);
    return new_instance;
}

// check if open
bool GDFReader::isOpen()
{
    return file_.isOpen();
}

// close
void GDFReader::close()
{
    if (file_.isOpen())
    {
        if (log_stream_)
        {
            *log_stream_ << "GDFReader::close '" << full_file_name_ << "'\n";
        }
        file_.close();
        resetBasicHeader();
        resetGDFHeader();
    }
}

// open
bool GDFReader::open(const QString& file_name)
{
    qDebug( "GDFReader::open(const QString& file_name) 1" );
    if (file_.isOpen())
    {
        if (log_stream_)
        {
            *log_stream_ << "GDFReader::open '" << file_name << "' Error: '"
                        << full_file_name_ << "' not closed\n";
        }
        return false;
    }
    file_.setFileName(file_name);
    if (!file_.open(QIODevice::ReadOnly))
    {
        if (log_stream_)
        {
            *log_stream_ << "GDFReader::open '" << file_name
                        << "' Error: reading file\n";
        }
        return false;
    }

    // read headers
    if (log_stream_)
    {
        *log_stream_ << "GDFReader::open '" << file_name << "'\n";
    }
    if (!loadFixedHeader(file_name) ||
        !loadSignalHeaders(file_name))
    {
        file_.close();
        resetBasicHeader();
        resetGDFHeader();
        return false;
    }
    loadEventTableHeader();
    return true;
}

// load fixed header
inline bool GDFReader::loadFixedHeader(const QString& file_name)
{
    file_size_ = file_.size();
    readStreamString(gdf_version_id_, file_);
    readStreamString(gdf_patient_id_, file_);
    readStreamString(gdf_recording_id_, file_);
    readStreamString(gdf_start_recording_, file_);
    readStreamValue(gdf_header_size_, file_);
    readStreamValue(gdf_equipment_provider_id_, file_);
    readStreamValue(gdf_labratory_id_, file_);
    readStreamValue(gdf_technican_id_, file_);
    readStreamArray(gdf_resered_, file_);
    readStreamValue(gdf_number_data_records_, file_);
    readStreamArray(gdf_duration_data_record_, file_);
    readStreamValue(gdf_number_signals_, file_);
    if (strncmp(gdf_version_id_, "GDF", 3) != 0)
    {
        return false;
    }
    full_file_name_ = file_name;
    type_ = "GDF";
    version_ = gdf_version_id_;
    recording_time_ = QDateTime::fromString(QString(gdf_start_recording_).left(12),
                                            "yyyyMMddhhmmss");
    number_records_ = gdf_number_data_records_;
    record_duration_= (float64)gdf_duration_data_record_[0] /
                               gdf_duration_data_record_[1];
    patient_name_ = gdf_patient_id_;
    patient_age_ = BasicHeader::UNDEFINED_AGE;
    patient_sex_ = BasicHeader::UNDEFINED_SEX;
    doctor_id_ = gdf_technican_id_;
    hospital_id_ = gdf_labratory_id_;
    // mistake in specification: 0x2020.. for undefined integer values
    doctor_id_ =  doctor_id_ == 0x2020202020202020LL ? 0 : doctor_id_;
    hospital_id_ =  hospital_id_ == 0x2020202020202020LL ? 0 : hospital_id_;
    records_position_ = (uint32)gdf_header_size_;
    number_channels_ = gdf_number_signals_;

    return true;
}

// load signal headers
inline bool GDFReader::loadSignalHeaders(const QString& file_name)
{
    file_.seek(GDFHeader::SIZE);
    gdf_sig_vector_.resize(gdf_number_signals_);
    GDFSignalHeaderVector::iterator sig;
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        readStreamString(sig->label, file_);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        readStreamString(sig->transducer_type, file_);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        readStreamString(sig->physical_dimension, file_);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        readStreamValue(sig->physical_minimum, file_);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        readStreamValue(sig->physical_maximum, file_);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        readStreamValue(sig->digital_minimum, file_);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        readStreamValue(sig->digital_maximum, file_);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        readStreamString(sig->pre_filtering, file_);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        readStreamValue(sig->samples_per_record, file_);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        readStreamValue(sig->channel_type, file_);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        readStreamArray(sig->reserved, file_);
    }
    
    record_size_ = 0;
    if (number_channels_ > 0)
    {
        event_sample_rate_ = 1;
        sig = gdf_sig_vector_.begin();
        for (uint32 channel_nr = 0;
             sig != gdf_sig_vector_.end();
             sig++, channel_nr++)
        {
            SignalChannel* channel = new SignalChannel(channel_nr, sig->label,
                                                       sig->samples_per_record,
                                                       sig->physical_dimension,
                                                       sig->physical_minimum,
                                                       sig->physical_maximum,
                                                       sig->digital_minimum,
                                                       sig->digital_maximum,
                                                       sig->channel_type,
                                                       record_size_ / 8,
                                                       sig->pre_filtering, -1, -1, false);

            channel_vector_ << channel;
            event_sample_rate_ = lcm(event_sample_rate_,
                                     sig->samples_per_record);
            record_size_ += channel->typeBitSize() *
                            sig->samples_per_record;
            // TODO : signal record-size no multiple of 8 bits
            if (record_size_ % 8 != 0)
            {
                if (log_stream_)
                {
                    *log_stream_ << "GDFReader::open '" << file_name
                                 << "' Error: signal record size no multiple of "
                                 << "8 bits is not supported\n";
                }
                return false;
            }
        }
        event_sample_rate_ *= gdf_duration_data_record_[1];
        
        if (event_sample_rate_ % gdf_duration_data_record_[0] != 0)
        {
            if (log_stream_)
            {
                *log_stream_ << "GDFReader::open Warning: event-samplerate is"
                             << " non integer value\n";
            }
        }
        event_sample_rate_ /= gdf_duration_data_record_[0];
        record_size_ = (record_size_ + 7) / 8;
        buffer_ = new int8[record_size_];
    }
    return true;
}

// load event table header
inline void GDFReader::loadEventTableHeader()
{
    qDebug( "GDFReader::loadEventTableHeader() begin" );
    event_table_position_ = (uint32)gdf_header_size_ + record_size_
                            * (uint32)gdf_number_data_records_;
    qDebug( "GDFReader::loadEventTableHeader() 1" );
    file_.seek(event_table_position_);
    qDebug( "GDFReader::loadEventTableHeader() 2" );
    event_table_position_ += readStreamValue(gdf_event_table_type_,
                                             file_);
    event_table_position_ += readStreamArray(gdf_event_table_sample_rate_,
                                             file_);
    qDebug( "GDFReader::loadEventTableHeader() 3" );
    event_table_position_ +=
        readStreamValue(gdf_number_events_, file_);
    qDebug( "GDFReader::loadEventTableHeader() %d", gdf_number_events_ );
    if (event_sample_rate_ == 0)
    {
        event_sample_rate_ = ((uint32)gdf_event_table_sample_rate_[2] << 16) +
                             ((uint32)gdf_event_table_sample_rate_[1] << 8) +
                             gdf_event_table_sample_rate_[0];
    }
    else
    {
        gdf_event_table_sample_rate_[0] = (uint8)event_sample_rate_;
        gdf_event_table_sample_rate_[1] = (uint8)(event_sample_rate_ << 8);
        gdf_event_table_sample_rate_[2] = (uint8)(event_sample_rate_ << 16);
    }
    qDebug( "GDFReader::loadEventTableHeader() 5" );

    // calculate number of events
    number_events_ = 0;
    GDFEvent gdf_event;
    file_.seek(event_table_position_ + gdf_number_events_ *
               sizeof(gdf_event.position));
    qDebug( "GDFReader::loadEventTableHeader() 6" );
    for (uint32 event_nr = 0; event_nr < gdf_number_events_; event_nr++)
    {
        readStreamValue(gdf_event.type, file_);
        qDebug( "GDFReader::loadEventTableHeader() 7" );
        if ((gdf_event.type & SignalEvent::EVENT_END) == 0)
        {
            number_events_++;
        }
    }
    qDebug( "GDFReader::loadEventTableHeader() end" );
}


// load signals
void GDFReader::loadSignals(SignalDataBlockPtrIterator begin,
                            SignalDataBlockPtrIterator end,
                            uint32 start_record)
{
    if (!file_.isOpen())
    {
        if (log_stream_)
        {
            *log_stream_ << "GDFReader::loadChannels Error: not open\n";
        }
        return;
    }

    file_.seek(records_position_ + start_record * record_size_);
    bool something_done = true;
    for (uint32 rec_nr = start_record; something_done; rec_nr++)
    {
        bool rec_out_of_range = (rec_nr >= number_records_);
        if (!rec_out_of_range)
        {
            readStreamValues(buffer_, file_, record_size_);
        }
        something_done = false;
        for (SignalDataBlockPtrIterator data_block = begin;
             data_block != end;
             data_block++)
        {
            if ((*data_block)->sub_sampling > 1 ||
                (*data_block)->channel_number >= number_channels_)
            {
                if (log_stream_)
                {
                    *log_stream_ << "GDFReader::loadChannels Error: "
                                 << "invalid SignalDataBlock\n";
                }
                continue;
            }
            SignalChannel* sig = channel_vector_[(*data_block)->channel_number];
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
                convertData(buffer_, &data_block_buffer[actual_sample],
                            *sig, samples);
                for (uint32 samp = actual_sample;
                     samp < actual_sample + samples;
                     samp++)
                {
                    data_block_upper_buffer[samp] = data_block_buffer[samp];
                    data_block_lower_buffer[samp] = data_block_buffer[samp];
                    data_block_buffer_valid[samp] 
                        = data_block_buffer[samp] > sig->getPhysicalMinimum() &&
                          data_block_buffer[samp] < sig->getPhysicalMaximum();
                }
            }
        }
    }
}

// load events
void GDFReader::loadEvents(SignalEventVector& event_vector)
{
    if (!file_.isOpen())
    {
        if (log_stream_)
        {
            *log_stream_ << "GDFReader::loadEvents Error: not open\n";
        }
        return;
    }

    // load all gdf-events
    file_.seek(event_table_position_);
    QVector<GDFEvent> gdf_events(gdf_number_events_);
    QVector<GDFEvent>::iterator iter;
    for (iter = gdf_events.begin(); iter != gdf_events.end(); iter++)
    {
        readStreamValue(iter->position, file_);
    }
    for (iter = gdf_events.begin(); iter != gdf_events.end(); iter++)
    {
        readStreamValue(iter->type, file_);
    }
    if (gdf_event_table_type_ == EXTENDED_EVENT_TABLE)
    {
        for (iter = gdf_events.begin(); iter != gdf_events.end(); iter++)
        {
            readStreamValue(iter->channel, file_);
        }
        for (iter = gdf_events.begin(); iter != gdf_events.end(); iter++)
        {
            readStreamValue(iter->duration, file_);
        }
    }

    // sort events by position, type and channel
    qSort(gdf_events); // TODO
//    std::sort(gdf_events.begin(), gdf_events.end(), std::less<GDFEvent>());

    // store to signal events
    for (iter = gdf_events.begin(); iter != gdf_events.end(); iter++)
    {
        if (iter->type & SignalEvent::EVENT_END)
        {
            uint32 start_type = iter->type - SignalEvent::EVENT_END;
            bool start_found = false;
            SignalEventVector::iterator rev_iter = event_vector.end();
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
                if (log_stream_)
                {
                    *log_stream_ << "GDFReader::loadEvents Warning: unexpected "
                                 << " end-event\n";
                }
            }
        }
        else
        {
            if (gdf_event_table_type_ == EXTENDED_EVENT_TABLE)
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

// load raw records
bool GDFReader::loadRawRecords(float64** record_data, uint32 start_record,
                               uint32 records)
{
    if (!file_.isOpen())
    {
        if (log_stream_)
        {
            *log_stream_ << "GDFReader::loadRawRecord Error: not open\n";
        }
        return false;
    }
    if (start_record + records > number_records_)
    {
        if (log_stream_)
        {
            *log_stream_ << "GDFReader::loadRawRecord Error: invalid record\n";
        }
        return false;
    }

    file_.seek(records_position_ + start_record * record_size_);
    for (uint32 record = 0; record < records; record++)
    {
        readStreamValues(buffer_, file_, record_size_);
        float64** data = record_data;
        for (SignalChannelPtrVector::iterator it = channel_vector_.begin();
             it != channel_vector_.end();
             it++, data++)
        {
            convertData(buffer_, *data + (*it)->getSamplesPerRecord() * record,
                        *(*it), (*it)->getSamplesPerRecord(), false);
        }
    }
    return true;
}

} // namespace BioSig_
