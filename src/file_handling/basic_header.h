// basic_header.h

#ifndef BASIC_HEADER_H
#define BASIC_HEADER_H

#include "base/signal_channel.h"
#include "base/filter.h"

#include <QString>
#include <QList>
#include <QDateTime>
#include <QMap>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
/// @class BasicHeader (common for all file formats)
/// @brief base class for any biosignal file header
class BasicHeader
{
public:
    //-------------------------------------------------------------------------
    virtual ~BasicHeader() {}

    //-------------------------------------------------------------------------
    QString getFileTypeString () const;

    //-------------------------------------------------------------------------
    QString getFilePath () const {return file_path_;}

    //-------------------------------------------------------------------------
    virtual QMap<QString, QString> getPatientInfo () const
    {return patient_info_;}

    //-------------------------------------------------------------------------
    /// may include recording time, triggered, etc.
    virtual QMap<QString, QString> getRecordingInfo () const
    {return recording_info_;}

    //-------------------------------------------------------------------------
    float getSampleRate () const;

    //-------------------------------------------------------------------------
    QList<QSharedPointer<Filter const> > getFilters () const
    {return filters_;}

    //-------------------------------------------------------------------------
    ///
    /// @return index of the added filter
    int addFilter (QSharedPointer<Filter> filter, int before_index = -1);

    //-------------------------------------------------------------------------
    void removeFilter (int filter_index)
    {if (filters_.size() > filter_index && filter_index >= 0) filters_.removeAt (filter_index);}

    //-------------------------------------------------------------------------
    void setDownSamplingFactor (int downsampling_factor)
    {sample_rate_ *= downsampling_factor_;
     sample_rate_ /= downsampling_factor;
     downsampling_factor_ = downsampling_factor;}

    //-------------------------------------------------------------------------
    int getDownSamplingFactor () const
    {return downsampling_factor_;}

    //-------------------------------------------------------------------------
    QSharedPointer<SignalChannel const> getChannel (ChannelID id) const;

    //-------------------------------------------------------------------------
    unsigned getNumberChannels() const;

    //-------------------------------------------------------------------------
    virtual uint32 getNumberOfSamples () const = 0;

    //-------------------------------------------------------------------------
    virtual QMap<unsigned, QString> getNamesOfUserSpecificEvents () const
    {return QMap<unsigned, QString>();}

    uint32 getNumberEvents() const;
    void setNumberEvents (uint32 number_events);
    double getEventSamplerate() const;
    void setEventSamplerate (double event_sample_rate);


protected:
    //-------------------------------------------------------------------------
    BasicHeader (QString const& file_path) : file_path_ (file_path) {}

    // events
    uint32 number_events_;
    double event_sample_rate_;
    
    //-------------------------------------------------------------------------
    /// optional
    void setFileTypeString (QString const& file_type_string);

    //-------------------------------------------------------------------------
    /// required
    void setSampleRate (float sample_rate, int downsampling_factor = 1);

    //-------------------------------------------------------------------------
    /// required
    void addChannel (ChannelID id, QSharedPointer<SignalChannel const> channel);

    //-------------------------------------------------------------------------
    /// optional
    void addRecordingInfo (QString const& info_label, QString const& value)
    {recording_info_[info_label] = value;}

    //-------------------------------------------------------------------------
    /// optional
    void addPatientInfo (QString const& info_label, QString const& value)
    {patient_info_[info_label] = value;}

private:
    QList<QSharedPointer<Filter const> > filters_;
    QString const file_path_;
    QString file_type_string_;
    float sample_rate_;
    int downsampling_factor_;
    QMap<ChannelID, QSharedPointer<SignalChannel const> > channels_;
    QMap<QString, QString> recording_info_;
    QMap<QString, QString> patient_info_;

};

} // namespace SigViewer_

#endif
