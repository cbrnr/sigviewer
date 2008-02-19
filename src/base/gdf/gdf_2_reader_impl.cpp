#include "gdf_2_reader_impl.h"
#include "gdf_signal_header.h"
#include "../stream_utils.h"
#include "../math_utils.h"
#include "../signal_data_block.h"
#include "gdf_event.h"


namespace BioSig_ 
{

GDF2ReaderImpl::GDF2ReaderImpl(QPointer<QFile> file, QPointer<BasicHeader> basic_header)
: file_ (file),
  basic_header_ (basic_header)
{
    // nothing to do here
}

GDF2ReaderImpl::~GDF2ReaderImpl ()
{
    
}


bool GDF2ReaderImpl::loadFixedHeader ()
{
    file_->seek(8);
    basic_header_->setFileSize (file_->size());
    basic_header_->setFullFileName (file_->fileName());
    basic_header_->setType("GDF");
    
    readStreamChars (gdf_header_.patient_identification_, *file_, sizeof(gdf_header_.patient_identification_));
    readStreamValues (gdf_header_.reserved_1_, *file_, sizeof(gdf_header_.reserved_1_));
    readStreamValue (gdf_header_.smoking_alcohol_drug_medication, *file_);
    readStreamValue (gdf_header_.weight_in_kg_, *file_);
    readStreamValue (gdf_header_.height_in_cm_, *file_);
    readStreamValue (gdf_header_.gender_handedness_visual_impairment_, *file_);
    readStreamChars (gdf_header_.recording_identification_, *file_, sizeof (gdf_header_.recording_identification_));
    readStreamValues (gdf_header_.recording_location_, *file_, sizeof(gdf_header_.recording_location_));
    readStreamValues (gdf_header_.startdate_and_time_of_recording_, *file_, sizeof(gdf_header_.startdate_and_time_of_recording_));
    readStreamValues (gdf_header_.birthday_, *file_, sizeof(gdf_header_.birthday_));
    readStreamValue (gdf_header_.header_length_, *file_);
    readStreamValues (gdf_header_.reserved_2_, *file_, sizeof (gdf_header_.reserved_2_));
    readStreamValue (gdf_header_.equipment_provider_identification_, *file_);
    readStreamValues (gdf_header_.ip_address_, *file_, sizeof (gdf_header_.ip_address_));
    readStreamValues (gdf_header_.head_size_in_mm_, *file_, sizeof (gdf_header_.head_size_in_mm_));
    readStreamValues (gdf_header_.position_reference_electrode_, *file_, sizeof (gdf_header_.position_reference_electrode_));
    readStreamValues (gdf_header_.position_ground_electrode_, *file_, sizeof (gdf_header_.position_ground_electrode_));
    readStreamValue (gdf_header_.number_of_data_records_, *file_);
    readStreamValues (gdf_header_.duration_data_record_rational_, *file_, sizeof (gdf_header_.duration_data_record_rational_));
    readStreamValue (gdf_header_.number_of_signals_, *file_);
    readStreamValue (gdf_header_.reserved_3_, *file_);
    

//    recording_time_ = QDateTime::fromString(QString(gdf_start_recording_).left(12),
//                                            "yyyyMMddhhmmss");
    basic_header_->setNumberRecords(gdf_header_.number_of_data_records_);
    basic_header_->setRecordDuration(static_cast<float64>(gdf_header_.duration_data_record_rational_[0]) /
                                     gdf_header_.duration_data_record_rational_[1]);
//    patient_name_ = gdf_patient_id_;
//    patient_age_ = BasicHeader::UNDEFINED_AGE;
//    patient_sex_ = BasicHeader::UNDEFINED_SEX;
//    doctor_id_ = gdf_technican_id_;
//    hospital_id_ = gdf_labratory_id_;
//    // mistake in specification: 0x2020.. for undefined integer values
//    doctor_id_ =  doctor_id_ == 0x2020202020202020LL ? 0 : doctor_id_;
//    hospital_id_ =  hospital_id_ == 0x2020202020202020LL ? 0 : hospital_id_;
    basic_header_->setRecordsPosition(static_cast<uint32>(gdf_header_.header_length_) * 256);
    basic_header_->setNumberChannels(gdf_header_.number_of_signals_);
    
    return true;
}

bool GDF2ReaderImpl::loadSignalHeaders ()
{
    file_->seek(GDF2Header::SIZE_);
    gdf_header_.sig_vector_.resize(gdf_header_.number_of_signals_);
    GDF2Header::GDFSignalHeaderVector::iterator sig;
    for (sig = gdf_header_.sig_vector_.begin(); sig != gdf_header_.sig_vector_.end(); sig++)
        readStreamChars(sig->label, *file_, sizeof(sig->label));
    for (sig = gdf_header_.sig_vector_.begin(); sig != gdf_header_.sig_vector_.end(); sig++)
        readStreamChars(sig->transducer_type, *file_, sizeof (sig->transducer_type));
    for (sig = gdf_header_.sig_vector_.begin(); sig !=gdf_header_. sig_vector_.end(); sig++)
        readStreamChars(sig->physical_dimension, *file_, sizeof(sig->physical_dimension));
    for (sig = gdf_header_.sig_vector_.begin(); sig != gdf_header_.sig_vector_.end(); sig++)
        readStreamValue(sig->physical_dimension_code, *file_);
    for (sig = gdf_header_.sig_vector_.begin(); sig != gdf_header_.sig_vector_.end(); sig++)
        readStreamValue(sig->physical_minimum, *file_);
    for (sig = gdf_header_.sig_vector_.begin(); sig != gdf_header_.sig_vector_.end(); sig++) 
        readStreamValue(sig->physical_maximum, *file_);
    for (sig = gdf_header_.sig_vector_.begin(); sig != gdf_header_.sig_vector_.end(); sig++)
        readStreamValue(sig->digital_minimum, *file_);
    for (sig = gdf_header_.sig_vector_.begin(); sig != gdf_header_.sig_vector_.end(); sig++)
        readStreamValue(sig->digital_maximum, *file_);
    for (sig = gdf_header_.sig_vector_.begin(); sig != gdf_header_.sig_vector_.end(); sig++)
        readStreamChars(sig->pre_filtering, *file_, sizeof(sig->pre_filtering));
    for (sig = gdf_header_.sig_vector_.begin(); sig != gdf_header_.sig_vector_.end(); sig++)
        readStreamValue(sig->low_pass, *file_);
    for (sig = gdf_header_.sig_vector_.begin(); sig != gdf_header_.sig_vector_.end(); sig++)
        readStreamValue(sig->high_pass, *file_);
    for (sig = gdf_header_.sig_vector_.begin(); sig != gdf_header_.sig_vector_.end(); sig++)
        readStreamValue(sig->notch, *file_);
    for (sig = gdf_header_.sig_vector_.begin(); sig != gdf_header_.sig_vector_.end(); sig++)
        readStreamValue(sig->samples_per_record, *file_);
    for (sig = gdf_header_.sig_vector_.begin(); sig != gdf_header_.sig_vector_.end(); sig++)
        readStreamValue(sig->channel_type, *file_);
    for (sig = gdf_header_.sig_vector_.begin(); sig != gdf_header_.sig_vector_.end(); sig++)
        readStreamValues(sig->electrode_position, *file_, sizeof(sig->electrode_position));
    for (sig = gdf_header_.sig_vector_.begin(); sig != gdf_header_.sig_vector_.end(); sig++)
        readStreamValue(sig->electrode_impedance, *file_);
    for (sig = gdf_header_.sig_vector_.begin(); sig != gdf_header_.sig_vector_.end(); sig++)
        readStreamValues(sig->reserved, *file_, sizeof(sig->reserved));
    
    uint32 record_size = 0;
    if (basic_header_->getNumberChannels() > 0)
    {
        uint32 event_sample_rate = 1;
        sig = gdf_header_.sig_vector_.begin();
        for (uint32 channel_nr = 0;
             sig != gdf_header_.sig_vector_.end();
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
                                                       record_size / 8,
                                                       sig->pre_filtering, 
                                                       sig->low_pass, 
                                                       sig->high_pass, 
                                                       sig->notch);

            basic_header_->addChannel(channel);
            event_sample_rate = lcm(event_sample_rate,
                                     sig->samples_per_record);

            record_size += channel->typeBitSize() *
            sig->samples_per_record;
            // TODO : signal record-size no multiple of 8 bits
        }
        event_sample_rate *= gdf_header_.duration_data_record_rational_[1];
        
        if (event_sample_rate % gdf_header_.duration_data_record_rational_[0] != 0)
        {
//            if (log_stream_)
//            {
//                *log_stream_ << "GDFReader::open Warning: event-samplerate is"
//                             << " non integer value\n";
//            }
        }
        event_sample_rate /= gdf_header_.duration_data_record_rational_[0];
        record_size = (record_size + 7) / 8;
        buffer_ = new int8[record_size];
        basic_header_->setRecordSize(record_size);
        basic_header_->setEventSamplerate (event_sample_rate);
    }    
    return true; 
}

