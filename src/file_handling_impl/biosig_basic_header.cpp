#include "biosig_basic_header.h"

#include <ctime>
#include <cmath>

namespace SigViewer_
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
                user_defined_event_map_[index+1] = QString(raw_header->EVENT.CodeDesc[index]);
        }
    }


    setFileTypeString (QString (GetFileTypeString(raw_header->TYPE)).append(" v").append(QString::number(raw_header->VERSION)));

    float sampling_rate = raw_header->SampleRate;

    setSampleRate (sampling_rate);
    readChannelsInfo (raw_header);
    readPatientInfo (raw_header);
    readRecordingInfo (raw_header);
}

//-----------------------------------------------------------------------------
uint32 BiosigBasicHeader::getNumberOfSamples () const
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
    for (unsigned channel_index = 0; channel_index < raw_header->NS; channel_index++)
    {
        QString label = QString (QByteArray(raw_header->CHANNEL[channel_index].Label, MAX_LENGTH_LABEL)).trimmed();

        char p[MAX_LENGTH_PHYSDIM+1];
        p[MAX_LENGTH_PHYSDIM] = 0;
        PhysDim(raw_header->CHANNEL[channel_index].PhysDimCode, p);
        QString phys_y_dim_label = QString (p).trimmed();
        if (phys_y_dim_label.compare("uV") == 0)
            phys_y_dim_label = QString (QChar((ushort)0xb5)).append("V");
        QSharedPointer<SignalChannel> channel (new SignalChannel(channel_index, label,
                                                                 phys_y_dim_label));
        addChannel (channel_index, channel);
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
