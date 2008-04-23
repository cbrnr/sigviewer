// evt_reader.h

#ifndef EVT_READER_H
#define EVT_READER_H

#include "../file_signal_reader.h"
#include "../basic_header.h"


#include <QFile>
#include <QPointer>
#include <memory>


namespace BioSig_
{

// EVT reader
  class EVTReader : public FileSignalReader
  {
    public:
      enum EventTableType
      {
        NORMAL_EVENT_TABLE = 1,
        EXTENDED_EVENT_TABLE = 3
      };
      
      EVTReader();
      virtual ~EVTReader();
      virtual FileSignalReader* clone();

      virtual QString open(const QString& file_name);
      virtual bool isOpen();
      virtual void close();

      virtual void loadSignals(SignalDataBlockPtrIterator begin, 
                               SignalDataBlockPtrIterator end,
                               uint32 start_record);
      virtual void loadEvents(SignalEventVector& event_vector);

      virtual QPointer<BasicHeader> getBasicHeader ();

    private:
    // not allowed
      EVTReader(const EVTReader& src);
      const EVTReader& operator=(const EVTReader& src);

      bool loadEventTableHeader ();

      QPointer<QFile> file_;
      QPointer<BasicHeader> basic_header_;
      int8* buffer_;
      
      uint32 event_table_position_;
      uint8 event_table_type_; 
      uint8 event_table_sample_rate_[3];
      uint32 number_events_;
  };

} // namespace BioSig_

#endif
