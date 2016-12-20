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


#include "biosig_basic_header.h"

#include <ctime>
#include <cmath>

namespace sigviewer
{

//-----------------------------------------------------------------------------
BiosigBasicHeader::BiosigBasicHeader (HDRTYPE* raw_header, QString const& file_path)
    : BasicHeader (file_path),
      number_samples_ (raw_header->NRec * raw_header->SPR)
{
    if (raw_header->EVENT.CodeDesc)
    {
        for (unsigned index = 0; index < raw_header->EVENT.LenCodeDesc; index++)
        {
            if (raw_header->EVENT.CodeDesc[index])
                user_defined_event_map_[index] = QString(raw_header->EVENT.CodeDesc[index]);
        }
    }


    setFileTypeString (QString (GetFileTypeString(raw_header->TYPE)).append(" v").append(QString::number(raw_header->VERSION)));

    float64 sampling_rate = raw_header->SampleRate;

    setSampleRate (sampling_rate);
    readChannelsInfo (raw_header);
    readPatientInfo (raw_header);
    readRecordingInfo (raw_header);
}

//-----------------------------------------------------------------------------
size_t BiosigBasicHeader::getNumberOfSamples () const
{
    return ceil(static_cast<double>(number_samples_));
}

//-----------------------------------------------------------------------------
QMap<unsigned, QString> BiosigBasicHeader::getNamesOfUserSpecificEvents () const
{
    return user_defined_event_map_;
}

//-------------------------------------------------------------------------
void BiosigBasicHeader::readChannelsInfo (HDRTYPE const* raw_header)
{
    unsigned ch = 0;
    for (unsigned channel_index = 0; channel_index < raw_header->NS; channel_index++)
        if (raw_header->CHANNEL[channel_index].OnOff)
        {
            QSharedPointer<SignalChannel> channel(new SignalChannel(channel_index, raw_header));
            addChannel(ch++, channel);
        }
}

//-------------------------------------------------------------------------
void BiosigBasicHeader::readPatientInfo (HDRTYPE const* raw_header)
{
    switch (raw_header->Patient.Handedness)
    {
    case 1:
        addPatientInfo ("Handedness", "Right"); break;
    case 2:
        addPatientInfo ("Handedness", "Left"); break;
    case 3:
        addPatientInfo ("Handedness", "Equal"); break;
    }

    switch (raw_header->Patient.Sex)
    {
    case 1:
        addPatientInfo ("Sex", "Male"); break;
    case 2:
        addPatientInfo ("Sex", "Female"); break;
    }

    switch (raw_header->Patient.Smoking)
    {
    case 1:
        addPatientInfo ("Smoking", "No"); break;
    case 2:
        addPatientInfo ("Smoking", "Yes"); break;
    }
    switch (raw_header->Patient.Smoking)
    {
    case 1:
        addPatientInfo ("Smoking", "No"); break;
    case 2:
        addPatientInfo ("Smoking", "Yes"); break;
    }

    QString patient_id;
    for (unsigned i = 0; i < MAX_LENGTH_PID && raw_header->Patient.Id[i]; i++)
        patient_id.append (raw_header->Patient.Id[i]);
    addPatientInfo ("Id", patient_id.trimmed());

    if (raw_header->Patient.Birthday)
    {
        time_t birthday_t = mktime(gdf_time2tm_time (raw_header->Patient.Birthday));
        addPatientInfo ("Birthday", QString (ctime(&birthday_t)).trimmed());
    }

    if (raw_header->Patient.Weight)
        addPatientInfo ("Weight", QString::number(raw_header->Patient.Weight).append("kg"));
    if (raw_header->Patient.Height)
        addPatientInfo ("Height", QString::number(raw_header->Patient.Height).append("cm"));
}

//-------------------------------------------------------------------------
void BiosigBasicHeader::readRecordingInfo (HDRTYPE const* raw_header)
{
    if (raw_header->T0)
    {
        time_t recording_t = mktime(gdf_time2tm_time (raw_header->T0));
        addRecordingInfo("Recording Time", QString (ctime(&recording_t)).trimmed());
    }
}


} // namespace SigViewer_
