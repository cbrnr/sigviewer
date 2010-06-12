#ifndef INIT_FILE_HANDLING_IMPL_H
#define INIT_FILE_HANDLING_IMPL_H

#include "biosig_reader.h"
#include "sinus_dummy_reader.h"

namespace BioSig_
{

void initFileHandlingImpl ()
{
    BioSigReader reader (true);
    SinusDummyReader dummy_reader (true);
}

}

#endif // INIT_FILE_HANDLING_IMPL_H
