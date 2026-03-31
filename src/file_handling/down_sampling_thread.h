// © SigViewer developers
//
// License: GPL-3.0


#ifndef DOWN_SAMPLING_THREAD_H
#define DOWN_SAMPLING_THREAD_H

#include "base/data_block.h"
#include "channel_manager.h"

#include <QThread>
#include <QMutex>
#include <QSharedPointer>
#include <QVector>
#include <QList>

#include <functional>

namespace SigViewer_
{

using sigviewer::DataBlock;
using sigviewer::ChannelManager;
using sigviewer::ChannelID;

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

    //-------------------------------------------------------------------------
    /// Run minMaxDownsampling() directly on the calling thread.
    /// Use this instead of start()+wait() when already on the main thread to
    /// avoid Qt widgets being instantiated on a background thread.
    void runSynchronously ();

    //-------------------------------------------------------------------------
    /// Optional callback invoked once per channel after its min/max pyramid
    /// has been built.  The second argument is the full raw DataBlock that was
    /// already read from disk – callers can use it without a second getData()
    /// call.  The hook is responsible for any progress bar update.
    void setPerChannelHook (std::function<void(ChannelID, QSharedPointer<DataBlock const>)> hook) { per_channel_hook_ = std::move(hook); }

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
    std::function<void(ChannelID, QSharedPointer<DataBlock const>)> per_channel_hook_;
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
