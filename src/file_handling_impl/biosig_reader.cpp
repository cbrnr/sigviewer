// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "biosig_reader.h"
#include "biosig_basic_header.h"
#include "file_handler_factory_registrator.h"
#include "gui/progress_bar.h"
#include "base/fixed_data_block.h"


#include "biosig.h"

#include <QTextStream>
#include <QTranslator>
#include <QMutexLocker>
#include <QDebug>
#include <QTime>
#include <QMessageBox>

#include <cmath>
#include <cassert>
#include <algorithm>

using namespace std;

namespace sigviewer
{

//-----------------------------------------------------------------------------
FILE_SIGNAL_READER_REGISTRATION(gdf, BioSigReader);
FILE_SIGNAL_READER_REGISTRATION(edf, BioSigReader);
FILE_SIGNAL_READER_REGISTRATION(bdf, BioSigReader);
FILE_SIGNAL_READER_REGISTRATION(dat, BioSigReader);
FILE_SIGNAL_READER_REGISTRATION(cnt, BioSigReader);
FILE_SIGNAL_READER_REGISTRATION(vhdr, BioSigReader);
FILE_SIGNAL_READER_REGISTRATION(bkr, BioSigReader);

FILE_SIGNAL_READER_REGISTRATION(evt, BioSigReader);

FILE_SIGNAL_READER_DEFAULT_REGISTRATION(BioSigReader);

//-----------------------------------------------------------------------------
BioSigReader::BioSigReader() :
    basic_header_ (0),
    biosig_header_ (0),
    buffered_all_channels_ (false),
    buffered_all_events_ (false)
{
    qDebug () << "Constructed BioSigReader";
    // nothing to do here
}

//-----------------------------------------------------------------------------
BioSigReader::~BioSigReader()
{
    doClose();
}

//-----------------------------------------------------------------------------
QPair<FileSignalReader*, QString> BioSigReader::createInstance (QString const& file_path)
{
    BioSigReader* reader (new BioSigReader);
    if (file_path.section('.', -1) == "evt")
        reader->buffered_all_channels_ = true;
    QString error = reader->open (file_path);
    if (error.size() > 0)
    {
        qDebug () << error;
        return QPair<FileSignalReader*, QString> (0, error);
    }
    else
        return QPair<FileSignalReader*, QString> (reader, "");
}


//-----------------------------------------------------------------------------
void BioSigReader::doClose () const
{
    if (biosig_header_)
        destructHDR (biosig_header_);
    biosig_header_ = NULL;
}

//-----------------------------------------------------------------------------
QSharedPointer<DataBlock const> BioSigReader::getSignalData (ChannelID channel_id,
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
QList<QSharedPointer<SignalEvent const> > BioSigReader::getEvents () const
{
    QMutexLocker lock (&mutex_);
    QList<QSharedPointer<SignalEvent const> > empty_list;
    if (!biosig_header_)
        return empty_list;

    if (!buffered_all_events_)
        bufferAllEvents();

    return events_;
}

//-----------------------------------------------------------------------------
QString BioSigReader::open (QString const& file_name)
{

    QMutexLocker lock (&mutex_);
    return loadFixedHeader (file_name);
}

//-----------------------------------------------------------------------------
QString BioSigReader::loadFixedHeader(const QString& file_name)
{
    QMutexLocker locker (&biosig_access_lock_);
    char *c_file_name = new char[file_name.length() + 1];
    strcpy (c_file_name, file_name.toLocal8Bit ().data());
    c_file_name[file_name.length()] = '\0';

    tzset();

    if(biosig_header_==NULL)
    {
        biosig_header_ = constructHDR (0,0);
        biosig_header_->FLAG.UCAL = 0;
        biosig_header_->FLAG.OVERFLOWDETECTION = 0;
    }

    biosig_header_ = sopen(c_file_name, "r", biosig_header_ );

    basic_header_ = QSharedPointer<BasicHeader>
                    (new BiosigBasicHeader (biosig_header_, file_name));

    if (biosig_header_ == NULL || serror2(biosig_header_))
    {
            sclose (biosig_header_);
            destructHDR(biosig_header_);
            biosig_header_ = NULL;
            delete[] c_file_name;
            return "file not supported";
    }

    convert2to4_eventtable(biosig_header_);

    delete[] c_file_name;
    c_file_name = NULL;

    basic_header_->setNumberEvents(biosig_header_->EVENT.N);

    if (biosig_header_->EVENT.SampleRate)
        basic_header_->setEventSamplerate(biosig_header_->EVENT.SampleRate);
    else
        basic_header_->setEventSamplerate(biosig_header_->SampleRate);

    return "";
}

//-----------------------------------------------------------------------------
QSharedPointer<BasicHeader> BioSigReader::getBasicHeader ()
{
    //QMutexLocker lock (&mutex_);
    return basic_header_;
}

//-----------------------------------------------------------------------------
void BioSigReader::bufferAllChannels () const
{
    size_t numberOfSamples = biosig_header_->NRec * biosig_header_->SPR;
    biosig_data_type* read_data = new biosig_data_type[numberOfSamples * basic_header_->getNumberChannels()];

    biosig_header_->FLAG.ROW_BASED_CHANNELS = 0;

    QString progress_name = QObject::tr("Loading data...");

    sread(read_data, 0, biosig_header_->NRec, biosig_header_);

    for (unsigned channel_id = 0; channel_id < basic_header_->getNumberChannels();
         ++channel_id)
    {
        ProgressBar::instance().increaseValue (1, progress_name);

        QSharedPointer<QVector<float32> > raw_data(new QVector<float32> (numberOfSamples));
        for (size_t data_index = 0; data_index < numberOfSamples; data_index++)
            raw_data->operator [](data_index) = read_data[data_index + channel_id * numberOfSamples];

        //Calculate mean
        float64 init = 0.0;
        float64 mean = std::accumulate(raw_data->begin(), raw_data->end(), init) / raw_data->size();
        std::transform(raw_data->begin(), raw_data->end(), raw_data->begin(), bind2nd(std::minus<double>(), mean));
        basic_header_->means_.push_back(mean);

        QSharedPointer<DataBlock const> data_block(new FixedDataBlock(raw_data, basic_header_->getSampleRate()));
        channel_map_[channel_id] = data_block;
    }

    buffered_all_channels_ = true;
    if (buffered_all_events_)
        doClose();
    delete[] read_data;
}

//-------------------------------------------------------------------------
void BioSigReader::bufferAllEvents () const
{
    unsigned number_events = biosig_header_->EVENT.N;
    // Hack Hack: Transforming Events to have the same sample rate as the signals
    double rate_transition = basic_header_->getEventSamplerate() / biosig_header_->EVENT.SampleRate;

    for (unsigned index = 0; index < number_events; index++)
    {
        QSharedPointer<SignalEvent> event (new SignalEvent (biosig_header_->EVENT.POS[index] * rate_transition,
                                                            biosig_header_->EVENT.TYP[index],
                                                            biosig_header_->EVENT.SampleRate * rate_transition));
        if (biosig_header_->EVENT.CHN)
        {
            if (biosig_header_->EVENT.CHN[index] == 0)
                event->setChannel (UNDEFINED_CHANNEL);
            else
                event->setChannel (biosig_header_->EVENT.CHN[index] - 1);

            if (biosig_header_->EVENT.TYP[index] != 0x7fff)
                event->setDuration (biosig_header_->EVENT.DUR[index] * rate_transition);
            else 	// sparse samples (Typ=0x7fff) do not have a duration, but the duration field is used to store the sample value
                event->setDuration(0);
        }
        else
        {
            event->setChannel(UNDEFINED_CHANNEL);
            event->setDuration (1);
        }
        events_.append (event);
    }

    buffered_all_events_ = true;
    if (buffered_all_channels_)
        doClose();
}

}
