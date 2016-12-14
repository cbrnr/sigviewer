#ifndef DOWN_SAMPLING_THREAD_H
#define DOWN_SAMPLING_THREAD_H

#include "base/data_block.h"
#include "file_handling/channel_manager.h"

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

    //-------------------------------------------------------------------------
    DownSamplingThread (QSharedPointer<ChannelManager> channel_manager, unsigned downsampling_step, unsigned downsampling_max);

    //-------------------------------------------------------------------------
    virtual ~DownSamplingThread ();

signals:
    //-------------------------------------------------------------------------
    void downsamplingDataFinished (QSharedPointer<DataBlock> data, ChannelID channel, unsigned factor);

private:
    //-------------------------------------------------------------------------
    virtual void run ();

    //-------------------------------------------------------------------------
    void minMaxDownsampling ();

    //-------------------------------------------------------------------------
    void downsampleAllOnBasisData ();

    //-------------------------------------------------------------------------
    void downsampleOnDownsampledData ();

    //-------------------------------------------------------------------------
    QSharedPointer<ChannelManager> channel_manager_;
    QList<QSharedPointer<DataBlock> > basis_data_;
    QList<QSharedPointer<DataBlock> > data_;
    QList<QSharedPointer<DataBlock> > new_data_;
    unsigned downsampling_step_;
    unsigned downsampling_max_;
    bool running_;

    static QString PROCESS_NAME_;
};

}

#endif // DOWN_SAMPLING_THREAD_H
