#include "gdf_writer.h"
//#include "gdfwriter.h"

namespace BioSig_
{

//-------------------------------------------------------------------------
GDFWriter::GDFWriter()
{

}

//-------------------------------------------------------------------------
QSharedPointer<FileSignalWriter> GDFWriter::createInstance (QString const&)
{
    return QSharedPointer<FileSignalWriter> (0);
}


//-------------------------------------------------------------------------
QString GDFWriter::saveEventsToSignalFile (QSharedPointer<EventManager const>,
                                        std::set<EventType> const&)
{
    return "";
}

//-------------------------------------------------------------------------
QString GDFWriter::save (QSharedPointer<EventManager>,
                      QString const&,
                      std::set<EventType> const&)
{
    return "";
}

}
