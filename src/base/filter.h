#ifndef FILTER_H
#define FILTER_H

#include <QSharedPointer>

namespace SigViewer_
{

class Filter
{
public:
    virtual void filter (double* in, double* out, int in_out_length) const = 0;
//    {
//        filterImpl (in, out, in_out_length);

//        if (!appended_filter_.isNull())
//        {
//            appended_filter_->filter (out, in, in_out_length);
//            memcpy (out, in, sizeof(double) * in_out_length);
//        }
//    }

//    void appendFilter (QSharedPointer<Filter const> filter) const
//    {
//        if (appended_filter_.isNull())
//            appended_filter_ = filter;
//        else
//            appended_filter_->appendFilter (filter);
//    }

//protected:
//    virtual void filterImpl (double* in, double* out, int in_out_length) const = 0;

//private:
//    mutable QSharedPointer<Filter const> appended_filter_;
};

}

#endif // FILTER_H
