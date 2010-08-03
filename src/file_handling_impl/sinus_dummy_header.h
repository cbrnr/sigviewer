#ifndef SINUS_DUMMY_HEADER_H
#define SINUS_DUMMY_HEADER_H

#include "file_handling/basic_header.h"

namespace SigViewer_
{

class SinusDummyHeader : public BasicHeader
{
public:
    //-------------------------------------------------------------------------
    SinusDummyHeader ();

    //-------------------------------------------------------------------------
    virtual uint32 getNumberOfSamples () const {return 10000;}

    //-------------------------------------------------------------------------
    void addDummyChannel (ChannelID id, QSharedPointer<SignalChannel const> channel)
    {
        addChannel (id, channel);
    }

    //-------------------------------------------------------------------------
    virtual QMap<unsigned, QString> getNamesOfUserSpecificEvents () const
    {return QMap<unsigned, QString>();}
};

} // namespace SigViewer_

#endif // SINUS_DUMMY_HEADER_H
