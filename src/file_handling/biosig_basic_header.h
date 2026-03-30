// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef BIOSIG_BASIC_HEADER_H
#define BIOSIG_BASIC_HEADER_H

#include "file_handling/basic_header.h"
#include "biosig.h"

namespace sigviewer
{

class BiosigBasicHeader : public BasicHeader
{
public:
    //-------------------------------------------------------------------------
    BiosigBasicHeader (HDRTYPE* raw_header, QString const& file_path);

    //!Alternative constructor for XDF-----------------------------------------
    BiosigBasicHeader (QString file_format, QString const& file_path);

    //-------------------------------------------------------------------------
    virtual size_t getNumberOfSamples () const;

    //-------------------------------------------------------------------------
    virtual QMap<unsigned, QString> getNamesOfUserSpecificEvents () const;

private:
    //-------------------------------------------------------------------------
    void readChannelsInfo (HDRTYPE const* raw_header);

    //!alternative functions for XDF-------------------------------------------
    void readChannelsInfo (QString file_format);

    //-------------------------------------------------------------------------
    void readPatientInfo (HDRTYPE const* raw_header);

    //-------------------------------------------------------------------------
    void readRecordingInfo (HDRTYPE const* raw_header);




    unsigned number_samples_;
    QMap<unsigned, QString> user_defined_event_map_;
    QMap<unsigned, QSharedPointer<SignalChannel> > channels_;
};

}

#endif // BIOSIG_BASIC_HEADER_H
