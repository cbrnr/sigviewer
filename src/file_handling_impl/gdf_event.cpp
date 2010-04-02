// gdf_event.cpp

#include "gdf_event.h"

namespace BioSig_
{

// less operator
bool operator<(const GDFEvent& a, const GDFEvent& b)
{
    if (a.position < b.position)
    {
        return true;
    }
    if (a.position > b.position)
    {
        return false;
    }
    if (a.type < b.type)
    {
        return true;
    }
    if (a.type > b.type)
    {
        return false;
    }
    return a.channel < b.channel;
}

} // namespace BioSig_
