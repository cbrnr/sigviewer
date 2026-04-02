// © SigViewer developers
//
// License: GPL-3.0

#ifndef MOCK_FILE_SIGNAL_READER_H
#define MOCK_FILE_SIGNAL_READER_H

#include <QSharedPointer>
#include <QVector>

#include "base/fixed_data_block.h"
#include "base/signal_channel.h"
#include "base/signal_event.h"
#include "base/sigviewer_user_types.h"
#include "file_handling/basic_header.h"
#include "file_handling/file_signal_reader.h"
#include "file_handling/file_signal_reader_factory.h"

namespace sigviewer {

static constexpr int MOCK_NUM_CHANNELS = 8;
static constexpr int MOCK_NUM_EVENTS = 44;
static constexpr double MOCK_SAMPLE_RATE = 256.0;
static constexpr size_t MOCK_NUM_SAMPLES = 256 * 600;  // 600 s

class MockBasicHeader : public BasicHeader {
   public:
    MockBasicHeader() : BasicHeader("blub.sinusdummy") {
        setFileTypeString("sinusdummy");
        setSampleRate(MOCK_SAMPLE_RATE);
        setEventSamplerate(MOCK_SAMPLE_RATE);
        number_events_ = MOCK_NUM_EVENTS;
        for (int ch = 0; ch < MOCK_NUM_CHANNELS; ++ch)
            addChannel(ch,
                QSharedPointer<SignalChannel const>(
                    new SignalChannel(QString("Ch%1").arg(ch), MOCK_SAMPLE_RATE)));
    }

    size_t getNumberOfSamples() const override { return MOCK_NUM_SAMPLES; }
};

class MockFileSignalReader : public FileSignalReader {
   public:
    QPair<FileSignalReader*, QString> createInstance(QString const& /*file_path*/) override {
        return {new MockFileSignalReader(), ""};
    }

    QSharedPointer<DataBlock const> getSignalData(ChannelID /*channel_id*/,
        size_t /*start_sample*/,
        size_t length) const override {
        auto data = QSharedPointer<QVector<float32>>(new QVector<float32>(length, 0.0f));
        return QSharedPointer<DataBlock const>(new FixedDataBlock(data, MOCK_SAMPLE_RATE));
    }

    QList<QSharedPointer<SignalEvent const>> getEvents() const override {
        static const EventType types[] = {0x0001, 0x0002, 0x0003, 0x0004, 0x0005};
        QList<QSharedPointer<SignalEvent const>> events;
        for (int i = 0; i < MOCK_NUM_EVENTS; ++i)
            events.append(
                QSharedPointer<SignalEvent const>(new SignalEvent(static_cast<size_t>(i) * 256,
                    types[i % 5],
                    MOCK_SAMPLE_RATE,
                    UNDEFINED_STREAM_ID,
                    static_cast<ChannelID>(i % MOCK_NUM_CHANNELS),
                    128)));
        return events;
    }

    QSharedPointer<BasicHeader> getBasicHeader() override {
        if (!header_) header_ = QSharedPointer<BasicHeader>(new MockBasicHeader());
        return header_;
    }

    QSharedPointer<BasicHeader const> getBasicHeader() const override {
        if (!header_) header_ = QSharedPointer<BasicHeader>(new MockBasicHeader());
        return header_;
    }

   private:
    mutable QSharedPointer<BasicHeader> header_;
};

/// Call once before using FileSignalReaderFactory with "blub.sinusdummy".
inline void registerMockFileSignalReader() {
    FileSignalReaderFactory::getInstance()->registerHandler("sinusdummy",
        QSharedPointer<FileSignalReader>(new MockFileSignalReader()));
}

}  // namespace sigviewer

#endif  // MOCK_FILE_SIGNAL_READER_H
