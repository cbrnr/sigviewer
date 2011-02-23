#include "gdf_file_signal_reader.h"
#include "../file_handler_factory_registrator.h"
#include "gdf_data_block.h"

#include "GDF/EventConverter.h"

#include <QMessageBox>

namespace SigViewer_
{

//-------------------------------------------------------------------------------------------------
//FILE_SIGNAL_READER_REGISTRATION(gdf, GDFFileSignalReader);


//-------------------------------------------------------------------------------------------------
GDFFileSignalReader::GDFFileSignalReader() : reader_ (0), downsampling_thread_ (0)
{
    // nothing to do here
}

//-------------------------------------------------------------------------------------------------
GDFFileSignalReader::~GDFFileSignalReader()
{
    if (downsampling_thread_)
    {
        downsampling_thread_->terminate();
        downsampling_thread_->wait();
    }
    delete downsampling_thread_;
    if (reader_)
    {
        reader_->close ();
        delete reader_;
    }
    qDebug () << "deleting GDFFileSignalReader";
}

//-------------------------------------------------------------------------------------------------
QPair<FileSignalReader*, QString> GDFFileSignalReader::createInstance (QString const& file_path)
{
    GDFFileSignalReader* reader = new GDFFileSignalReader ();

    QString error = reader->open (file_path);
    if (error.size())
        return QPair<FileSignalReader*, QString> (0, error);
    else
        return QPair<FileSignalReader*, QString> (reader, "");
}

//-------------------------------------------------------------------------------------------------
QSharedPointer<DataBlock const> GDFFileSignalReader::getSignalData (ChannelID channel_id,
                                                       unsigned start_sample,
                                                       unsigned length) const
{
    if (channel_map_.size() == 0)
    {
        QList<QSharedPointer<DataBlock> > data_for_downsampling;
        unsigned max_channel_length = 0;
        for (unsigned channel_index = 0; channel_index < reader_->getMainHeader_readonly().get_num_signals(); channel_index++)
        {
            unsigned current_channel_length = reader_->getSignalHeader_readonly(channel_index).get_samples_per_record() * reader_->getMainHeader_readonly().get_num_datarecords();
            max_channel_length = std::max (max_channel_length, current_channel_length);
            QSharedPointer<GDFDataBlock> data (new GDFDataBlock (cache_, channel_index,
                                                                    current_channel_length,
                                                                    reader_->getSignalHeader_readonly(channel_index).get_samplerate()));
            channel_map_[channel_index] = data;
            data_for_downsampling.append (data);
        }
        downsampling_thread_ = new DownSamplingThread (data_for_downsampling, 3, max_channel_length / 2000);
        downsampling_thread_->start (QThread::LowestPriority);
    }
    return channel_map_[channel_id]->createSubBlock (start_sample, length);
}

//-------------------------------------------------------------------------------------------------
QList<QSharedPointer<SignalEvent const> > GDFFileSignalReader::getEvents () const
{
    try
    {
        //qDebug () << "Number events " << reader_->getEventHeader()->getNumEvents() << "; Event Mode = " << reader_->getEventHeader()->getMode();
        if (events_.size ())
            return events_;

        std::vector<gdf::Mode3Event> mode_3_events;
        if (reader_->getEventHeader()->getMode() == 1)
            mode_3_events = gdf::convertMode1EventsIntoMode3Events (reader_->getEventHeader()->getMode1Events());
        else
            mode_3_events = reader_->getEventHeader()->getMode3Events();

        for (unsigned index = 0; index < mode_3_events.size (); index++)
        {
            gdf::Mode3Event& gdf_event = mode_3_events[index];
            QSharedPointer<SignalEvent const> event (new SignalEvent (gdf_event.position / header_->getDownSamplingFactor(), gdf_event.type, reader_->getEventHeader()->getSamplingRate() / header_->getDownSamplingFactor(),
                                                                      gdf_event.channel - 1, gdf_event.duration / header_->getDownSamplingFactor()));
            events_.push_back (event);
        }
    }
    catch (...)
    {
        //QMessageBox::critical (0, "Error", tr(exc.what()));
    }
    return events_;
}

//-------------------------------------------------------------------------------------------------
QSharedPointer<BasicHeader> GDFFileSignalReader::getBasicHeader ()
{
    return header_;
}

//-------------------------------------------------------------------------------------------------
QString GDFFileSignalReader::open (QString const& file_path)
{
    if (reader_)
        delete reader_;
    reader_ = new gdf::Reader ();
    reader_->enableCache (false);
    try
    {
        reader_->open (file_path.toStdString());
    }
    catch (std::exception& exc)
    {
        return QString (exc.what ());
    }
    header_ = QSharedPointer<GDFBasicHeader> (new GDFBasicHeader (file_path, reader_->getHeaderAccess_readonly()));
    cache_ = QSharedPointer<GDFSignalCache> (new GDFSignalCache (reader_));
    return "";
}





} // namespace
