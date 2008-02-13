#include "bkr_reader.h"	
#include "../stream_utils.h"
#include "../signal_data_block.h"
#include "../math_utils.h"

#include <QTextStream>

// read character array from string
// dest must be an array e.g. char text[10];
#ifndef readStreamString
#define readStreamString(dest, src_stream) \
readStreamChars(dest, src_stream, sizeof(dest))
#endif

namespace BioSig_
{

//-----------------------------------------------------------------------------
BKRReader::BKRReader() :
    buffer_(0)
{

}

//-----------------------------------------------------------------------------
BKRReader::~BKRReader()
{
    if (file_.isOpen())
        file_.close();
    delete buffer_;
}

//-----------------------------------------------------------------------------
FileSignalReader* BKRReader::clone()
{
    BKRReader *new_instance = new BKRReader ();
    return new_instance;
}

//-----------------------------------------------------------------------------
bool BKRReader::open(const QString& file_name)
{
    qDebug( "BKRReader::open(const QString& file_name) 1" );
    if (file_.isOpen())
    {
        if (log_stream_)
        {
            *log_stream_ << "BKRReader::open '" << file_name << "' Error: '"
                        << full_file_name_ << "' not closed\n";
        }
        return false;
    }
    
    file_.setFileName(file_name);
    
    if (!file_.open(QIODevice::ReadOnly))
    {
        if (log_stream_)
        {
            *log_stream_ << "BKRReader::open '" << file_name
                        << "' Error: reading file\n";
        }
        return false;
    }
    

    
    // read headers
    if (log_stream_)
        *log_stream_ << "BKRReader::open '" << file_name << "'\n";
    
    if (!loadFixedHeader(file_name) /*||
        !loadSignalHeaders(file_name)*/)
    {
        file_.close();
//        resetBasicHeader();
//        resetGDFHeader();
        return false;
    }
//    loadEventTableHeader();
    
    return true;
}

//-----------------------------------------------------------------------------
bool BKRReader::isOpen()
{
    return file_.isOpen();
}

//-----------------------------------------------------------------------------
void BKRReader::close()
{

}

//-----------------------------------------------------------------------------
void BKRReader::loadSignals(SignalDataBlockPtrIterator begin,
                            SignalDataBlockPtrIterator end, uint32 start_record)
{
    if (!file_.isOpen())
    {
        if (log_stream_)
            *log_stream_ << "BKRReader::loadChannels Error: not open\n";
        return;
    }

    file_.seek(records_position_ + start_record * (record_size_ * number_channels_));
    bool something_done = true;
    for (uint32 rec_nr = start_record; something_done; rec_nr++)
    {
        bool rec_out_of_range = (rec_nr >= number_records_);
        if (!rec_out_of_range)
        {
            readStreamValues(buffer_, file_, record_size_ * bkr_number_channels_);
        }
        something_done = false;
        for (SignalDataBlockPtrIterator data_block = begin;
             data_block != end;
             ++data_block)
        {
            if ((*data_block)->sub_sampling > 1 ||
                (*data_block)->channel_number >= number_channels_)
            {
                if (log_stream_)
                {
                    *log_stream_ << "BKRReader::loadChannels Error: "
                                 << "invalid SignalDataBlock\n";
                }
                continue;
            }
            SignalChannel* sig = channel_vector_[(*data_block)->channel_number];
            
            qDebug ("reading signal for channel " + (*data_block)->channel_number);
            
            uint32 samples = sig->getSamplesPerRecord();
            uint32 actual_sample = (rec_nr - start_record) * samples;
            if (actual_sample + samples > (*data_block)->number_samples)
            {
                // sample file doesnt enter here
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
                // sample file doesnt enter here
                for (uint32 samp = actual_sample;
                     samp < actual_sample + samples;
                     samp++)
                {
                    data_block_buffer_valid[samp] = false;
                }
            }
            else
            {
                convertData(buffer_ + ((*data_block)->channel_number * 2), &data_block_buffer[actual_sample],
                            *sig, samples, true);
                //memcpy (&data_block_buffer[actual_sample], buffer_, 2);
                for (uint32 samp = actual_sample;
                     samp < actual_sample + samples;
                     samp++)
                {
                    data_block_upper_buffer[samp] = data_block_buffer[samp];
                    data_block_lower_buffer[samp] = data_block_buffer[samp];
                    data_block_buffer_valid[samp] = true;
                        //= data_block_buffer[samp] > sig->getPhysicalMinimum() &&
                        //  data_block_buffer[samp] < sig->getPhysicalMaximum();
                }
            }
        }
    }    
}

//-----------------------------------------------------------------------------
void BKRReader::loadEvents(SignalEventVector& event_vector)
{
    // TODO: are there events in BKR files ????
}

//-----------------------------------------------------------------------------
void BKRReader::loadRawRecords(float64** record_data, uint32 start_record,
                               uint32 records)
{

}

//-----------------------------------------------------------------------------
bool BKRReader::loadFixedHeader(const QString& file_name)
{
    file_size_ = file_.size();
    
    readStreamValue (bkr_version_, file_);
    readStreamValue (bkr_number_channels_, file_);
    readStreamValue (bkr_sample_frequency_, file_);
    readStreamValue (bkr_number_trials_, file_);
    readStreamValue (bkr_samples_per_trial_, file_);
    readStreamValue (bkr_calibration_voltage_, file_);
    readStreamValue (bkr_calibration_value_, file_);
    readStreamValues (bkr_electrode_code_, file_, 4);
    readStreamValue (bkr_cut_off_low_frequency_, file_);
    readStreamValue (bkr_cut_off_high_frequency_, file_);
    readStreamValue (bkr_reference_periode_start_, file_);
    readStreamValue (bkr_reference_periode_end_, file_);
    readStreamValue (bkr_action_period_start_, file_);
    readStreamValue (bkr_action_period_end_, file_);
    readStreamValue (bkr_triggered_, file_);
    readStreamValue (bkr_trigger_samples_before_, file_);
    readStreamValue (bkr_trigger_samples_after_, file_);
    readStreamValue (bkr_horizontally_averaged_, file_);
    readStreamValue (bkr_horizontally_averaged_trials_, file_);
    readStreamValue (bkr_vertically_averaged_, file_);
    readStreamValue (bkr_vertically_averaged_trials_, file_);
    readStreamValue (bkr_compression_, file_);
    readStreamValue (bkr_compressed_samples_, file_);
    readStreamValue (bkr_common_average_reference_, file_);
    readStreamValue (bkr_local_average_reference_, file_);
    readStreamValue (bkr_laplace_, file_);
    readStreamValue (bkr_weighted_average_reference_, file_);
    readStreamValue (bkr_power_, file_);
    readStreamValue (bkr_average_, file_);
    readStreamValue (bkr_standrard_deviation_, file_);
    readStreamValue (bkr_bandpass_filtered_, file_);
    readStreamValue (bkr_erd_, file_);
    readStreamValue (bkr_significance_, file_);
    readStreamValue (bkr_coherence_, file_);
    readStreamValue (bkr_specturm_, file_);
    readStreamValue (bkr_confidance_, file_);
    readStreamValue (bkr_coherence_power_specturm_, file_);
    readStreamValue (bkr_erc_, file_);
    readStreamValue (bkr_hanning_smoothed_, file_);
    readStreamValue (bkr_neuronal_network_filter_, file_);
    readStreamValue (bkr_number_input_units_, file_);
    readStreamValue (bkr_number_hidden_units_, file_);
    readStreamValue (bkr_number_learn_cycles_, file_);
    readStreamValue (bkr_regression_, file_);
    readStreamValue (bkr_learn_coefficient_, file_);
    readStreamValue (bkr_epoch_, file_);
    readStreamValue (bkr_erc_relative_values_, file_);
    readStreamValue (bkr_window_type_, file_);
    readStreamValue (bkr_kalman_filtered_, file_);
    readStreamValue (bkr_wavelet_transformed_, file_);
    readStreamValue (bkr_lower_frequency_wavelet_, file_);
    readStreamValue (bkr_upper_frequency_wavelet_, file_);
    readStreamValue (bkr_number_frequency_bands_wavelet_, file_);
    readStreamValue (bkr_frequency_wavelet_, file_);
    readStreamValue (bkr_start_sample_wavelet_, file_);
    
    full_file_name_ = file_name; 
    type_ = "BKR";
    version_ = bkr_version_;
    number_channels_ = bkr_number_channels_;
    number_records_ = bkr_samples_per_trial_ * bkr_number_trials_;
    record_size_ = 2; // 2 bytes = 1 sample
    records_position_ = 1024;
    record_duration_ = 1;
    record_duration_ /= bkr_sample_frequency_;
    
    if (bkr_triggered_)
        qDebug ("bkr_triggered_ is true");
    
    // TODO: set more headers info!!!
    
    
    // channels...
    if (number_channels_ > 0)
    {
        //event_sample_rate_ = 1;
        //sig = bkr_sig_vector_.begin();
        for (uint32 channel_nr = 0;
             channel_nr < number_channels_;
             ++channel_nr)
        {
            SignalChannel* channel = new SignalChannel(channel_nr, "ups",
                                                       1, // samples per record
                                                       "µV",//sig->physical_dimension,
                                                       0, //sig->physical_minimum,
                                                       bkr_calibration_voltage_, //sig->physical_maximum,
                                                       0, //sig->digital_minimum,
                                                       bkr_calibration_value_, //sig->digital_maximum,
                                                       3, //sig->channel_type,
                                                       0, //record_size_ / 16,
                                                       "filer", -1, -1, false);

            channel_vector_ << channel;
            //event_sample_rate_ = lcm(event_sample_rate_,
//                                     sig->samples_per_record);
            //record_size_ += channel->typeBitSize() *
            //                sig->samples_per_record;
            // TODO : signal record-size no multiple of 8 bits
//            if (record_size_ % 8 != 0)
//            {
//                if (log_stream_)
//                {
//                    *log_stream_ << "GDFReader::open '" << file_name
//                                 << "' Error: signal record size no multiple of "
//                                 << "8 bits is not supported\n";
//                }
//                return false;
//            }
        }
//        event_sample_rate_ *= gdf_duration_data_record_[1];
        
//        if (event_sample_rate_ % gdf_duration_data_record_[0] != 0)
//        {
//            if (log_stream_)
//            {
//                *log_stream_ << "GDFReader::open Warning: event-samplerate is"
//                             << " non integer value\n";
//            }
//        }
//        event_sample_rate_ /= gdf_duration_data_record_[0];
//        record_size_ = (record_size_ + 7) / 8;
        buffer_ = new int8[record_size_ * number_channels_];
    }
    
    return true;
}

//-----------------------------------------------------------------------------
bool BKRReader::loadSignalHeaders(const QString& file_name)
{
    file_.seek(BKRHeader::SIZE);
//    gdf_sig_vector_.resize(gdf_number_signals_);
//    GDFSignalHeaderVector::iterator sig;
//    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
//    {
//        readStreamString(sig->label, file_);
//    }
//    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
//    {
//        readStreamString(sig->transducer_type, file_);
//    }
//    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
//    {
//        readStreamString(sig->physical_dimension, file_);
//    }
//    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
//    {
//        readStreamValue(sig->physical_minimum, file_);
//    }
//    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
//    {
//        readStreamValue(sig->physical_maximum, file_);
//    }
//    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
//    {
//        readStreamValue(sig->digital_minimum, file_);
//    }
//    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
//    {
//        readStreamValue(sig->digital_maximum, file_);
//    }
//    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
//    {
//        readStreamString(sig->pre_filtering, file_);
//    }
//    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
//    {
//        readStreamValue(sig->samples_per_record, file_);
//    }
//    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
//    {
//        readStreamValue(sig->channel_type, file_);
//    }
//    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
//    {
//        readStreamArray(sig->reserved, file_);
//    }
//    
//    record_size_ = 0;
//    if (number_channels_ > 0)
//    {
//        event_sample_rate_ = 1;
//        sig = gdf_sig_vector_.begin();
//        for (uint32 channel_nr = 0;
//             sig != gdf_sig_vector_.end();
//             sig++, channel_nr++)
//        {
//            SignalChannel* channel = new SignalChannel(channel_nr, sig->label,
//                                                       sig->samples_per_record,
//                                                       sig->physical_dimension,
//                                                       sig->physical_minimum,
//                                                       sig->physical_maximum,
//                                                       sig->digital_minimum,
//                                                       sig->digital_maximum,
//                                                       sig->channel_type,
//                                                       record_size_ / 8,
//                                                       sig->pre_filtering, -1, -1, false);
//
//            channel_vector_ << channel;
//            event_sample_rate_ = lcm(event_sample_rate_,
//                                     sig->samples_per_record);
//            record_size_ += channel->typeBitSize() *
//                            sig->samples_per_record;
//            // TODO : signal record-size no multiple of 8 bits
//            if (record_size_ % 8 != 0)
//            {
//                if (log_stream_)
//                {
//                    *log_stream_ << "GDFReader::open '" << file_name
//                                 << "' Error: signal record size no multiple of "
//                                 << "8 bits is not supported\n";
//                }
//                return false;
//            }
//        }
//        event_sample_rate_ *= gdf_duration_data_record_[1];
//        
//        if (event_sample_rate_ % gdf_duration_data_record_[0] != 0)
//        {
//            if (log_stream_)
//            {
//                *log_stream_ << "GDFReader::open Warning: event-samplerate is"
//                             << " non integer value\n";
//            }
//        }
//        event_sample_rate_ /= gdf_duration_data_record_[0];
//        record_size_ = (record_size_ + 7) / 8;
//        buffer_ = new int8[record_size_];
//    }
    return true;
}

}
