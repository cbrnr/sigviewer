// basic_header.h

#ifndef BASIC_HEADER_H
#define BASIC_HEADER_H

#include "signal_channel.h"

#include <QVector>
#include <QString>
#include <QDateTime>
#include <QObject>

namespace BioSig_
{

// Basic header definition (common for all file formats)
class BasicHeader : public QObject
{
public:
    enum Sex
    {
        UNDEFINED_SEX = 0,
        MALE = 1,
        FEMALE = 2
    };

    enum
    {
        UNDEFINED_AGE = -1
    };

    BasicHeader();
    virtual ~BasicHeader();

    // basic
    const QString& getType() const;
    const QString& getVersion() const;
    const QDateTime& getRecordingTime() const;
    const QString& getReference() const;
    bool isTriggered() const;
    int64 getNumberRecords() const;
    float64 getRecordDuration() const;

    // file
    const QString& getFullFileName() const;
    const QString getFileName() const;
    const QString getFilePath() const;
    const QString getFileExtension() const;
    uint32 getFileSize() const;

    // patient
    const QString& getPatientName() const;
    int32 getPatientAge() const;
    Sex getPatientSex() const;
    const QString& getPatientHandedness() const;
    const QString& getPatientMedication() const;
    const QString& getPatientClassification() const;
    uint64 getDoctorId() const;
    uint64 getHospitalId() const;

    // channels
    uint32 getNumberChannels() const;
    const SignalChannel& getChannel(uint32 channel_nr) const;

    // events
    uint32 getNumberEvents() const;
    uint32 getEventSamplerate() const;

protected:
    typedef QVector<SignalChannel*> SignalChannelPtrVector;

    void resetBasicHeader();

    // basic
    QString type_;
    QString version_;
    QDateTime recording_time_;
    QString reference_;
    bool triggered_;
    int64 number_records_;
    float64 record_duration_;
    uint32 record_size_;

    // file
    QString full_file_name_;
    uint32 file_size_;

    // patient
    QString patient_name_;
    int32 patient_age_;
    Sex patient_sex_;
    QString patient_handedness_;
    QString patient_medication_;
    QString patient_classification_;
    uint64 doctor_id_;
    uint64 hospital_id_;

    // channels
    uint32 number_channels_;
    SignalChannelPtrVector channel_vector_;
    uint32 records_position_;

    // events
    uint32 number_events_;
    uint32 event_sample_rate_;
    uint32 event_table_position_;
};

} // namespace BioSig_

#endif
