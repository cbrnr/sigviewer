#include "biosig_basic_header.h"

#include <ctime>

namespace BioSig_
{

//-----------------------------------------------------------------------------
BiosigBasicHeader::BiosigBasicHeader (HDRTYPE* raw_header)
    : number_samples_ (raw_header->NRec * raw_header->SPR)
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

    setSampleRate (raw_header->SampleRate);
    readChannelsInfo (raw_header);
    readPatientInfo (raw_header);
}

//-----------------------------------------------------------------------------
uint32 BiosigBasicHeader::getNumberOfSamples () const
{
    return number_samples_;
}

//-----------------------------------------------------------------------------
QMap<QString, QString> BiosigBasicHeader::getPatientInfo () const
{
    return patient_info_;
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
        QSharedPointer<SignalChannel> channel (new SignalChannel(channel_index, raw_header->CHANNEL[channel_index].SPR, label,
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
        patient_info_["Handedness"] = "Right"; break;
    case 2:
        patient_info_["Handedness"] = "Left"; break;
    case 3:
        patient_info_["Handedness"] = "Equal"; break;
    }

    switch (raw_header->Patient.Sex)
    {
    case 1:
        patient_info_["Sex"] = "Male"; break;
    case 2:
        patient_info_["Sex"] = "Female"; break;
    }

    switch (raw_header->Patient.Smoking)
    {
    case 1:
        patient_info_["Smoking"] = "No"; break;
    case 2:
        patient_info_["Smoking"] = "Yes"; break;
    }
    switch (raw_header->Patient.Smoking)
    {
    case 1:
        patient_info_["Smoking"] = "No"; break;
    case 2:
        patient_info_["Smoking"] = "Yes"; break;
    }

    for (unsigned i = 0; i < MAX_LENGTH_PID && raw_header->Patient.Id[i]; i++)
        patient_info_["Id"][i] = raw_header->Patient.Id[i];

    if (raw_header->Patient.Birthday)
    {
        time_t birthday_t = mktime(gdf_time2tm_time (raw_header->Patient.Birthday));
        patient_info_["Birthday"] = QString (ctime(&birthday_t)).trimmed();
    }

    if (raw_header->Patient.Weight)
        patient_info_["Weight"] = QString::number(raw_header->Patient.Weight).append("kg");
    if (raw_header->Patient.Height)
        patient_info_["Height"] = QString::number(raw_header->Patient.Height).append("cm");
}


} // namespace BioSig_
