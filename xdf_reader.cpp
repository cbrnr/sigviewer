#include "xdf_reader.h"
#include "biosig_basic_header.h"
#include "file_handler_factory_registrator.h"
#include "gui/progress_bar.h"
#include "base/fixed_data_block.h"
#include "biosig_reader.h"
#include "loadxdf.h"

#include "biosig.h"

#include <QTextStream>
#include <QTranslator>
#include <QMutexLocker>
#include <QDebug>
#include <QTime>
#include <QMessageBox>

#include <cmath>
#include <cassert>
#include <algorithm>
//#include <vector>
//#include <string>
#include <cstring>
#include <iostream>

using namespace std;

namespace SigViewer_
{

//-----------------------------------------------------------------------------

FILE_SIGNAL_READER_REGISTRATION(xdf, XDFReader);


//-----------------------------------------------------------------------------
XDFReader::XDFReader() :
    basic_header_ (0),
    xdf_header_ (0),
    buffered_all_channels_ (false),
    buffered_all_events_ (false)
{
    qDebug () << "Constructed XDFReader";
    // nothing to do here
}

//-----------------------------------------------------------------------------
XDFReader::~XDFReader()
{
    doClose();
}

//-----------------------------------------------------------------------------
QPair<FileSignalReader*, QString> XDFReader::createInstance (QString const& file_path)
{
    XDFReader* reader (new XDFReader);
    if (file_path.section('.', -1) == "evt")
        reader->buffered_all_channels_ = true;
    //std::string filename{ file_path.toStdString()};
    //reader->XDFdata = load_xdf(filename);

    QString error = reader->open (file_path);
    if (error.size() > 0)
    {
        qDebug () << error;
        //QMessageBox::critical(0, QObject::tr("Error"), error);
        return QPair<FileSignalReader*, QString> (0, error);
    }
    else
        return QPair<FileSignalReader*, QString> (reader, "");
}


//-----------------------------------------------------------------------------
void XDFReader::doClose () const
{
    if (xdf_header_)
        destructHDR (xdf_header_);
    xdf_header_ = NULL;
}

//-----------------------------------------------------------------------------
QSharedPointer<DataBlock const> XDFReader::getSignalData (ChannelID channel_id,
                                       size_t start_sample,
                                       size_t length) const
{
    QMutexLocker lock (&mutex_);

    if (!buffered_all_channels_)
        bufferAllChannels();

    if (!channel_map_.contains(channel_id))
        return QSharedPointer<DataBlock const> (0);

    if (length == basic_header_->getNumberOfSamples() &&
        start_sample == 0)
        return channel_map_[channel_id];
    else
        return channel_map_[channel_id]->createSubBlock (start_sample, length);
}

//-----------------------------------------------------------------------------
QList<QSharedPointer<SignalEvent const> > XDFReader::getEvents () const
{
    QMutexLocker lock (&mutex_);
    QList<QSharedPointer<SignalEvent const> > empty_list;
    if (!xdf_header_)
        return empty_list;

    if (!buffered_all_events_)
        bufferAllEvents();

    return events_;
}

//-----------------------------------------------------------------------------
QString XDFReader::open (QString const& file_name)
{

    QMutexLocker lock (&mutex_);
    return loadFixedHeader (file_name);
}

//by YL, pass the data in XDFdata to xdf_header_
void XDFReader::loadXDFHeader(XDFdataStruct &data, const QString& file_name)
{
    std::string name = file_name.toStdString();
    xdf_header_->FileName = const_cast<char*>(name.c_str());
    xdf_header_->VERSION = data.fileHeader.info.version;
    xdf_header_->TYPE = XDF;

    xdf_header_->data.size[0] = data.totalCh;
    xdf_header_->data.size[1] = data.totalLen;
    //xdf_header_->data.block =new biosig_data_type[xdf_header_->data.size[0]*xdf_header_->data.size[1]]();

    //IPaddr
    xdf_header_->SampleRate = data.majSR;
    xdf_header_->NRec = xdf_header_->data.size[1]; //1
    xdf_header_->T0 = 0;
    //HeadLen
    xdf_header_->SPR = 1; //xdf_header_->data.size[1];
    //LOC[4]
    xdf_header_->NS = data.totalCh;
    //tzmin
    //*Calib
    //*rerefCHANNEL

    //Patient

    //ID: N/A

    //ELEC: N/A

    //event
    xdf_header_->EVENT.N = XDFdata.eventMap.size();

    if (xdf_header_->EVENT.N != 0)
    {
        std::vector<std::string> dictionary;        //store unique event types
        for (size_t e = 0; e < XDFdata.eventMap.size(); e++)   //make another map that types must be unique
        {
            if (std::find(dictionary.begin(),dictionary.end(),XDFdata.eventMap[e].first) == dictionary.end())
            {
                dictionary.push_back(XDFdata.eventMap[e].first);
            }
        }

        xdf_header_->EVENT.SampleRate = data.majSR;
        xdf_header_->EVENT.TYP = new uint16_t[XDFdata.eventMap.size()];
        if (xdf_header_->EVENT.TYP == NULL)
        {
            std::cerr << "Couldn't allocate memory";
        }

        //for each event, look through the dictionary to find out its type number
        for (size_t t = 0; t < XDFdata.eventMap.size(); t++)
        {
            std::vector<std::string>::iterator it
                = std::find(dictionary.begin(), dictionary.end(), XDFdata.eventMap[t].first);

            if (it != dictionary.end())
                xdf_header_->EVENT.TYP[t] = std::distance(dictionary.begin(), it);
        }

        xdf_header_->EVENT.POS = new uint32_t[xdf_header_->EVENT.N];
        for (size_t t = 0; t < xdf_header_->EVENT.N; t++)
        {
            xdf_header_->EVENT.POS[t] = (XDFdata.eventMap[t].second - data.minTS) * data.majSR;
        }

        xdf_header_->EVENT.DUR = new uint32_t[xdf_header_->EVENT.N];
        for (size_t t = 0; t < xdf_header_->EVENT.N; t++)
        {
            xdf_header_->EVENT.DUR[t] = data.majSR;
        }

        xdf_header_->EVENT.CHN = new uint16_t[xdf_header_->EVENT.N]();
        xdf_header_->EVENT.TimeStamp = new gdf_time[xdf_header_->EVENT.N];
        for (size_t t = 0; t < xdf_header_->EVENT.N; t++)
        {
            xdf_header_->EVENT.TimeStamp[t] = XDFdata.eventMap[t].second;
        }

        xdf_header_->EVENT.LenCodeDesc = dictionary.size();

        xdf_header_->EVENT.CodeDesc = new const char*[dictionary.size()];
        for (size_t t = 0; t < dictionary.size();t++)
        {
            xdf_header_->EVENT.CodeDesc[t] = dictionary[t].c_str();
        }
    }

    //FLAG

    xdf_header_->CHANNEL = new CHANNEL_TYPE[xdf_header_->NS];
    for (size_t i = 0; i < xdf_header_->NS; i++)
    {
        std::string label = "CHN";
        label += std::to_string(i);
        label += "\nStream";
        for (size_t k = 0; k < data.streamMap.size(); k++)
        {
            if (i < data.streamMap[k].second)
            {
                int nb = data.streamMap[k].first;
                label += std::to_string(nb);
                label += '\n';
                label += data.streams[nb].info.name;
                label += '\n';
                label += data.streams[nb].info.type;
                break;
            }
        }
        int size = label.size();
        int maxLen = std::min(39,size);
        for (int j = 0; j< maxLen; j++)
        {
            xdf_header_->CHANNEL[i].Label[j] = label[j];
        }
        xdf_header_->CHANNEL[i].Label[maxLen] = '\0';
        xdf_header_->CHANNEL[i].OnOff = 1;
        xdf_header_->CHANNEL[i].LeadIdCode = 0;
        xdf_header_->CHANNEL[i].LowPass = 0.1;
        xdf_header_->CHANNEL[i].HighPass = 140;
        xdf_header_->CHANNEL[i].PhysDimCode = 4275;
    }
    //gzFID

    //FILE

    //AS : internal only

    //*aECG

    //viewtime

    //SCP

}

//-----------------------------------------------------------------------------
QString XDFReader::loadFixedHeader(const QString& file_name)
{
    QMutexLocker locker (&xdf_access_lock_);
    char *c_file_name = new char[file_name.length() + 1];
    strcpy (c_file_name, file_name.toLocal8Bit ().data());
    c_file_name[file_name.length()] = '\0';

    tzset();

    // set flags
    if(xdf_header_==NULL)
    {
        xdf_header_ = constructHDR (0,0);
        xdf_header_->FLAG.UCAL = 0;
        xdf_header_->FLAG.OVERFLOWDETECTION = 0;
    }

    xdf_header_ = sopen(c_file_name, "r", xdf_header_ );

    //by YL
    std::string filename{ file_name.toStdString()};
    this->XDFdata = load_xdf(filename);
    //XDFdataStruct XDFdata = load_xdf(filename);
    //loadXDFHeader(XDFdata, file_name);
    loadXDFHeader(this->XDFdata, file_name);


    basic_header_ = QSharedPointer<BasicHeader>
                    (new BiosigBasicHeader (xdf_header_, file_name));

    if (xdf_header_ == NULL) // || serror2(xdf_header_))
    {
            sclose (xdf_header_);
            destructHDR(xdf_header_);
            xdf_header_ = NULL;
            delete c_file_name;
            return "file not supported";
    }

    // (C) 2008 AS: EVENT.DUR and EVENT.CHN are optional in SOPEN, but SigViewer needs them.
    //convert2to4_eventtable(xdf_header_);

    //hdr2ascii(XDF_header_,stdout,4);

    delete c_file_name;
    c_file_name = NULL;

    basic_header_->setNumberEvents(xdf_header_->EVENT.N);


    if (xdf_header_->EVENT.SampleRate)
        basic_header_->setEventSamplerate(xdf_header_->EVENT.SampleRate);
    else
        basic_header_->setEventSamplerate(xdf_header_->SampleRate);

//#ifdef CHOLMOD_H
//    if (XDF_header_->Calib==NULL) {
//#endif
//    uint16_t chan=0;
//    for (uint16_t channel_index = 0; channel_index < XDF_header_->NS; ++channel_index)
//    if (XDF_header_->CHANNEL[channel_index].OnOff)	// show only selected channels - status channels are not shown.
//    {
//        SignalChannel* channel = new SignalChannel(chan++,
//                                                   XDF_header_->SPR,
//                                                   XDF_header_->CHANNEL[channel_index]);
//        basic_header_->addChannel(channel);
//    }
//#ifdef CHOLMOD_H
//    } else
//    for (uint16_t channel_index = 0; channel_index < XDF_header_->Calib->ncol; ++channel_index)
//    {
//        SignalChannel* channel = new SignalChannel(channel_index,
//                                                   XDF_header_->SPR,
//                                                   XDF_header_->rerefCHANNEL[channel_index]);
//        basic_header_->addChannel(channel);
//    }
//#endif
    return "";
}

//-----------------------------------------------------------------------------
QSharedPointer<BasicHeader> XDFReader::getBasicHeader ()
{
    //QMutexLocker lock (&mutex_);
    return basic_header_;
}

//-----------------------------------------------------------------------------
//original code
/*void XDFReader::bufferAllChannels () const
{
    size_t numberOfSamples = xdf_header_->NRec * xdf_header_->SPR;


    biosig_data_type* read_data = new biosig_data_type[numberOfSamples * basic_header_->getNumberChannels()];


    xdf_header_->FLAG.ROW_BASED_CHANNELS = 0;

    QString progress_name = QObject::tr("Loading data...");

    sread(read_data, 0, xdf_header_->NRec, xdf_header_);

    for (size_t i = 0; i<numberOfSamples* basic_header_->getNumberChannels(); i++ )
    {
        read_data[i]=xdf_header_->data.block[i];
    }

    for (unsigned channel_id = 0; channel_id < basic_header_->getNumberChannels();
         ++channel_id)

    {
        ProgressBar::instance().increaseValue (1, progress_name);

        QSharedPointer<QVector<float32> > raw_data(new QVector<float32> (numberOfSamples));
        for (size_t data_index = 0; data_index < numberOfSamples; data_index++)
            raw_data->operator [](data_index) = read_data[data_index + channel_id * numberOfSamples];
        QSharedPointer<DataBlock const> data_block(new FixedDataBlock(raw_data, basic_header_->getSampleRate()));
        channel_map_[channel_id] = data_block;
    }

    buffered_all_channels_ = true;
    if (buffered_all_events_)
        doClose();
    delete[] read_data;
}*/

//alteration by YL
void XDFReader::bufferAllChannels () const
{
    size_t numberOfSamples = xdf_header_->NRec * xdf_header_->SPR;

    xdf_header_->FLAG.ROW_BASED_CHANNELS = 0;

    QString progress_name = QObject::tr("Loading data...");


    //load all signals channel by channel!!
    unsigned channel_id = 0;
    for (size_t st = 0; st< XDFdata.streams.size(); st++)
    {
        if (XDFdata.streams[st].info.nominal_srate != 0)
        {
            int pos = (XDFdata.streams[st].time_stamps.front() - XDFdata.minTS) * XDFdata.majSR;
            if (XDFdata.streams[st].time_series.front().size() < XDFdata.totalLen-pos )
            {
                for (size_t ch = 0; ch < XDFdata.streams[st].time_series.size(); ch++)
                {
                    ProgressBar::instance().increaseValue (1, progress_name);

                    QSharedPointer<QVector<float32> > raw_data(new QVector<float32> (numberOfSamples,0));


                    std::copy(XDFdata.streams[st].time_series[ch].begin(),
                              XDFdata.streams[st].time_series[ch].end(),
                              raw_data->begin() + pos);
                    QSharedPointer<DataBlock const> data_block(new FixedDataBlock(raw_data, XDFdata.majSR));

                    channel_map_[channel_id] = data_block;
                    channel_id++;
                }
            }
            else
            {
                for (size_t ch = 0; ch < XDFdata.streams[st].time_series.size(); ch++)
                {
                    ProgressBar::instance().increaseValue (1, progress_name);

                    QSharedPointer<QVector<float32> > raw_data(new QVector<float32> (numberOfSamples,0));


                    std::copy(XDFdata.streams[st].time_series[ch].begin(),
                              XDFdata.streams[st].time_series[ch].end(),
                              raw_data->begin() + XDFdata.totalLen - XDFdata.streams[st].time_series[ch].size());
                    QSharedPointer<DataBlock const> data_block(new FixedDataBlock(raw_data, XDFdata.majSR));
                    channel_map_[channel_id] = data_block;
                    channel_id++;
                }
            }
        }
        //else if: irregualar samples
        else if (XDFdata.streams[st].info.nominal_srate == 0 && !XDFdata.streams[st].time_series.empty())
        {
            for (size_t ch = 0; ch < XDFdata.streams[st].time_series.size(); ch++)
            {
                ProgressBar::instance().increaseValue (1, progress_name);

                QSharedPointer<QVector<float32> > raw_data(new QVector<float32> (numberOfSamples,0));


                for (size_t i = 0; i < XDFdata.streams[st].time_series[ch].size(); i++)
                {
                    float* pt = raw_data->begin()  + (int)((XDFdata.streams[st].time_stamps[i]- XDFdata.minTS)* XDFdata.majSR);
                    *(pt) = XDFdata.streams[st].time_series[ch][i];

                    if (i != XDFdata.streams[st].time_stamps.size() - 1)
                    {
                        int interval = (XDFdata.streams[st].time_stamps[i+1]
                                        - XDFdata.streams[st].time_stamps[i]) * XDFdata.majSR;
                        for (int mid = 1; mid <= interval; mid++)
                            *(pt + mid) = XDFdata.streams[st].time_series[ch][i]
                                + mid * ((XDFdata.streams[st].time_series[ch][i+1]
                                - XDFdata.streams[st].time_series[ch][i])) / interval;
                    }
                }
                QSharedPointer<DataBlock const> data_block(new FixedDataBlock(raw_data, XDFdata.majSR));
                channel_map_[channel_id] = data_block;
                channel_id++;
            }
        }
    }


    buffered_all_channels_ = true;
    if (buffered_all_events_)
        doClose();
    //delete[] read_data;

}

//-------------------------------------------------------------------------
void XDFReader::bufferAllEvents () const
{
    unsigned number_events = xdf_header_->EVENT.N;


    // Hack Hack: Transforming Events to have the same sample rate as the signals
    double rate_transition = basic_header_->getEventSamplerate() / xdf_header_->EVENT.SampleRate;

    for (unsigned index = 0; index < number_events; index++)
    {
        /*QSharedPointer<SignalEvent> event (new SignalEvent (xdf_header_->EVENT.POS[index] * rate_transition,
                                                            xdf_header_->EVENT.TYP[index],
                                                            xdf_header_->EVENT.SampleRate * rate_transition));
        */
        QSharedPointer<SignalEvent> event (new SignalEvent (xdf_header_->EVENT.POS[index] * rate_transition,
                                                            xdf_header_->EVENT.TYP[index],
                                                            xdf_header_->EVENT.SampleRate * rate_transition));


        if (xdf_header_->EVENT.CHN)
        {
            if (xdf_header_->EVENT.CHN[index] == 0)
                event->setChannel (UNDEFINED_CHANNEL);
            else
                event->setChannel (xdf_header_->EVENT.CHN[index] - 1);

            if (xdf_header_->EVENT.TYP[index] != 0x7fff)
                event->setDuration (xdf_header_->EVENT.DUR[index] * rate_transition);
            else 	// sparse samples (Typ=0x7fff) do not have a duration, but the duration field is used to store the sample value
                event->setDuration(0);
        }
        else
        {
            event->setChannel(UNDEFINED_CHANNEL);
            event->setDuration (1);
        }
        events_.append (event);
    }

    buffered_all_events_ = true;
    if (buffered_all_channels_)
        doClose();
}

}
