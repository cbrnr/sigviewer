#ifndef DUMMY_BASIC_HEADER_H
#define DUMMY_BASIC_HEADER_H

#include "file_handling/basic_header.h"

using BioSig_::BasicHeader;

class DummyBasicHeader : public BasicHeader
{
public:
    //-------------------------------------------------------------------------
    virtual ~DummyBasicHeader () {}

    //-------------------------------------------------------------------------
    virtual uint32 getNumberOfSamples () const
    {return 500;}

};

#endif // DUMMY_BASIC_HEADER_H
