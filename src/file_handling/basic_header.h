// basic_header.h

#ifndef BASIC_HEADER_H
#define BASIC_HEADER_H

#include "base/signal_channel.h"

#include <QString>
#include <QDateTime>
#include <QMap>

namespace BioSig_
{

//-----------------------------------------------------------------------------
/// @class BasicHeader (common for all file formats)
/// @brief base class for any biosignal file header
class BasicHeader
{
public:
    virtual ~BasicHeader() {}

    //-------------------------------------------------------------------------
    QString getFileTypeString () const;

    const QDateTime& getRecordingTime() const;
    const QString& getReference() const;
    bool isTriggered() const;
    int64 getNumberRecords() const;
    void setNumberRecords (int64 number_records);
    float64 getRecordDuration() const;
    void setRecordDuration (float64 record_duration);


    uint32 getFileSize() const;
    void setFileSize (uint32 file_size);


    virtual QMap<QString, QString> getPatientInfo () const
    {return QMap<QString, QString> ();}

    //-------------------------------------------------------------------------
    float getSampleRate () const;

    //-------------------------------------------------------------------------
    QSharedPointer<SignalChannel const> getChannel (ChannelID id) const;

    //-------------------------------------------------------------------------
    unsigned getNumberChannels() const;

    //-------------------------------------------------------------------------
    virtual uint32 getNumberOfSamples () const = 0;

    //-------------------------------------------------------------------------
    virtual QMap<unsigned, QString> getNamesOfUserSpecificEvents () const = 0;

    uint32 getNumberEvents() const;
    void setNumberEvents (uint32 number_events);
    double getEventSamplerate() const;
    void setEventSamplerate (double event_sample_rate);


protected:
    QDateTime recording_time_;
    QString reference_;
    bool triggered_;
    int64 number_records_;
    float64 record_duration_;

    // file
    uint32 file_size_;

    // events
    uint32 number_events_;
    double event_sample_rate_;
    
    //-------------------------------------------------------------------------
    void setFileTypeString (QString const& file_type_string);

    //-------------------------------------------------------------------------
    void setSampleRate (float sample_rate);

    //-------------------------------------------------------------------------
    void addChannel (ChannelID id, QSharedPointer<SignalChannel const> channel);
private:
    QString file_type_string_;
    float sample_rate_;
    QMap<ChannelID, QSharedPointer<SignalChannel const> > channels_;
};

} // namespace BioSig_

#endif
