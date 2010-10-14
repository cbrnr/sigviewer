#include "gdf_file_signal_reader.h"
#include "file_handler_factory_registrator.h"

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
        double* buffer = new double[header_->getNumberOfSamples()];
        reader_->getSignal (channel_id, buffer, 0, header_->getNumberOfSamples());

        QSharedPointer<std::vector<float32> > raw_data (new std::vector<float32> (length));
        for (unsigned index = 0; index <= header_->getNumberOfSamples(); index++)
            raw_data->operator [](index) = buffer[index];

        QSharedPointer<DataBlock const> data (new DataBlock (raw_data, reader_->getSignalHeader_readonly(channel_id).get_samplerate()));

        delete buffer;
        channel_map_[channel_id] = data;
    }
    return channel_map_[channel_id]->createSubBlock (start_sample, length);
}

//-------------------------------------------------------------------------------------------------
QList<QSharedPointer<SignalEvent const> > GDFFileSignalReader::getEvents () const
{
    return QList<QSharedPointer<SignalEvent const> > ();
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
    try
    {
        reader_->open (file_path.toStdString());
    }
    catch (std::exception& exc)
    {
        return QString (exc.what ());
    }
    header_ = QSharedPointer<GDFBasicHeader> (new GDFBasicHeader (file_path, reader_->getHeaderAccess_readonly()));
    return "";
}





} // namespace
