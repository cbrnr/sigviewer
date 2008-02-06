// basic_header.cpp

#include "basic_header.h"

namespace BioSig_
{

// constructor
BasicHeader::BasicHeader()
: type_(""),
  version_(""),
  reference_(""),
  triggered_(false),
  number_records_(0),
  record_duration_(0),
  record_size_(0),
  full_file_name_(""),
  file_size_(0),
  patient_name_(""),
  patient_age_(0),
  patient_sex_(UNDEFINED_SEX),
  patient_handedness_(""),
  patient_medication_(""),
  patient_classification_(""),
  doctor_id_(0),
  hospital_id_(0),
  number_channels_(0),
  records_position_(0),
  number_events_(0),
  event_sample_rate_(0),
  event_table_position_(0)
{
    // nothing
}

// destructor
BasicHeader::~BasicHeader()
{
    for (SignalChannelPtrVector::iterator it = channel_vector_.begin();
         it != channel_vector_.end();
         it++)
    {
        delete *it;
    }
}

// get type
const QString& BasicHeader::getType() const
{
    return type_;
}

// get version
const QString& BasicHeader::getVersion() const
{
    return version_;
}

// get recording time
const QDateTime& BasicHeader::getRecordingTime() const
{
    return recording_time_;
}

// get reference
const QString& BasicHeader::getReference() const
{
    return reference_;
}

// triggered
bool BasicHeader::isTriggered() const
{
    return triggered_;
}

// get number of records
int64 BasicHeader::getNumberRecords() const
{
    return number_records_;
}

// get record duration
float64 BasicHeader::getRecordDuration() const
{
    return record_duration_;
}

// get full file-name
const QString& BasicHeader::getFullFileName() const
{
    return full_file_name_;
}

// get file-name
const QString BasicHeader::getFileName() const
{
    int32 pos = full_file_name_.lastIndexOf(DIR_SEPARATOR);
    return pos == -1 ? full_file_name_ 
                     : full_file_name_.mid(pos + 1);
}

// get file-path
const QString BasicHeader::getFilePath() const
{
    int32 pos = full_file_name_.lastIndexOf(DIR_SEPARATOR);
    return pos == -1 ? full_file_name_.mid(0, 0)
                     : full_file_name_.mid(0, pos);
}

// get file-path
const QString BasicHeader::getFileExtension() const
{
    int32 pos = full_file_name_.lastIndexOf(".");
    return pos == -1 ? full_file_name_ 
                     : full_file_name_.mid(pos + 1);
}

// get file-size
uint32 BasicHeader::getFileSize() const
{
    return file_size_;
}

// get patient name
const QString& BasicHeader::getPatientName() const
{
    return patient_name_;
}

// get patient age
int32 BasicHeader::getPatientAge() const
{
    return patient_age_;
}

// get patient sex
BasicHeader::Sex BasicHeader::getPatientSex() const
{
    return patient_sex_;
}

// get patient handedness
const QString& BasicHeader::getPatientHandedness() const
{
    return patient_handedness_;
}

// get patient medication
const QString& BasicHeader::getPatientMedication() const
{
    return patient_medication_;
}

// get patient classification
const QString& BasicHeader::getPatientClassification() const
{
    return patient_classification_;
}

// get doctor-ID
uint64 BasicHeader::getDoctorId() const
{
    return doctor_id_;
}

// get hospitale-ID
uint64 BasicHeader::getHospitalId() const
{
    return hospital_id_;
}

// get number of channels
uint32 BasicHeader::getNumberChannels() const
{
    return number_channels_;
}


// get channel
const SignalChannel& BasicHeader::getChannel(uint32 channel_nr) const
{
    // TODO: check what hapens if no channel is found!!!!
    return channel_nr >= number_channels_ ? *(*channel_vector_.end())
                                          : *channel_vector_[channel_nr];
}

// get number of events
uint32 BasicHeader::getNumberEvents() const
{
    return number_events_;
}

// get event samplerate
uint32 BasicHeader::getEventSamplerate() const
{
    return event_sample_rate_;
}

// reset basic header
void BasicHeader::resetBasicHeader()
{
    type_ = "";
    version_ = "";
    recording_time_ = QDateTime();
    reference_ = "";
    triggered_ = false;
    number_records_ = 0;
    record_duration_ = 0;
    record_size_ = 0;
    full_file_name_ = "";
    file_size_ = 0;
    patient_name_ = "";
    patient_age_ = 0;
    patient_sex_ = UNDEFINED_SEX;
    patient_handedness_ = "";
    patient_medication_ = "";
    patient_classification_ = "";
    doctor_id_ = 0;
    hospital_id_ = 0;
    number_channels_ = 0;
    records_position_ = 0;
    number_events_ = 0;
    event_sample_rate_ = 0;
    event_table_position_ = 0;
    for (SignalChannelPtrVector::iterator it = channel_vector_.begin();
         it != channel_vector_.end();
         it++)
    {
        delete *it;
    }
    channel_vector_.clear();
}

} // namespace BioSig_
