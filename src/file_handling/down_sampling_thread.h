// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


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
