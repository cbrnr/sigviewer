// evt_writer.cpp

#include "evt_writer.h"
//#include "gdf_signal_header.h"
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
  EVTWriter::EVTWriter()
  : file_signal_reader_(0)
  {
    // nothing
  }

// destructor
  EVTWriter::~EVTWriter()
  {
    // nothing
  }

// clone
  FileSignalWriter* EVTWriter::clone()
  {
    FileSignalWriter* new_instance = new EVTWriter;
    new_instance->setLogStream(log_stream_);
    return new_instance;
  }

// save
  QString EVTWriter::save(FileSignalReader& file_signal_reader,
                          SignalEventVector& event_vector,
                          const QString& file_name, bool save_signals)
                          {
                            file_signal_reader_ = &file_signal_reader;
                            QString save_file_name = file_name;
                            if (file_name == file_signal_reader_->getBasicHeader()->getFullFileName())
                            {
                              if (!save_signals)
                              {
                                if (log_stream_)
                                {
                                  *log_stream_ << "EVTWriter::save '" << save_file_name
                                      << "' Error: cannot remove signal data from open file\n";
                                }
                                return "cannot remove signal data from open file";
                              }
                              save_file_name += ".tmp";
                            }

    // open file
                            file_.setFileName(save_file_name);
                            if (!file_.open(QIODevice::WriteOnly))
                            {
                              if (log_stream_)
                              {
                                *log_stream_ << "EVTWriter::save '" << save_file_name
                                    << "' Error: writing file\n";
                              }
                              return "writing file";
                            }

    // save data
                            if (log_stream_)
                            {
                              *log_stream_ << "GDFWriter::save '" << save_file_name << "'\n";
                            }
                            saveFixedHeader(save_signals);
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

                            return "";
                          }

// save fixed header
                          void EVTWriter::saveFixedHeader(bool save_signals)
                          {
    // get header from signal reader
                            strcpy(gdf_version_id_, "GDF 1.25");
                            strncpy(gdf_patient_id_, file_signal_reader_->getBasicHeader()->getPatientName().toAscii().data(),
                                    sizeof(gdf_patient_id_));
                            QString tmp = file_signal_reader_->getBasicHeader()->getRecordingTime().toString("yyyyMMddhhmmss") + "00";
                            memcpy(gdf_start_recording_, tmp.toAscii().data(), 16);
  
                            gdf_equipment_provider_id_ = 0x0553696756696577LL;
                            gdf_labratory_id_ = file_signal_reader_->getBasicHeader()->getHospitalId();
                            gdf_technican_id_ = file_signal_reader_->getBasicHeader()->getDoctorId();
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
                          void EVTWriter::saveEvents(SignalEventVector& event_vector)
                          {
    // get event tabel header from file signal reader
                            gdf_event_table_type_ = EXTENDED_EVENT_TABLE;
                            uint32 tmp = file_signal_reader_->getBasicHeader()->getEventSamplerate();
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
