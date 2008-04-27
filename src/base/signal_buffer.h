// signal_buffer.h



#ifndef SIGNAL_BUFFER_H

#define SIGNAL_BUFFER_H



#include "signal_event.h"

#include "signal_data_block_queue.h"



#include <memory>



#include <QMap>

#include <QVector>

#include <QMutex>



class QTextStream;



namespace BioSig_

{



class FileSignalReader;

class SignalDataBlock;



// buffer for signal

class SignalBuffer

{

public:

    enum State

    {

        STATE_READY = 0,

        STATE_INIT = 1

    };



    enum

    {

        BUFFER_QUEUE_SIZE = 4096  // samples

    };



    enum SUB_SAMPLING

    {

        NO_SUBSAMPLING = 0,

        SUBSAMPLING_2 = 1,

        SUBSAMPLING_4 = 2,

        SUBSAMPLING_8 = 3,

        SUBSAMPLING_16 = 4,

        SUBSAMPLING_32 = 5,

        SUBSAMPLING_64 = 6,

        SUBSAMPLING_128 = 7,

        SUBSAMPLING_256 = 8

    };



    enum

    {

        MAX_SUBSAMPLING = SUBSAMPLING_256

    };



    enum

    {

        WHOLE_SUBSAMPLING = SUBSAMPLING_64

    };



    SignalBuffer(FileSignalReader& reader);

    virtual ~SignalBuffer();



    void setLogStream(QTextStream* log_stream);



    float64 getBlockDuration() const;

    uint32 getNumberBlocks() const;

    uint32 getRecordsPerBlock() const;



    void addChannel(uint32 channel_nr);

    void removeChannel(uint32 channel_nr);

    bool isChannelBuffered(uint32 channel_nr) const;

    void setChannelActive(uint32 channel_nr, bool active);

    bool isChannelActive(uint32 channel_nr) const;

    void init();

    float64 getMinValue(uint32 channel_nr) const;

    float64 getMaxValue(uint32 channel_nr) const;



    SignalDataBlock* getSignalDataBlock(uint32 channel_nr,

                                        uint32 sub_sampl, // SUB_SAMPLING

                                        uint32 block_nr); // in sub_sampl



    uint32 getNumberEvents() const;

    int32 eventNumber2ID(uint32 event_number) const;

    SignalEvent* getEvent(uint32 event_id);

    int32 addEvent(const SignalEvent& event);

    void removeEvent(uint32 event_id);

    uint32 getEventSamplerate() const;



protected:

    enum InitStatus

    {

        INIT_DOWNSAMPLE = 0,

        INIT_MIN_MAX = 1,

        INIT_EVENTS = 2

    };



    // overwrite this member functions

    virtual void initDone(uint32 percent, InitStatus status);



private:

    struct SubBuffers

    {

        std::auto_ptr<SignalDataBlockQueue> sub_queue[MAX_SUBSAMPLING + 1];

        SignalDataBlock* actual_data_block[MAX_SUBSAMPLING + 1]; // no auto_ptr

        uint32 next_data_block_nr;

        bool active;

        float64 min_value_;

        float64 max_value_;

        bool min_max_calculated_;

    };



private:

    typedef QMap<uint32, SignalEvent*> Int2SignalEventPtrMap;

    typedef QMap<uint32, SubBuffers*> Int2SubBuffersPtrMap;

    typedef QVector<SignalEvent> SignalEventVector;



    // not allowed

    SignalBuffer(const SignalBuffer& src);

    const SignalBuffer& operator=(const SignalBuffer& src);



    bool checkReadyState(const QString& function) const;

    SignalDataBlock* getSignalDataBlockImpl(uint32 channel_nr,

                                           uint32 sub_sampl, // SUB_SAMPLING

                                           uint32 block_nr); // in sub_sampl



    FileSignalReader& signal_reader_;

    QTextStream* log_stream_;

    uint32 records_per_block_;

    float64 block_duration_;

    uint32 number_blocks_;

    Int2SubBuffersPtrMap channel_nr2sub_buffers_map_;

    Int2SignalEventPtrMap id2signal_events_map_;

    State state_;

    uint32 next_event_id_;

    mutable QMutex mutex_;

};



} // namespace BioSig_



#endif

