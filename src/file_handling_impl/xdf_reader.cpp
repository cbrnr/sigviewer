#include "xdf_reader.h"
#include "biosig_basic_header.h"
#include "file_handler_factory_registrator.h"
#include "gui/progress_bar.h"
#include "base/fixed_data_block.h"

#include <QTextStream>
#include <QTranslator>
#include <QMutexLocker>
#include <QDebug>
#include <QTime>
#include <QMessageBox>

#include <cmath>
#include <cassert>
#include <algorithm>

#include <iostream>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */
#include "resampling.h"

using namespace std;

namespace SigViewer_
{

//the object to store XDF data
Xdf XDFdata;

//-----------------------------------------------------------------------------

FILE_SIGNAL_READER_REGISTRATION(xdf, XDFReader);

//-----------------------------------------------------------------------------
XDFReader::XDFReader() :
    basic_header_ (0),
    buffered_all_channels_ (false),
    buffered_all_events_ (false)
{
    qDebug () << "Constructed XDFReader";
    // nothing to do here
}

//-----------------------------------------------------------------------------
XDFReader::~XDFReader()
{
}

//-----------------------------------------------------------------------------
QPair<FileSignalReader*, QString> XDFReader::createInstance (QString const& file_path)
{
    XDFReader* reader (new XDFReader);

    QString error = reader->open (file_path);
    if (error.size() > 0)
    {
        qDebug () << error;
        //QMessageBox::critical(0, QObject::tr("Error"), error);
        return QPair<FileSignalReader*, QString> (0, error);
    }
    else
        return QPair<FileSignalReader*, QString> (reader, "");
}

//-----------------------------------------------------------------------------
QSharedPointer<DataBlock const> XDFReader::getSignalData (ChannelID channel_id,
                                       size_t start_sample,
                                       size_t length) const
{
    QMutexLocker lock (&mutex_);

    if (!buffered_all_channels_)
        bufferAllChannels();

    if (!channel_map_.contains(channel_id))
        return QSharedPointer<DataBlock const> (0);

    if (length == basic_header_->getNumberOfSamples() &&
        start_sample == 0)
        return channel_map_[channel_id];
    else
        return channel_map_[channel_id]->createSubBlock (start_sample, length);
}

//-----------------------------------------------------------------------------
QList<QSharedPointer<SignalEvent const> > XDFReader::getEvents () const
{
    QMutexLocker lock (&mutex_);

    if (!buffered_all_events_)
        bufferAllEvents();

    return events_;
}

//-----------------------------------------------------------------------------
QString XDFReader::open (QString const& file_name)
{

    QMutexLocker lock (&mutex_);
    return loadFixedHeader (file_name);
}

//-----------------------------------------------------------------------------
QString XDFReader::loadFixedHeader(const QString& file_name)
{
    QMutexLocker locker (&xdf_access_lock_);

    clock_t t = clock();
    clock_t t2 = clock();

    XDFdata.load_xdf(file_name.toStdString());

    Resampling prompt(XDFdata.majSR);
    prompt.setModal(true);
    prompt.exec();

    if (prompt.getUserSrate())
        XDFdata.majSR = prompt.getUserSrate();

    XDFdata.resampleXDF(XDFdata.majSR);


    t = clock() - t;
    std::cout << "it took " << ((float)t) / CLOCKS_PER_SEC << " seconds reading data" << std::endl;

    t = clock() - t - t2;
    std::cout << "it took " << ((float)t) / CLOCKS_PER_SEC << " additional seconds loading XDF header" << std::endl;


    basic_header_ = QSharedPointer<BasicHeader>
                    (new BiosigBasicHeader ("XDF", file_name));

    basic_header_->setNumberEvents(XDFdata.eventType.size());

    basic_header_->setEventSamplerate(XDFdata.majSR);

    return "";
}

//-----------------------------------------------------------------------------
QSharedPointer<BasicHeader> XDFReader::getBasicHeader ()
{
    //QMutexLocker lock (&mutex_);
    return basic_header_;
}

//-----------------------------------------------------------------------------

void XDFReader::bufferAllChannels () const
{
    size_t numberOfSamples = XDFdata.totalLen;

    QString progress_name = QObject::tr("Loading data...");

    //load all signals channel by channel
    unsigned channel_id = 0;
    for (size_t st = 0; st< XDFdata.streams.size(); st++)
    {
        if (XDFdata.streams[st].info.nominal_srate != 0)
        {
            int pos = (XDFdata.streams[st].time_stamps.front() - XDFdata.minTS) * XDFdata.majSR;
            if (XDFdata.streams[st].time_series.front().size() < XDFdata.totalLen-pos )
            {
                for (size_t ch = 0; ch < XDFdata.streams[st].time_series.size(); ch++)
                {
                    ProgressBar::instance().increaseValue (1, progress_name);

                    QSharedPointer<QVector<float32> > raw_data(new QVector<float32> (numberOfSamples,0));


                    std::copy(XDFdata.streams[st].time_series[ch].begin(),
                              XDFdata.streams[st].time_series[ch].end(),
                              raw_data->begin() + pos);
                    QSharedPointer<DataBlock const> data_block(new FixedDataBlock(raw_data, XDFdata.majSR));

                    channel_map_[channel_id] = data_block;
                    channel_id++;

                    std::vector<float> nothing;
                    XDFdata.streams[st].time_series[ch].swap(nothing);
                }

                std::vector<float> nothing2;
                XDFdata.streams[st].time_stamps.swap(nothing2);
            }
            else
            {
                for (size_t ch = 0; ch < XDFdata.streams[st].time_series.size(); ch++)
                {
                    ProgressBar::instance().increaseValue (1, progress_name);

                    QSharedPointer<QVector<float32> > raw_data(new QVector<float32> (numberOfSamples,0));


                    std::copy(XDFdata.streams[st].time_series[ch].begin(),
                              XDFdata.streams[st].time_series[ch].end(),
                              raw_data->begin() + XDFdata.totalLen - XDFdata.streams[st].time_series[ch].size());
                    QSharedPointer<DataBlock const> data_block(new FixedDataBlock(raw_data, XDFdata.majSR));
                    channel_map_[channel_id] = data_block;
                    channel_id++;

                    std::vector<float> nothing;
                    XDFdata.streams[st].time_series[ch].swap(nothing);
                }

                std::vector<float> nothing2;
                XDFdata.streams[st].time_stamps.swap(nothing2);
            }
        }
        //else if: irregualar samples
        else if (XDFdata.streams[st].info.nominal_srate == 0 && !XDFdata.streams[st].time_series.empty())
        {
            for (size_t ch = 0; ch < XDFdata.streams[st].time_series.size(); ch++)
            {
                ProgressBar::instance().increaseValue (1, progress_name);

                QSharedPointer<QVector<float32> > raw_data(new QVector<float32> (numberOfSamples,0));


                for (size_t i = 0; i < XDFdata.streams[st].time_series[ch].size(); i++)
                {
                    //find out the position using the timestamp provided
                    float* pt = raw_data->begin()  + (int)((XDFdata.streams[st].time_stamps[i]- XDFdata.minTS)* XDFdata.majSR);
                    *(pt) = XDFdata.streams[st].time_series[ch][i];

                    //if i is not the last element of the irregular time series
                    if (i != XDFdata.streams[st].time_stamps.size() - 1)
                    {
                        //using linear interpolation to fill in the space between every two signals
                        int interval = (XDFdata.streams[st].time_stamps[i+1]
                                        - XDFdata.streams[st].time_stamps[i]) * XDFdata.majSR;
                        for (int mid = 1; mid <= interval; mid++)
                            *(pt + mid) = XDFdata.streams[st].time_series[ch][i]
                                + mid * ((XDFdata.streams[st].time_series[ch][i+1]
                                - XDFdata.streams[st].time_series[ch][i])) / interval;
                    }
                }
                QSharedPointer<DataBlock const> data_block(new FixedDataBlock(raw_data, XDFdata.majSR));
                channel_map_[channel_id] = data_block;
                channel_id++;

                std::vector<float> nothing;
                XDFdata.streams[st].time_series[ch].swap(nothing);

            }

            std::vector<float> nothing2;
            XDFdata.streams[st].time_stamps.swap(nothing2);

        }
    }


    buffered_all_channels_ = true;
}

//-------------------------------------------------------------------------
void XDFReader::bufferAllEvents () const
{
    unsigned number_events = XDFdata.eventMap.size();

    for (unsigned index = 0; index < number_events; index++)
    {
        QSharedPointer<SignalEvent> event
                (new SignalEvent ((XDFdata.eventMap[index].second - XDFdata.minTS) * XDFdata.majSR,
                                                            XDFdata.eventType[index],
                                                            XDFdata.majSR));

        event->setChannel (UNDEFINED_CHANNEL);
        event->setDuration (1);
        events_.append (event);
    }

    buffered_all_events_ = true;
}

}
