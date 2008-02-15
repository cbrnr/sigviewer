// gdf_writer.cpp

#include "gdf_writer.h"
#include "gdf_signal_header.h"
#include "../stream_utils.h"
#include "../signal_data_block.h"
#include "../math_utils.h"

#include <QTextStream>

// write character array from string
// src must be an array e.g. char text[10];
#define writeStreamString(dest_stream, src) \
writeStreamChars(dest_stream, src, sizeof(src))

// write array data from stream
// src must be an array e.g. int a[10]
#define writeStreamArray(dest_stream, src) \
writeStreamValues(dest_stream, src, sizeof(src))

namespace BioSig_
{

// constructor
GDFWriter::GDFWriter()
: file_signal_reader_(0)
{
    // nothing
}

// destructor
GDFWriter::~GDFWriter()
{
    // nothing
}

// clone
FileSignalWriter* GDFWriter::clone()
{
    FileSignalWriter* new_instance = new GDFWriter;
    new_instance->setLogStream(log_stream_);
    return new_instance;
}

// save
bool GDFWriter::save(FileSignalReader& file_signal_reader,
                     SignalEventVector& event_vector,
                     const QString& file_name, bool save_signals)
{
    file_signal_reader_ = &file_signal_reader;
    QString save_file_name = file_name;
    if (file_name == file_signal_reader_->getFullFileName())
    {
        if (!save_signals)
        {
            if (log_stream_)
            {
                *log_stream_ << "GDFWriter::save '" << save_file_name
                             << "' Error: cannot remove signal data from open file\n";
            }
            return false;
        }
        save_file_name += ".tmp";
    }

    // open file
    file_.setFileName(save_file_name);
    if (!file_.open(QIODevice::WriteOnly))
    {
        if (log_stream_)
        {
            *log_stream_ << "GDFWriter::save '" << save_file_name
                         << "' Error: writing file\n";
        }
        return false;
    }

    // save data
    if (log_stream_)
    {
        *log_stream_ << "GDFWriter::save '" << save_file_name << "'\n";
    }
    saveFixedHeader(save_signals);
    if (save_signals)
    {
        saveSignalHeaders();
        if (!saveSignals())
        {
           *log_stream_ << "GDFWriter::save '" << save_file_name
                         << "' Error: can't save signals\n";
           return false;
        }
    }
    saveEvents(event_vector);
    file_.close();

    if (save_file_name != file_name)
    {
        file_signal_reader_->close();
        file_.setFileName(file_name);
        file_.remove();
        file_.setFileName(save_file_name);
        file_.rename(file_name);
        file_signal_reader_->open(file_name);
    }

    return true;
}

// save fixed header
void GDFWriter::saveFixedHeader(bool save_signals)
{
    // get header from signal reader
    strcpy(gdf_version_id_, "GDF 1.25");
    strncpy(gdf_patient_id_, file_signal_reader_->getPatientName().toAscii().data(),
            sizeof(gdf_patient_id_));
    QString tmp = file_signal_reader_->getRecordingTime().toString("yyyyMMddhhmmss") + "00";
    memcpy(gdf_start_recording_, tmp.toAscii().data(), 16);
    if (save_signals)
    {
        gdf_header_size_ += file_signal_reader_->getNumberChannels() *
                            GDF1Header::SIZE;
    }
    gdf_equipment_provider_id_ = 0x0553696756696577LL;
    gdf_labratory_id_ = file_signal_reader_->getHospitalId();
    gdf_technican_id_ = file_signal_reader_->getDoctorId();
    if (save_signals)
    {
        gdf_number_data_records_ = file_signal_reader_->getNumberRecords();
        float2rational(file_signal_reader_->getRecordDuration(),
                       gdf_duration_data_record_[0],
                       gdf_duration_data_record_[1]);
        gdf_number_signals_ = file_signal_reader_->getNumberChannels();
    }

    // write header
    writeStreamString(file_, gdf_version_id_);
    writeStreamString(file_, gdf_patient_id_);
    writeStreamString(file_, gdf_recording_id_);
    writeStreamString(file_, gdf_start_recording_);
    writeStreamValue(file_, gdf_header_size_);
    writeStreamValue(file_, gdf_equipment_provider_id_);
    writeStreamValue(file_, gdf_labratory_id_);
    writeStreamValue(file_, gdf_technican_id_);
    writeStreamArray(file_, gdf_resered_);
    writeStreamValue(file_, gdf_number_data_records_);
    writeStreamArray(file_, gdf_duration_data_record_);
    writeStreamValue(file_, gdf_number_signals_);
}

// save signal headers
void GDFWriter::saveSignalHeaders()
{
    // get signal headers from file signal reader
    uint32 number_channels = file_signal_reader_->getNumberChannels();
    for (uint32 channel_nr = 0; channel_nr < number_channels; channel_nr++)
    {
        const SignalChannel& channel
            = file_signal_reader_->getChannel(channel_nr);
        GDFSignalHeader gdf_signal_header;
        strncpy(gdf_signal_header.label, channel.getLabel().toAscii().data(),
                sizeof(gdf_signal_header.label));
        gdf_signal_header.samples_per_record = channel.getSamplesPerRecord();
        strncpy(gdf_signal_header.physical_dimension,
                channel.getPhysicalDim().toAscii().data(),
                sizeof(gdf_signal_header.physical_dimension));
        gdf_signal_header.physical_maximum = channel.getPhysicalMaximum();
        gdf_signal_header.digital_maximum = channel.getDigitalMaximum();
        gdf_signal_header.physical_minimum = channel.getPhysicalMinimum();
        gdf_signal_header.digital_minimum = channel.getDigitalMinimum();
        strncpy(gdf_signal_header.pre_filtering,
                channel.getFilterLabel().toAscii().data(),
                sizeof(gdf_signal_header.pre_filtering));
        gdf_signal_header.channel_type = channel.getDataType();
        memset(gdf_signal_header.reserved, ' ',
               sizeof(gdf_signal_header.reserved));
        gdf_sig_vector_.push_back(gdf_signal_header);
    }

    // write signal headers
    GDFSignalHeaderVector::iterator sig;
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        writeStreamString(file_, sig->label);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        writeStreamString(file_, sig->transducer_type);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        writeStreamString(file_, sig->physical_dimension);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        writeStreamValue(file_, sig->physical_minimum);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        writeStreamValue(file_, sig->physical_maximum);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        writeStreamValue(file_, sig->digital_minimum);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        writeStreamValue(file_, sig->digital_maximum);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        writeStreamString(file_, sig->pre_filtering);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        writeStreamValue(file_, sig->samples_per_record);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        writeStreamValue(file_, sig->channel_type);
    }
    for (sig = gdf_sig_vector_.begin(); sig != gdf_sig_vector_.end(); sig++)
    {
        writeStreamArray(file_, sig->reserved);
    }
}

