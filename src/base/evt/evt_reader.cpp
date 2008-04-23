// evt_reader.cpp

#include "evt_reader.h"
#include "../file_signal_reader_factory.h"
#include "../math_utils.h"
#include "../stream_utils.h"
#include "../gdf_event.h"

#include <algorithm>

#include <QTextStream>


namespace BioSig_
{

// constructor
  EVTReader::EVTReader()
  : file_(new QFile()),
    basic_header_ (new BasicHeader ()),
    buffer_(0)
                                 {
    // nothing
                                 }

// destructor
                                 EVTReader::~EVTReader()
                                 {
                                   if (file_->isOpen())
                                   {
                                     file_->close();
                                   }
                                   if (buffer_)
                                   {
                                     delete[] buffer_;
                                   }
                                 }

// clone
                                 FileSignalReader* EVTReader::clone()
                                 {
                                   FileSignalReader* new_instance = new EVTReader;
                                   new_instance->setLogStream(log_stream_);
                                   return new_instance;
                                 }

// check if open
                                 bool EVTReader::isOpen()
                                 {
                                   return file_->isOpen();
                                 }

                                 QPointer<BasicHeader> EVTReader::getBasicHeader ()
                                 {
                                   return basic_header_;
                                 }

// close
                                 void EVTReader::close()
                                 {
                                   if (file_->isOpen())
                                   {
                                     if (log_stream_)
                                     {
                                       *log_stream_ << "EVTReader::close '" << basic_header_->getFullFileName() << "'\n";
                                     }
                                     file_->close();
                                     basic_header_->resetBasicHeader();
                                   }
                                 }

// open
                                 QString EVTReader::open(const QString& file_name)
                                 {
                                   qDebug( "EVTReader::open(const QString& file_name) 1" );
                                   if (file_->isOpen())
                                   {
                                     if (log_stream_)
                                     {
                                       *log_stream_ << "EVTReader::open '" << file_name << "' Error: '"
                                           << basic_header_->getFullFileName() << "' not closed\n";
                                     }
                                     return "file not closed";
                                   }
                                   file_->setFileName(file_name);
                                   if (!file_->open(QIODevice::ReadOnly))
                                   {
                                     if (log_stream_)
                                     {
                                       *log_stream_ << "EVTReader::open '" << file_name
                                           << "' Error: reading file\n";
                                     }
                                     return "can't read file";
                                   }

    // read headers
                                   if (log_stream_)
                                   {
                                     *log_stream_ << "EVTReader::open '" << file_name << "'\n";
                                   }
                                   char version_id [8];
                                   readStreamChars(version_id, *file_, sizeof(version_id));
                                   if (strncmp(version_id, "GDF 1.", 6) != 0)
                                   {
                                     file_->close ();
                                     basic_header_->resetBasicHeader();
                                     return "can't read file format";
                                   }
    

                                   loadEventTableHeader ();

                                   return "";
                                 }


// load signals
                                 void EVTReader::loadSignals(SignalDataBlockPtrIterator begin,
                                     SignalDataBlockPtrIterator end,
                                     uint32 start_record)
                                     {
                                       // nothing to do
                                     }

// load events
                                     void EVTReader::loadEvents(SignalEventVector& event_vector)
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
                                         QVector<GDFEvent> gdf_events(number_events_);
                                         QVector<GDFEvent>::iterator iter;
                                         for (iter = gdf_events.begin(); iter != gdf_events.end(); iter++)
                                         {
                                           readStreamValue(iter->position, *file_);
                                         }
                                         for (iter = gdf_events.begin(); iter != gdf_events.end(); iter++)
                                         {
                                           readStreamValue(iter->type, *file_);
                                         }
                                         if (event_table_type_ == EXTENDED_EVENT_TABLE)
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
                                             if (event_table_type_ == EXTENDED_EVENT_TABLE)
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


                                     bool EVTReader::loadEventTableHeader ()
                                         {
                                           qDebug( "EVTReader::loadEventTableHeader() begin" );
                                           qDebug( "EVTReader::loadEventTableHeader() 1" );
                                           event_table_position_ = 256;
                                           file_->seek(event_table_position_);
                                           qDebug( "EVTReader::loadEventTableHeader() 2" );
                                           event_table_position_ += readStreamValue(event_table_type_,
                                               *file_);
                                           event_table_position_ += readStreamValues(event_table_sample_rate_,
                                               *file_, sizeof (event_table_sample_rate_));
                                           qDebug( "EVTReader::loadEventTableHeader() 3" );
                                           event_table_position_ +=
                                               readStreamValue(number_events_, *file_);
                                           qDebug( "EVTReader::loadEventTableHeader() %d", number_events_ );
                                           uint32 event_sample_rate = basic_header_->getEventSamplerate();
                                           if (event_sample_rate == 0)
                                           {
                                             event_sample_rate = ((uint32)event_table_sample_rate_[2] << 16) +
                                                 ((uint32)event_table_sample_rate_[1] << 8) +
                                                 event_table_sample_rate_[0];
                                           }
                                           else
                                           {
                                             event_table_sample_rate_[0] = (uint8)event_sample_rate;
                                             event_table_sample_rate_[1] = (uint8)(event_sample_rate << 8);
                                             event_table_sample_rate_[2] = (uint8)(event_sample_rate << 16);
                                           }
                                           qDebug( "EVTReader::loadEventTableHeader() 5" );
                                           basic_header_->setEventSamplerate(event_sample_rate);
    // calculate number of events
                                           uint32 number_events = 0;
                                           GDFEvent gdf_event;
                                           file_->seek(event_table_position_ + number_events_ *
                                               sizeof(gdf_event.position));
                                           qDebug( "EVTReader::loadEventTableHeader() 6" );
                                           for (uint32 event_nr = 0; event_nr < number_events_; event_nr++)
                                           {
                                             readStreamValue(gdf_event.type, *file_);
                                             qDebug( "EVTReader::loadEventTableHeader() 7" );
                                             if ((gdf_event.type & SignalEvent::EVENT_END) == 0)
                                             {
                                               number_events++;
                                             }
                                           }
                                           basic_header_->setNumberEvents(number_events);
                                           qDebug( "EVTReader::loadEventTableHeader() end" );
                                           return true;
                                         }
} // namespace BioSig_
