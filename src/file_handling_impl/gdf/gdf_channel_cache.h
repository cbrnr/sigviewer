#ifndef GDF_CHANNEL_CACHE_H
#define GDF_CHANNEL_CACHE_H

#include <limits>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
/// GDFChannelCache
class GDFChannelCache
{
public:
    GDFChannelCache (unsigned max_size)
        : buffer_ (new double[max_size]),
          buffer_size_ (max_size),
          start_index_ (std::numeric_limits<unsigned>::max()),
          cyclic_start_index_ (0)
    {}

    virtual ~GDFChannelCache ()
    {delete[] buffer_;}

    double const& operator[] (unsigned index) const;

    double* buffer () {return buffer_;}
    unsigned bufferSize () {return buffer_size_;}
    void setStartIndex (unsigned start_index) {start_index_ = start_index;}
    void setCyclicStartIndex (unsigned cyclic_start_index) {cyclic_start_index_ = cyclic_start_index;}
    bool hasSampleIndex (unsigned sample_index) {return (sample_index >= start_index_) && (sample_index < (start_index_ + buffer_size_));}

private:
    //Q_DISABLE_COPY (GDFChannelCache);

    double* buffer_;
    unsigned buffer_size_;
    unsigned start_index_;
    unsigned cyclic_start_index_;
};

}

#endif // GDF_CHANNEL_CACHE_H
