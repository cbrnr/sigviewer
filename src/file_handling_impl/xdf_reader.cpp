// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl



#include "xdf_reader.h"
#include "biosig_basic_header.h"
#include "file_handler_factory_registrator.h"
#include "gui/progress_bar.h"
#include "base/fixed_data_block.h"
#include "gui_impl/dialogs/resampling_dialog.h"

#include <QTextStream>
#include <QTranslator>
#include <QMutexLocker>
#include <QTime>
#include <QMessageBox>

#include <cmath>
#include <algorithm>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */


namespace sigviewer
{

//the object to store XDF data
QSharedPointer<Xdf> XDFdata = QSharedPointer<Xdf>(new Xdf);


//-----------------------------------------------------------------------------

FILE_SIGNAL_READER_REGISTRATION(xdf, XDFReader);

//-----------------------------------------------------------------------------
XDFReader::XDFReader() :
    basic_header_ (0),
    buffered_all_channels_ (false),
    buffered_all_events_ (false)
{
    qDebug () << "Constructed XDFReader";
}

//-----------------------------------------------------------------------------
XDFReader::~XDFReader()
{
    qDebug() << "Destructed XDFReader.";
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
QString XDFReader::open (QString const& file_path)
{
    QMutexLocker lock (&mutex_);
    return loadFixedHeader (file_path);
}

//-----------------------------------------------------------------------------
QString XDFReader::loadFixedHeader(const QString& file_path)
{
    QMutexLocker locker (&xdf_access_lock_);

    clock_t t = clock();
    clock_t t2 = clock();

    if (QFile::exists(file_path)) //Double check whether file exists
    {
        if (XDFdata->load_xdf(file_path.toStdString()) == 0)
        {
            XDFdata->createLabels();

            sampleRateTypes sampleRateType = selectSampleRateType();

            switch (sampleRateType) {
            case No_streams_found:
            {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.setText(QObject::tr("No Stream Found"));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.exec();

                return "non-exist";
            }
            case Zero_Hz_Only:
            {
                ResamplingDialog prompt(XDFdata->majSR, XDFdata->maxSR);

                if (prompt.exec() == QDialog::Accepted)
                {
                    XDFdata->majSR = prompt.getUserSrate();
                    XDFdata->resample(XDFdata->majSR);
                }
                else
                {
                    Xdf empty;
                    std::swap(*XDFdata, empty);
                    return "Cancelled";
                }
            }
                break;
            case Mono_Sample_Rate:
            {
                XDFdata->calcTotalLength(XDFdata->majSR);

                XDFdata->adjustTotalLength();

                if (XDFdata->effectiveSampleRateVector.size())
                {
                    /* If and only if the file contains only one sample rate, we use effective
                    sample rate to more accurately display events. Effective sample rates
                    can be slightly different across streams, so we calculate the mean here */

                    double init = 0.0;
                    XDFdata->fileEffectiveSampleRate =
                            std::accumulate(XDFdata->effectiveSampleRateVector.begin(),
                                            XDFdata->effectiveSampleRateVector.end(), init)
                            / XDFdata->effectiveSampleRateVector.size();
                }
            }
                break;
            case Multi_Sample_Rate:
            {
                ResamplingDialog prompt(XDFdata->majSR, XDFdata->maxSR);

                if (prompt.exec() == QDialog::Accepted)
                {
                    XDFdata->majSR = prompt.getUserSrate();
                    XDFdata->resample(XDFdata->majSR);
                }
                else
                {
                    Xdf empty;
                    std::swap(*XDFdata, empty);
                    return "Cancelled";
                }
            }
                break;
            default:
                qDebug() << "Unknown sample rate type.";
                break;
            }

            XDFdata->freeUpTimeStamps(); //to save some memory

            setStreamColors();
            setEventTypeColors();

            t = clock() - t;
            qDebug() << "it took " << ((float)t) / CLOCKS_PER_SEC << " seconds reading data";

            t = clock() - t - t2;
            qDebug() << "it took " << ((float)t) / CLOCKS_PER_SEC << " additional seconds loading XDF header";

            bool showWarning = false;

            for (auto const &stream : XDFdata->streams)
            {
                if (std::abs(stream.info.effective_sample_rate - stream.info.nominal_srate) >
                        stream.info.nominal_srate / 20)
                {
                    showWarning = true;
                }
            }

            if (showWarning)
                QMessageBox::warning(0, "SigViewer",
                                     QObject::tr("The effective sampling rate of at least one stream is significantly different than the reported nominal sampling rate. Signal visualization might be inaccurate."), QMessageBox::Ok, QMessageBox::Ok);


            basic_header_ = QSharedPointer<BasicHeader>
                    (new BiosigBasicHeader ("XDF", file_path));

            basic_header_->setNumberEvents(XDFdata->eventType.size());

            if (XDFdata->fileEffectiveSampleRate)
                basic_header_->setEventSamplerate(XDFdata->fileEffectiveSampleRate);
            else
                basic_header_->setEventSamplerate(XDFdata->majSR);

            return "";
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(QObject::tr("Unable to open file."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();

            return "non-exist";
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(QObject::tr("File does not exist."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();

        return "non-exist";
    }
}

//-----------------------------------------------------------------------------
QSharedPointer<BasicHeader> XDFReader::getBasicHeader ()
{
    //QMutexLocker lock (&mutex_);
    return basic_header_;
}

//-----------------------------------------------------------------------------
int XDFReader::setStreamColors()
{
    // Display each stream in a distinct color
    QSharedPointer<ColorManager> colorPicker = ApplicationContextImpl::getInstance()->color_manager_;
    QVector<QColor> colorList = {"#0055ff", "#00aa00", "#aa00ff", "#00557f",
                                 "#5555ff", "#ff55ff", "#00aaff", "#00aa7f"};

    //some streams contains text only and no channels, so we skip those streams
    //because the colors I picked look the best when they are sorted in order
    int colorChoice = -1;
    int stream = -1;
    for (size_t i = 0; i < XDFdata->totalCh; i++)
    {
        if (stream != XDFdata->streamMap[i])
        {
            stream = XDFdata->streamMap[i];
            colorChoice++;
            if (colorChoice == 8)   //we only have 8 colors
                colorChoice = 0;
        }
        colorPicker->setChannelColor(i, colorList[colorChoice]);
    }

    colorPicker->saveSettings();

    return 0;
}

//-----------------------------------------------------------------------------
XDFReader::sampleRateTypes XDFReader::selectSampleRateType()
{
    switch (XDFdata->sampleRateMap.size()) {
    case 0:
        return No_streams_found;
    case 1:
        if (XDFdata->sampleRateMap.count(0))
            return Zero_Hz_Only;
        else
            return Mono_Sample_Rate;
    case 2:
        if (XDFdata->sampleRateMap.count(0))
            return Mono_Sample_Rate;
        else
            return Multi_Sample_Rate;
    default:
        return Multi_Sample_Rate;
    }
}

//-----------------------------------------------------------------------------
void XDFReader::bufferAllChannels () const
{
    size_t numberOfSamples = XDFdata->totalLen;

    QString progress_name = QObject::tr("Loading data...");

    //load all signals channel by channel
    unsigned channel_id = 0;
    for (auto &stream : XDFdata->streams)
    {
        if (stream.info.nominal_srate != 0 && stream.info.channel_format.compare("string")) // filter the string streams
        {
            int startingPosition = (stream.info.first_timestamp - XDFdata->minTS) * XDFdata->majSR;

            if (stream.time_series.front().size() > XDFdata->totalLen - startingPosition )
                startingPosition = XDFdata->totalLen - stream.time_series.front().size();

            for (auto &row : stream.time_series)
            {
                ProgressBar::instance().increaseValue (1, progress_name);

                QSharedPointer<QVector<float32> > raw_data(new QVector<float32> (numberOfSamples, NAN));

                std::copy(row.begin(), row.end(), raw_data->begin() + startingPosition);
                QSharedPointer<DataBlock const> data_block(new FixedDataBlock(raw_data, XDFdata->majSR));

                channel_map_[channel_id] = data_block;
                channel_id++;

                std::vector<float> nothing;
                row.swap(nothing);
            }
            std::vector<double> nothing2;
            stream.time_stamps.swap(nothing2);
        }
        //else if: irregualar samples
        else if (stream.info.nominal_srate == 0 && !stream.time_series.empty())
        {
            for (auto &row : stream.time_series)
            {
                ProgressBar::instance().increaseValue (1, progress_name);

                QSharedPointer<QVector<float32> > raw_data(new QVector<float32> (numberOfSamples, NAN));

                for (size_t i = 0; i < row.size(); i++)
                {
                    //find out the position using the timestamp provided
                    float* pt = raw_data->begin()  + (int)(round((stream.time_stamps[i]- XDFdata->minTS)* XDFdata->majSR));
                    *pt = row[i];

                    //if i is not the last element of the irregular time series
                    if (i != stream.time_stamps.size() - 1)
                    {
                        //using linear interpolation to fill in the space between every two signals
                        int interval = round((stream.time_stamps[i+1]
                                - stream.time_stamps[i]) * XDFdata->majSR);
                        for (int interpolation = 1; interpolation <= interval; interpolation++)
                        {
                            *(pt + interpolation) = row[i] + interpolation * ((row[i+1] - row[i])) / (interval + 1);
                        }
                    }
                }
                QSharedPointer<DataBlock const> data_block(new FixedDataBlock(raw_data, XDFdata->majSR));
                channel_map_[channel_id] = data_block;
                channel_id++;

                std::vector<float> nothing;
                row.swap(nothing);
            }
            std::vector<double> nothing2;
            stream.time_stamps.swap(nothing2);
        }
    }

    buffered_all_channels_ = true;
}

//-------------------------------------------------------------------------
void XDFReader::bufferAllEvents () const
{
    unsigned number_events = XDFdata->eventMap.size();

    double eventSampleRate = 0;

    if (XDFdata->fileEffectiveSampleRate)
        eventSampleRate = XDFdata->fileEffectiveSampleRate;
    else
        eventSampleRate = XDFdata->majSR;

    for (unsigned index = 0; index < number_events; index++)
    {
        QSharedPointer<SignalEvent> event
                (new SignalEvent (round ((XDFdata->eventMap[index].first.second - XDFdata->minTS) * eventSampleRate),
                                  XDFdata->eventType[index] + 1,//index+1 because in SigViewer and libbiosig
                                  //0 is reserved for a special type of event. Thus we increment by 1
                                  eventSampleRate, XDFdata->eventMap[index].second));

        event->setChannel (UNDEFINED_CHANNEL);
        event->setDuration (0);
        events_.append (event);
    }

    buffered_all_events_ = true;
}

}
