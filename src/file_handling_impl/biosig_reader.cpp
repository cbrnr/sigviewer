// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "biosig_reader.h"
#include "biosig_basic_header.h"
#include "file_handler_factory_registrator.h"
#include "gui/progress_bar.h"
#include "base/fixed_data_block.h"
#include "application_context_impl.h"

#include "biosig.h"

#include <QTextStream>
#include <QTranslator>
#include <QMutexLocker>
#include <QDebug>
#include <QTime>
#include <QMessageBox>
#include <QSettings>

#include <cmath>
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
        biosig_header_->FLAG.OVERFLOWDETECTION = 1;
    }

    biosig_header_ = sopen(c_file_name, "r", biosig_header_ );

    basic_header_ = QSharedPointer<BasicHeader>
                    (new BiosigBasicHeader (biosig_header_, file_name));

    if (!QFile::exists(file_name))
    {
        sclose (biosig_header_);
        destructHDR(biosig_header_);
        biosig_header_ = NULL;

        delete[] c_file_name;

        qDebug() << "File doesn't exist.";
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("File does not exist.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();

        return "non-exist";
    }

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

    setChannelColors();
    setEventTypeColors();

    return "";
}

//-----------------------------------------------------------------------------
QSharedPointer<BasicHeader> BioSigReader::getBasicHeader ()
{
    //QMutexLocker lock (&mutex_);
    return basic_header_;
}

//-----------------------------------------------------------------------------
int BioSigReader::setChannelColors()
{
    QSharedPointer<ColorManager> colorPicker = ApplicationContextImpl::getInstance()->color_manager_;
    for (size_t i = 0; i < basic_header_->getNumberChannels(); i++)
        colorPicker->setChannelColor(i, colorPicker->getDefaultChannelColor());

    colorPicker->saveSettings();

    return 0;
}

//-----------------------------------------------------------------------------
int BioSigReader::setEventTypeColors()
{
    // Display each event type in a distinct color
    QSharedPointer<ColorManager> colorPicker = ApplicationContextImpl::getInstance()->color_manager_;

    //set event colors
    srand (time(NULL));     /* initialize random seed: */

    QVector<QColor> eventColorList = {"#0055ff", "#00aa00", "#aa00ff", "#ff0000", "#00557f",
                                      "#5555ff", "#ff55ff", "#00aaff", "#00aa7f", "#ff5500"};

    int colorChoice = 5;    //Set the first event color to be pink

    QSettings settings;
    settings.beginGroup("ColorManager");
    int size = settings.beginReadArray("event");

    for (int type = 0; type < 254; type++)
    {
        QColor color;
        if (type < size)
        {
            settings.setArrayIndex(type);
            color = settings.value("color").toString();
            color.setAlpha(settings.value("alpha").toInt());
        }

        /* if the user has specified color setting for the current event type previously
             * in QSettings, we want to use it.
             * If the color setting is #0000ff (default), we assume the user
             * hasn't specified color for the current event type yet. Below is our algorithm
             * to pick some good colors for event types. */

        if (color.name().compare("#0000ff", Qt::CaseInsensitive) == 0)
        {
            // generate random number first:
            int red = rand() % 41 + (-20);
            int green = rand() % 41 + (-20);
            int blue = rand() % 41 + (-20);

            colorChoice++;
            if (colorChoice == 10)   //we only have 10 basic colors
                colorChoice = 0;

            color = eventColorList[colorChoice];

            red += color.red();
            green += color.green();
            blue += color.blue();

            if (red < 0)
                red = 0;
            if (red > 255)
                red = 255;
            if (green < 0)
                green = 0;
            if (green > 255)
                green = 255;
            if (blue < 0)
                blue = 0;
            if (blue > 255)
                blue = 255;


            color.setRgb(red, green, blue);
            color.setAlpha(120);
        }
        colorPicker->setEventColor(type, color); //QColor(0, 85, 255, 80)
        eventColorList[colorChoice] = color;
    }

    settings.endArray();
    settings.endGroup();

    colorPicker->saveSettings();

    return 0;
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

        QSharedPointer<QVector<float32> > raw_data(new QVector<float32> (numberOfSamples, NAN));
        for (size_t data_index = 0; data_index < numberOfSamples; data_index++)
            raw_data->operator [](data_index) = read_data[data_index + channel_id * numberOfSamples];

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
                                                            biosig_header_->EVENT.SampleRate * rate_transition, -1));
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