// convert data
template<typename SrcType>
inline void convertData(SrcType* src, int8* dest, const SignalChannel& sig)
{
    uint32 samples = sig.getSamplesPerRecord();
    dest += sig.getDataOffset();
    if (sig.getDataType() > SignalChannel::UBITN)
    {
        // TODO
/*
        // TODO : signal record-size no multiple of 8 bits
        uint32 bits = sig.data_type - SignalChannel::UBITN;
        copyInterpolatedUNBitArray(dest, buffer, bits, scale, offset, samples);
*/
    }
    else if (sig.getDataType() > SignalChannel::BITN)
    {
        // TODO
/*
        // TODO : signal record-size no multiple of 8 bits
        uint32 bits = sig.data_type - SignalChannel::UBITN;
        copyInterpolatedSNBitArray(dest, buffer, bits, scale, offset, samples);
*/
    }
    else switch(sig.getDataType())
    {
        case SignalChannel::INT8:
            copyArray((int8*)dest, src, samples);
            break;
        case SignalChannel::UINT8:
            copyArray((uint8*)dest, src, samples);
            break;
        case SignalChannel::INT16:
            copyArray((int16*)dest, src, samples);
            break;
        case SignalChannel::UINT16:
            copyArray((uint16*)dest, src, samples);
            break;
        case SignalChannel::INT32:
            copyArray((int32*)dest, src, samples);
            break;
        case SignalChannel::UINT32:
            copyArray((uint32*)dest, src, samples);
            break;
        case SignalChannel::INT64:
            copyArray((int64*)dest, src, samples);
            break;
        case SignalChannel::FLOAT32:
            copyArray((float32*)dest, src, samples);
            break;
        case SignalChannel::FLOAT64:
            copyArray((float64*)dest, src, samples);
            break;
    }
}

// save signals
bool GDFWriter::saveSignals()
{
    // allocate buffers
    int32 record_size = 0;
    uint32 number_channels = file_signal_reader_->getNumberChannels();
    float64** float_buffer = new float64*[number_channels];
    for (uint32 channel_nr = 0; channel_nr < number_channels; channel_nr++)
    {
        const SignalChannel& channel
            = file_signal_reader_->getChannel(channel_nr);
        float_buffer[channel_nr] = new float64[channel.getSamplesPerRecord()];
        record_size += channel.typeBitSize() / 8;
    }
    int8* record_buffer = new int8[record_size];

    // save all records
    uint32 number_records = file_signal_reader_->getNumberRecords();
    for (uint32 record_nr = 0; record_nr < number_records; record_nr++)
    {
        if (!file_signal_reader_->loadRawRecords(float_buffer, record_nr, 1))
            return false;
        for (uint32 channel_nr = 0; channel_nr < number_channels; channel_nr++)
        {
            const SignalChannel& channel
                = file_signal_reader_->getChannel(channel_nr);
            convertData(float_buffer[channel_nr], record_buffer, channel);
        }
        writeStreamValues(file_, record_buffer, record_size);
    }

    // delete buffers
    delete[] record_buffer;
    for (uint32 channel_nr = 0; channel_nr < number_channels; channel_nr++)
    {
        delete[] float_buffer[channel_nr];
    }
    delete[] float_buffer;
    return true;
}

// save events
void GDFWriter::saveEvents(SignalEventVector& event_vector)
{
    // get event tabel header from file signal reader
    gdf_event_table_type_ = EXTENDED_EVENT_TABLE;
    uint32 tmp = file_signal_reader_->getEventSamplerate();
    gdf_event_table_sample_rate_[0] = (uint8)tmp;
    gdf_event_table_sample_rate_[1] = (uint8)(tmp >> 8);
    gdf_event_table_sample_rate_[2] = (uint8)(tmp >> 16);
    gdf_number_events_ = event_vector.size();

    // write event table and events
    writeStreamValue(file_, gdf_event_table_type_);
    writeStreamArray(file_, gdf_event_table_sample_rate_);
    writeStreamValue(file_, gdf_number_events_);
    SignalEventVector::iterator iter;
    for (iter = event_vector.begin(); iter != event_vector.end(); iter++)
    {
        writeStreamValue(file_, iter->getPosition());
    }
    for (iter = event_vector.begin(); iter != event_vector.end(); iter++)
    {
        writeStreamValue(file_, iter->getType());
    }
    for (iter = event_vector.begin(); iter != event_vector.end(); iter++)
    {
        writeStreamValue(file_, (uint16)(iter->getChannel() + 1));
    }
    for (iter = event_vector.begin(); iter != event_vector.end(); iter++)
    {
        writeStreamValue(file_, iter->getDuration());
    }
}

} // namespace BioSig_
