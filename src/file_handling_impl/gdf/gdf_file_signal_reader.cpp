#include "gdf_file_signal_reader.h"
#include "../file_handler_factory_registrator.h"
#include "gdf_data_block.h"

#include "GDF/EventConverter.h"

#include <QMessageBox>

namespace SigViewer_
{

//-------------------------------------------------------------------------------------------------
FILE_SIGNAL_READER_REGISTRATION(gdf, GDFFileSignalReader);


//-------------------------------------------------------------------------------------------------
GDFFileSignalReader::GDFFileSignalReader() : reader_ (0)
{
    // nothing to do here
}

//-------------------------------------------------------------------------------------------------
GDFFileSignalReader::~GDFFileSignalReader()
{
    if (reader_)
    {
        reader_->close ();
        delete reader_;
    }
    qDebug () << "deleting GDFFileSignalReader";
}

//-------------------------------------------------------------------------------------------------
FileSignalReader* GDFFileSignalReader::createInstance (QString const& file_path)
{
    GDFFileSignalReader* reader = new GDFFileSignalReader ();

    QString error = reader->open (file_path);
    if (error.size())
    {
        QMessageBox::critical(0, QObject::tr("Error"), error);
        return 0;
    }
    else
        return reader;
}

//-------------------------------------------------------------------------------------------------
QSharedPointer<DataBlock const> GDFFileSignalReader::getSignalData (ChannelID channel_id,
                                                       unsigned start_sample,
                                                       unsigned length) const
{
    if (!channel_map_.contains (channel_id))
    {
//        unsigned whole_channel_length = header_->getNumberOfSamples();
//        QSharedPointer<std::vector<float32> > raw_data (new std::vector<float32> (whole_channel_length));

//        double* buffer = new double[header_->getDownSamplingFactor ()];

//        double sample = 0;
//        for (unsigned index = 0; index < whole_channel_length; index++)
//        {
//            sample = 0;
//            reader_->getSignal (channel_id, buffer, index * header_->getDownSamplingFactor (), (index + 1) * header_->getDownSamplingFactor ());
//            for (int sub_index = 0; sub_index < header_->getDownSamplingFactor(); sub_index++)
//                sample += buffer[sub_index];
//            raw_data->operator [](index) = sample / header_->getDownSamplingFactor();
//        }

        // qDebug () << "header_->getDownSamplingFactor() = " << header_->getDownSamplingFactor();
        QSharedPointer<DataBlock const> data (new GDFDataBlock (cache_, channel_id,
                                                                reader_->getSignalHeader_readonly(channel_id).get_samples_per_record() * reader_->getMainHeader_readonly().get_num_datarecords(),
                                                                reader_->getSignalHeader_readonly(channel_id).get_samplerate(),
                                                                header_->getDownSamplingFactor()));//reader_->getSignalHeader_readonly(channel_id).get_samplerate()
                                                                       /// header_->getDownSamplingFactor ()));
        channel_map_[channel_id] = data;

//        delete[] buffer;
    }
    return channel_map_[channel_id]->createSubBlock (start_sample, length);
}

//-------------------------------------------------------------------------------------------------
QList<QSharedPointer<SignalEvent const> > GDFFileSignalReader::getEvents () const
{
    qDebug () << "Number events " << reader_->getEventHeader()->getNumEvents() << "; Event Mode = " << reader_->getEventHeader()->getMode();
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
