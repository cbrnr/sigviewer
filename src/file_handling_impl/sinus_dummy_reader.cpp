#include "sinus_dummy_reader.h"
#include "file_handler_factory_registrator.h"
#include "sinus_dummy_header.h"
#include "base/signal_channel.h"
#include "base/fixed_data_block.h"

#include <QApplication>
#include <QDebug>

#include <cmath>

using namespace std;

namespace SigViewer_
{

//-----------------------------------------------------------------------------
FILE_SIGNAL_READER_REGISTRATION(sinusdummy, SinusDummyReader);

//-----------------------------------------------------------------------------
SinusDummyReader::SinusDummyReader ()
{
    QSharedPointer<SinusDummyHeader> header (new SinusDummyHeader);

    for (int i = 0; i < 10; i++)
    {
        QSharedPointer<QVector<float32> > data (new QVector<float32>);
        for (float sample_index = 0; sample_index < 10000; sample_index++)
            data->push_back (sin(sample_index / ((i*i+1))));
        QSharedPointer<DataBlock const> data_block (new FixedDataBlock (data, 100));
        data_.insert(i, data_block);

        QSharedPointer<SignalChannel> channel (new SignalChannel(i,
                                                   QString::number(i)));
        header->addDummyChannel(i, channel);
        header_ = header;
    }

    for (int i = 0; i < 40; i++)
    {
        QSharedPointer<SignalEvent> event (new SignalEvent ((i) * 200, 1, 100, UNDEFINED_CHANNEL, 75));
        events_.push_back (event);
        if ((i % 10) == 0)
        {
            event = QSharedPointer<SignalEvent> (new SignalEvent((i) * 200, 2, 100, UNDEFINED_CHANNEL, 500));
            events_.push_back (event);
        }
    }

    header_->setEventSamplerate(100);
    header_->setNumberEvents(events_.size());
}

//-------------------------------------------------------------------------
SinusDummyReader::~SinusDummyReader()
{
    qDebug () << "deleting SinusDummyReader";
}

//-------------------------------------------------------------------------
QPair<FileSignalReader*, QString> SinusDummyReader::createInstance (QString const&)
{
    qDebug () << "creating SinusDummyReader";
    return QPair<FileSignalReader*, QString> (new SinusDummyReader, "");
}

//-------------------------------------------------------------------------
QSharedPointer<DataBlock const> SinusDummyReader::getSignalData (ChannelID channel_id,
                                                                         unsigned start_sample,
                                                                         unsigned length) const
{
    return data_[channel_id]->createSubBlock(start_sample, length);
}

//-------------------------------------------------------------------------
QList<QSharedPointer<SignalEvent const> > SinusDummyReader::getEvents () const
{
    return events_;
}

//-------------------------------------------------------------------------
QSharedPointer<BasicHeader> SinusDummyReader::getBasicHeader ()
{
    return header_;
}

}
