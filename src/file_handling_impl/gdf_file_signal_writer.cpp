#include "gdf_file_signal_writer.h"
#include "file_handling/file_signal_writer_factory.h"
#include "file_handler_factory_registrator.h"
//#include "gdfwriter.h"

#include <QDebug>

namespace BioSig_
{

//FILE_SIGNAL_WRITER_REGISTRATION(gdf, GDFFileSignalWriter);

//-------------------------------------------------------------------------
GDFFileSignalWriter::GDFFileSignalWriter()
{
    qDebug() << "GDFFileSignalWriter constructed!";
}

//-------------------------------------------------------------------------
GDFFileSignalWriter::GDFFileSignalWriter (QString const& file_path)
    : new_file_path_ (file_path)
{
    // nothing to do here
}

//-------------------------------------------------------------------------
QSharedPointer<FileSignalWriter> GDFFileSignalWriter::createInstance (QString const& file_path)
{
    return QSharedPointer<FileSignalWriter> (new GDFFileSignalWriter (file_path));
}


//-------------------------------------------------------------------------
QString GDFFileSignalWriter::saveEventsToSignalFile (QSharedPointer<EventManager const>,
                                        std::set<EventType> const&)
{
    return "";
}

//-------------------------------------------------------------------------
QString GDFFileSignalWriter::save (QSharedPointer<FileContext const>,
                                   std::set<EventType> const&)
{
    //GDFWriter writer (new_file_path_.toStdString());

    return "";
}

}
