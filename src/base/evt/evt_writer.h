// evt_writer.h

#ifndef EVT_WRITER_H
#define EVT_WRITER_H

#include "../file_signal_writer.h"

#include <QFile>

namespace BioSig_
{

// EVT writer
  class EVTWriter : public FileSignalWriter
  {
    public:
      enum EventTableType
      {
        NORMAL_EVENT_TABLE = 1,
        EXTENDED_EVENT_TABLE = 3
      };
      
      EVTWriter();
      virtual ~EVTWriter();
      virtual FileSignalWriter* clone();

      virtual QString save(FileSignalReader& file_signal_reader,
                           SignalEventVector& event_vector,
                           const QString& file_name,
                           bool save_signals = true);

    private:
    // not allowed
      EVTWriter(const EVTWriter& src);
      const EVTWriter& operator=(const EVTWriter& src);

      void saveFixedHeader(bool save_signals);
      void saveEvents(SignalEventVector& event_vector);

      QFile file_;
      FileSignalReader* file_signal_reader_;
      
      
      char gdf_version_id_[8];
      char gdf_patient_id_[80];
      char gdf_recording_id_[80];
      char gdf_start_recording_[16]; // YYYYMMDDhhmmsscc
      int64 gdf_header_size_;
      uint64 gdf_equipment_provider_id_;
      uint64 gdf_labratory_id_;
      uint64 gdf_technican_id_;
      char gdf_resered_[20];
      int64 gdf_number_data_records_; // -1 if unknown
      uint32 gdf_duration_data_record_[2]; // rational - nominator, denominator
      uint32 gdf_number_signals_;
      uint8 gdf_event_table_type_; 
      uint8 gdf_event_table_sample_rate_[3];
      uint32 gdf_number_events_;

  };

} // namespace BioSig_

#endif
