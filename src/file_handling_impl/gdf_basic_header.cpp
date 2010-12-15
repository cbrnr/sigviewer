#include "gdf_basic_header.h"

namespace SigViewer_
{

//-------------------------------------------------------------------------------------------------
GDFBasicHeader::GDFBasicHeader (QString const& file_path,
                                gdf::GDFHeaderAccess const& header_access) : BasicHeader (file_path),
    header_access_ (header_access)
{
    setDownSamplingFactor (1);
    setSampleRate (header_access_.getSignalHeader_readonly(0).get_samplerate());
    for (unsigned index = 0; index < header_access_.getNumSignals(); index++)
    {
        QSharedPointer<SignalChannel> channel (new SignalChannel (index, QString::number(index)));
        addChannel (index, channel);
    }
}

//---------------------------------------------------------------------------------------------
uint32 GDFBasicHeader::getNumberOfSamples () const
{
    return header_access_.getSignalHeader_readonly(0).get_samples_per_record() *
            header_access_.getMainHeader_readonly().get_num_datarecords() /
            getDownSamplingFactor ();
}



}
