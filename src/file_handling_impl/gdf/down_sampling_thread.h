#ifndef DOWN_SAMPLING_THREAD_H
#define DOWN_SAMPLING_THREAD_H

#include "base/data_block.h"
#include "gdf_data_block.h"

#include "GDF/Reader.h"

#include <QThread>
#include <QMutex>
#include <QSharedPointer>
#include <QVector>
#include <QList>

namespace SigViewer_
{

class DownSamplingThread : public QThread
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    DownSamplingThread (QList<QSharedPointer<DataBlock> > data, unsigned downsampling_step, unsigned downsampling_max);

signals:
    //-------------------------------------------------------------------------
    void downsamplingDataFinished (QSharedPointer<DataBlock> data, ChannelID channel, unsigned factor);

private:
    //-------------------------------------------------------------------------
    virtual void run ();

    //-------------------------------------------------------------------------
    void downsampleAllOnBasisData ();

    //-------------------------------------------------------------------------
    void downsampleOnDownsampledData ();

    //-------------------------------------------------------------------------
    QList<QSharedPointer<DataBlock> > basis_data_;
    QList<QSharedPointer<DataBlock> > data_;
    QList<QSharedPointer<DataBlock> > new_data_;
    unsigned downsampling_step_;
    unsigned downsampling_max_;
    bool running_;
};

}

#endif // DOWN_SAMPLING_THREAD_H
