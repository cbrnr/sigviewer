#include "gdf_1_reader_impl.h"
#include "../stream_utils.h"


namespace BioSig_ 
{

GDF1ReaderImpl::GDF1ReaderImpl(QPointer<QFile> file, QPointer<BasicHeader> basic_header)
: file_ (file),
  basic_header_ (basic_header)
{
    // nothing to do here
}

GDF1ReaderImpl::~GDF1ReaderImpl ()
{
    
}


bool GDF1ReaderImpl::loadFixedHeader ()
{
    //file_size_ = file_.size();
    
    // version already read
    
//    full_file_name_ = file_name;
//    type_ = "GDF";
//    version_ = gdf_version_id_;

    
    readStreamChars (gdf_header_.gdf_patient_id_, *file_, sizeof(gdf_header_.gdf_patient_id_));
    readStreamChars (gdf_header_.gdf_recording_id_, *file_, sizeof(gdf_header_.gdf_recording_id_));
    readStreamChars (gdf_header_.gdf_start_recording_, *file_, sizeof(gdf_header_.gdf_start_recording_));
    readStreamValue (gdf_header_.gdf_header_size_, *file_);
    readStreamValue (gdf_header_.gdf_equipment_provider_id_, *file_);
    readStreamValue (gdf_header_.gdf_labratory_id_, *file_);
    readStreamValue (gdf_header_.gdf_technican_id_, *file_);
    readStreamValues (gdf_header_.gdf_resered_, *file_, sizeof(gdf_header_.gdf_resered_));
    readStreamValue (gdf_header_.gdf_number_data_records_, *file_);
    readStreamValues (gdf_header_.gdf_duration_data_record_, *file_, sizeof(gdf_header_.gdf_duration_data_record_));
    readStreamValue (gdf_header_.gdf_number_signals_, *file_);
//    if (strncmp(gdf_version_id_, "GDF", 3) != 0)
//    {
//        return false;
//    }
//
//    recording_time_ = QDateTime::fromString(QString(gdf_start_recording_).left(12),
//                                            "yyyyMMddhhmmss");
//    number_records_ = gdf_number_data_records_;
//    record_duration_= (float64)gdf_duration_data_record_[0] /
//                               gdf_duration_data_record_[1];
//    patient_name_ = gdf_patient_id_;
//    patient_age_ = BasicHeader::UNDEFINED_AGE;
//    patient_sex_ = BasicHeader::UNDEFINED_SEX;
//    doctor_id_ = gdf_technican_id_;
//    hospital_id_ = gdf_labratory_id_;
//    // mistake in specification: 0x2020.. for undefined integer values
//    doctor_id_ =  doctor_id_ == 0x2020202020202020LL ? 0 : doctor_id_;
//    hospital_id_ =  hospital_id_ == 0x2020202020202020LL ? 0 : hospital_id_;
//    records_position_ = (uint32)gdf_header_size_;
//    number_channels_ = gdf_number_signals_;
    
    return true;
}

bool GDF1ReaderImpl::loadSignalHeaders ()
{
    return false; 
}

bool GDF1ReaderImpl::loadEventTableHeader ()
{
    return false;
}


}