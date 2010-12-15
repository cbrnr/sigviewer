#include "gdf_channel_cache.h"

namespace SigViewer_
{

//-----------------------------------------------------------------------------
double const& GDFChannelCache::operator[] (unsigned index) const
{
    unsigned buffer_index = index;
    buffer_index -= start_index_;
    buffer_index += cyclic_start_index_;
    if (buffer_index > buffer_size_)
        buffer_index -= buffer_size_;

    return buffer_[buffer_index];
}

}