bool GDF2ReaderImpl::loadEventTableHeader ()
{
    qDebug( "GDF2ReaderImpl::loadEventTableHeader() begin" );
    event_table_position_ = basic_header_->getRecordsPosition() + basic_header_->getRecordSize ()
                            * basic_header_->getNumberRecords();
    qDebug( "GDF2ReaderImpl::loadEventTableHeader() 1" );
    file_->seek(event_table_position_);
    qDebug( "GDF2ReaderImpl::loadEventTableHeader() 2" );
    event_table_position_ += readStreamValue(gdf_header_.mode_of_event_table_,
                                             *file_);
    event_table_position_ +=
        readStreamValue(gdf_header_.number_of_events_, *file_);
    event_table_position_ += readStreamValues(gdf_header_.gdf_event_table_sample_rate_,
                                             *file_,sizeof(gdf_header_.gdf_event_table_sample_rate_));
    qDebug( "GDF2ReaderImpl::loadEventTableHeader() 3" );

    //qDebug( "GDFReader::loadEventTableHeader() %d", gdf_header_.gdf_number_events_ );
    uint32 event_sample_rate = basic_header_->getEventSamplerate();
    if (event_sample_rate == 0)
    {
        event_sample_rate = ((uint32)gdf_header_.gdf_event_table_sample_rate_[2] << 16) +
                             ((uint32)gdf_header_.gdf_event_table_sample_rate_[1] << 8) +
                             gdf_header_.gdf_event_table_sample_rate_[0];
    }
    else
    {
        gdf_header_.gdf_event_table_sample_rate_[0] = (uint8)event_sample_rate;
        gdf_header_.gdf_event_table_sample_rate_[1] = (uint8)(event_sample_rate << 8);
        gdf_header_.gdf_event_table_sample_rate_[2] = (uint8)(event_sample_rate << 16);
    }
    qDebug( "GDFReader::loadEventTableHeader() 5" );
    basic_header_->setEventSamplerate(event_sample_rate);
    // calculate number of events
    uint32 number_events = 0;
    GDFEvent gdf_event;
    file_->seek(event_table_position_ + gdf_header_.number_of_events_ *
               sizeof(gdf_event.position));
    qDebug( "GDFReader::loadEventTableHeader() 6" );
    for (uint32 event_nr = 0; event_nr < gdf_header_.number_of_events_; event_nr++)
    {
        readStreamValue(gdf_event.type, *file_);
        qDebug( "GDFReader::loadEventTableHeader() 7" );
        if ((gdf_event.type & SignalEvent::EVENT_END) == 0)
        {
            number_events++;
        }
    }
    basic_header_->setNumberEvents(number_events);
    qDebug( "GDFReader::loadEventTableHeader() end" );
    return true;
}

