#include "sinus_dummy_reader.h"
#include "../file_handling/file_signal_reader_factory.h"
#include "sinus_dummy_header.h"
#include "base/signal_channel.h"

#include <QApplication>
#include <QDebug>

#include <cmath>

using namespace std;

namespace BioSig_
{

SinusDummyReader SinusDummyReader::prototype_instance_ (true);

SinusDummyReader::SinusDummyReader ()
{
    header_ = QSharedPointer<BasicHeader> (new SinusDummyHeader);

    for (int i = 0; i < 10; i++)
    {
        QSharedPointer<vector<float32> > data (new vector<float32>);
        for (float sample_index = 0; sample_index < 10000; sample_index++)
            data->push_back (sin(sample_index / 100));
        QSharedPointer<DataBlock const> data_block (new DataBlock (data, 100));
        data_.insert(i, data_block);

        CHANNEL_STRUCT channel_struct;
        channel_struct.Label[0] = '1' + i;
        channel_struct.Label[1] = 0;
        SignalChannel* channel = new SignalChannel(i,
                                                   1,
                                                   channel_struct);
        header_->addChannel(channel);
    }

    for (int i = 0; i < 40; i++)
    {
        QSharedPointer<SignalEvent> event (new SignalEvent ((i) * 200, 1, 100, UNDEFINED_CHANNEL, 75));
        events_.push_back (event);
    }

    header_->setEventSamplerate(100);
    header_->setFileSize(1000);
    header_->setNumberChannels(10);
    header_->setNumberEvents(40);
    header_->setNumberRecords(10000);
    header_->setRecordDuration(0.01);
    header_->setRecordSize(1);
    header_->setSampleRate(100);
    header_->setType(QObject::tr("sinus dummy type"));
    header_->setVersion(QObject::tr("2"));


}

SinusDummyReader::SinusDummyReader (bool)
{
    if (!QApplication::arguments().contains("-test"))
        return;
    qDebug () << "register SinusDummyReader";
    FileSignalReaderFactory::getInstance()->registerHandler ("sinusdummy", QSharedPointer<SinusDummyReader>(new SinusDummyReader ()));
}

SinusDummyReader::~SinusDummyReader()
{}

//-------------------------------------------------------------------------
QSharedPointer<FileSignalReader> SinusDummyReader::createInstance (QString const&)
{
    qDebug () << "creating SinusDummyReader";
    return QSharedPointer<FileSignalReader> (new SinusDummyReader);
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
