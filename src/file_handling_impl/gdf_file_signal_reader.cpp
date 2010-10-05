#include "gdf_file_signal_reader.h"
#include "file_handler_factory_registrator.h"

#include "GDF/Reader.h"

namespace SigViewer_
{

//-------------------------------------------------------------------------------------------------
//FILE_SIGNAL_READER_REGISTRATION(gdf, GDFFileSignalReader);


//-------------------------------------------------------------------------------------------------
GDFFileSignalReader::GDFFileSignalReader()
{
}

//-------------------------------------------------------------------------------------------------
GDFFileSignalReader::~GDFFileSignalReader()
{

}

//-------------------------------------------------------------------------------------------------
QSharedPointer<FileSignalReader> GDFFileSignalReader::createInstance (QString const& file_path)
{

}

//-------------------------------------------------------------------------------------------------
QSharedPointer<DataBlock const> GDFFileSignalReader::getSignalData (ChannelID channel_id,
                                                       unsigned start_sample,
                                                       unsigned length) const
{

}

//-------------------------------------------------------------------------------------------------
QList<QSharedPointer<SignalEvent const> > GDFFileSignalReader::getEvents () const
{

}

//-------------------------------------------------------------------------------------------------
QSharedPointer<BasicHeader> GDFFileSignalReader::getBasicHeader ()
{

}



} // namespace
