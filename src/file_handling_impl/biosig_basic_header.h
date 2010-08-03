#ifndef BIOSIG_BASIC_HEADER_H
#define BIOSIG_BASIC_HEADER_H

#include "file_handling/basic_header.h"
#include "biosig.h"

namespace SigViewer_
{

class BiosigBasicHeader : public BasicHeader
{
public:
    //-------------------------------------------------------------------------
    BiosigBasicHeader (HDRTYPE* raw_header, QString const& file_path);

    //-------------------------------------------------------------------------
    virtual uint32 getNumberOfSamples () const;

    //-------------------------------------------------------------------------
    virtual QMap<unsigned, QString> getNamesOfUserSpecificEvents () const;

private:
    //-------------------------------------------------------------------------
    void readChannelsInfo (HDRTYPE const* raw_header);

    //-------------------------------------------------------------------------
    void readPatientInfo (HDRTYPE const* raw_header);

    //-------------------------------------------------------------------------
    void readRecordingInfo (HDRTYPE const* raw_header);

    unsigned number_samples_;
    QMap<unsigned, QString> user_defined_event_map_;
    QMap<unsigned, QSharedPointer<SignalChannel> > channels_;
};

} // namespace SigViewer_

#endif // BIOSIG_BASIC_HEADER_H