void GDF2ReaderImpl::loadSignals (FileSignalReader::SignalDataBlockPtrIterator begin,
                                  FileSignalReader::SignalDataBlockPtrIterator end,
                                  uint32 start_record)
{
    if (!file_->isOpen())
    {
//        if (log_stream_)
//        {
//            *log_stream_ << "GDFReader::loadChannels Error: not open\n";
//        }
        return;
    }
    file_->seek(basic_header_->getRecordsPosition() + start_record * basic_header_->getRecordSize());
    bool something_done = true;
    for (uint32 rec_nr = start_record; something_done; rec_nr++)
    {
        bool rec_out_of_range = (rec_nr >= basic_header_->getNumberRecords());
        if (!rec_out_of_range)
        {
            readStreamValues(buffer_, *file_, basic_header_->getRecordSize());
        }
        something_done = false;
        uint32 position_in_buffer = 0;
        for (FileSignalReader::SignalDataBlockPtrIterator data_block = begin;
             data_block != end;
             data_block++)
        {
            if ((*data_block)->sub_sampling > 1 ||
                (*data_block)->channel_number >= basic_header_->getNumberChannels())
            {
//                if (log_stream_)
//                {
//                    *log_stream_ << "GDFReader::loadChannels Error: "
//                                 << "invalid SignalDataBlock\n";
//                }
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
            position_in_buffer += samples;
        }
    }
}
    
void GDF2ReaderImpl::loadEvents(FileSignalReader::SignalEventVector& event_vector)
{
    if (!file_->isOpen())
    {
//        if (log_stream_)
//        {
//            *log_stream_ << "GDFReader::loadEvents Error: not open\n";
//        }
        return;
    }

    // load all gdf-events
    file_->seek(event_table_position_);
    QVector<GDFEvent> gdf_events(gdf_header_.number_of_events_);
    QVector<GDFEvent>::iterator iter;
    for (iter = gdf_events.begin(); iter != gdf_events.end(); iter++)
    {
        readStreamValue(iter->position, *file_);
    }
    for (iter = gdf_events.begin(); iter != gdf_events.end(); iter++)
    {
        readStreamValue(iter->type, *file_);
    }
    if (gdf_header_.mode_of_event_table_ == GDF2Header::EXTENDED_EVENT_TABLE)
    {
        for (iter = gdf_events.begin(); iter != gdf_events.end(); iter++)
        {
            readStreamValue(iter->channel, *file_);
        }
        for (iter = gdf_events.begin(); iter != gdf_events.end(); iter++)
        {
            readStreamValue(iter->duration, *file_);
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
            if (gdf_header_.mode_of_event_table_ == GDF2Header::EXTENDED_EVENT_TABLE)
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

bool GDF2ReaderImpl::loadRawRecords(float64** record_data, uint32 start_record,
                        uint32 records)
{
    // TODO: implement
    return false;
}


}
