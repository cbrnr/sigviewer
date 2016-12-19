// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


// basic_header.h

#ifndef BASIC_HEADER_H
#define BASIC_HEADER_H

#include "base/signal_channel.h"

#include <QString>
#include <QList>
#include <QDateTime>
#include <QMap>
#include <QSharedPointer>

namespace sigviewer
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
    float64 getSampleRate () const;

    //-------------------------------------------------------------------------
    QSharedPointer<SignalChannel const> getChannel (ChannelID id) const;

    //-------------------------------------------------------------------------
    unsigned getNumberChannels() const;

    //-------------------------------------------------------------------------
    virtual size_t getNumberOfSamples () const = 0;

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
    void setSampleRate (float64 sample_rate);

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
    QString const file_path_;
    QString file_type_string_;
    float64 sample_rate_;
    QMap<ChannelID, QSharedPointer<SignalChannel const> > channels_;
    QMap<QString, QString> recording_info_;
    QMap<QString, QString> patient_info_;

};

} // namespace SigViewer_

#endif
