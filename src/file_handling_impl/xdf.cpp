//libxdf is a static C++ library to load XDF files
//Copyright (C) 2017  Yida Lin

//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
//If you have questions, contact author at ITAL_FAZIOLI@hotmail.com


#include "xdf.h"

#include <iostream>
#include <fstream>
#include "pugixml/pugixml.hpp"  //pugi XML parser
#include <sstream>
#include <algorithm>
#include "smarc/smarc.h"      //resampling library
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */
#include <numeric>      //std::accumulate

Xdf::Xdf()
{
}

int Xdf::load_xdf(std::string filename)
{
    clock_t time;
    time = clock();


    /*	//uncompress if necessary
    char ext[_MAX_EXT]; //for file extension

    _splitpath_s ( argv[1], NULL, NULL, NULL, NULL, NULL, NULL, ext, NULL );
    if (strcmp(ext, ".xdfz") == 0)
    {
    //uncompress
    }
    */

    std::vector<int> idmap; //remaps stream id's onto indices in streams


    //===================================================================
    //========================= parse the file ==========================
    //===================================================================


    std::ifstream file(filename, std::ios::in | std::ios::binary);

    if (file.is_open())
    {
        //read [MagicCode]
        std::string magicNumber;
        for (char c; file >> c;)
        {
            magicNumber.push_back(c);
            if (magicNumber.size() == 4)
                break;
        }

        if (magicNumber.compare("XDF:"))
        {
            std::cout << "This is not a valid XDF file.('" << filename << "')\n";
            return -1;
        }

        //for each chunk
        while (1)
        {
            uint64_t ChLen = readLength(file);//chunk length

            if (ChLen == 0)
                break;

            uint16_t tag;   //read tag of the chunk, 6 possibilities
            file.read(reinterpret_cast<char *>(&tag), 2);

            switch (tag)
            {
            case 1: //[FileHeader]
            {
                char* buffer = new char[ChLen - 2];
                file.read(buffer, ChLen - 2);
                fileHeader = buffer;

                pugi::xml_document doc;

                doc.load_buffer_inplace(buffer, ChLen - 2);

                pugi::xml_node info = doc.child("info");

                version = info.child("version").text().as_float();

                delete[] buffer;
            }
                break;
            case 2: //read [StreamHeader] chunk
            {
                //read [StreamID]
                int streamID;
                int index;
                file.read((char*)&streamID, 4);
                std::vector<int>::iterator it {std::find(idmap.begin(),idmap.end(),streamID)};
                if (it == idmap.end())
                {
                    index = idmap.size();
                    idmap.emplace_back(streamID);
                    streams.emplace_back();
                }
                else
                    index = std::distance(idmap.begin(), it);


                pugi::xml_document doc;

                //read [Content]
                char* buffer = new char[ChLen - 6];
                file.read(buffer, ChLen - 6);
                streams[index].streamHeader = buffer;

                doc.load_buffer_inplace(buffer, ChLen - 6);

                pugi::xml_node info = doc.child("info");
                pugi::xml_node desc = info.child("desc");

                streams[index].info.channel_count = info.child("channel_count").text().as_int();
                streams[index].info.nominal_srate = info.child("nominal_srate").text().as_double();
                streams[index].info.name = info.child("name").text().get();
                streams[index].info.type = info.child("type").text().get();
                streams[index].info.channel_format = info.child("channel_format").text().get();

                for (auto channel = desc.child("channels").child("channel"); channel; channel = channel.next_sibling("channel"))
                {
                    streams[index].info.channels.emplace_back();

                    for (auto const &entry : channel.children())
                        streams[index].info.channels.back().emplace(entry.name(), entry.child_value());
                }

                if (streams[index].info.nominal_srate > 0)
                    streams[index].sampling_interval = 1 / streams[index].info.nominal_srate;
                else
                    streams[index].sampling_interval = 0;

                delete[] buffer;
            }
                break;
            case 3: //read [Samples] chunk
            {
                //read [StreamID]
                int streamID;
                int index;
                file.read((char*)&streamID, 4);
                std::vector<int>::iterator it {std::find(idmap.begin(),idmap.end(),streamID)};
                if (it == idmap.end())
                {
                    index = idmap.size();
                    idmap.emplace_back(streamID);
                    streams.emplace_back();
                }
                else
                    index = std::distance(idmap.begin(), it);


                //read [NumSampleBytes], [NumSamples]
                uint64_t numSamp = readLength(file);

                //check the data type
                if (streams[index].info.channel_format.compare("float32") == 0)
                {
                    //if the time series is empty
                    if (streams[index].time_series.empty())
                        streams[index].time_series.resize(streams[index].info.channel_count);

                    //for each sample
                    for (size_t i = 0; i < numSamp; i++)
                    {
                        //read or deduce time stamp
                        uint8_t tsBytes;
                        file.read(reinterpret_cast<char *>(&tsBytes), 1);

                        double ts;  //temporary time stamp

                        if (tsBytes == 8)
                        {
                            file.read((char*)&ts, 8);
                            streams[index].time_stamps.emplace_back(ts);
                        }
                        else
                        {
                            ts = streams[index].last_timestamp + streams[index].sampling_interval;
                            streams[index].time_stamps.emplace_back(ts);
                        }

                        streams[index].last_timestamp = ts;

                        //read the data
                        for (int v = 0; v < streams[index].info.channel_count; ++v)
                        {
                            float data;
                            file.read((char*)&data, 4);
                            streams[index].time_series[v].emplace_back(data);
                        }
                    }
                }
                else if (streams[index].info.channel_format.compare("double64") == 0)
                {
                    //if the time series is empty
                    if (streams[index].time_series.empty())
                        streams[index].time_series.resize(streams[index].info.channel_count);

                    //for each sample
                    for (size_t i = 0; i < numSamp; i++)
                    {
                        //read or deduce time stamp
                        uint8_t tsBytes;
                        file.read(reinterpret_cast<char *>(&tsBytes), 1);

                        double ts;  //temporary time stamp

                        if (tsBytes == 8)
                        {
                            file.read((char*)&ts, 8);
                            streams[index].time_stamps.emplace_back(ts);
                        }
                        else
                        {
                            ts = streams[index].last_timestamp + streams[index].sampling_interval;
                            streams[index].time_stamps.emplace_back(ts);
                        }

                        streams[index].last_timestamp = ts;

                        //read the data
                        for (int v = 0; v < streams[index].info.channel_count; ++v)
                        {
                            double data;
                            file.read((char*)&data, 8);
                            streams[index].time_series[v].emplace_back(data);
                        }
                    }
                }
                else if (streams[index].info.channel_format.compare("int8") == 0)
                {
                    //if the time series is empty
                    if (streams[index].time_series.empty())
                        streams[index].time_series.resize(streams[index].info.channel_count);

                    //for each sample
                    for (size_t i = 0; i < numSamp; i++)
                    {
                        //read or deduce time stamp
                        uint8_t tsBytes;
                        file.read(reinterpret_cast<char *>(&tsBytes), 1);

                        double ts;  //temporary time stamp

                        if (tsBytes == 8)
                        {
                            file.read((char*)&ts, 8);
                            streams[index].time_stamps.emplace_back(ts);
                        }
                        else
                        {
                            ts = streams[index].last_timestamp + streams[index].sampling_interval;
                            streams[index].time_stamps.emplace_back(ts);
                        }

                        streams[index].last_timestamp = ts;

                        //read the data
                        for (int v = 0; v < streams[index].info.channel_count; ++v)
                        {
                            int8_t data;
                            file.read((char*)&data, 1);
                            streams[index].time_series[v].emplace_back(data);
                        }
                    }
                }
                else if (streams[index].info.channel_format.compare("int16") == 0)
                {
                    //if the time series is empty
                    if (streams[index].time_series.empty())
                        streams[index].time_series.resize(streams[index].info.channel_count);

                    //for each sample
                    for (size_t i = 0; i < numSamp; i++)
                    {
                        //read or deduce time stamp
                        uint8_t tsBytes;
                        file.read(reinterpret_cast<char *>(&tsBytes), 1);

                        double ts;  //temporary time stamp

                        if (tsBytes == 8)
                        {
                            file.read((char*)&ts, 8);
                            streams[index].time_stamps.emplace_back(ts);
                        }
                        else
                        {
                            ts = streams[index].last_timestamp + streams[index].sampling_interval;
                            streams[index].time_stamps.emplace_back(ts);
                        }

                        streams[index].last_timestamp = ts;

                        //read the data
                        for (int v = 0; v < streams[index].info.channel_count; ++v)
                        {
                            int16_t data;
                            file.read((char*)&data, 2);
                            streams[index].time_series[v].emplace_back(data);
                        }
                    }
                }
                else if (streams[index].info.channel_format.compare("int32") == 0)
                {
                    //if the time series is empty
                    if (streams[index].time_series.empty())
                        streams[index].time_series.resize(streams[index].info.channel_count);

                    //for each sample
                    for (size_t i = 0; i < numSamp; i++)
                    {
                        //read or deduce time stamp
                        uint8_t tsBytes;
                        file.read(reinterpret_cast<char *>(&tsBytes), 1);

                        double ts;  //temporary time stamp

                        if (tsBytes == 8)
                        {
                            file.read((char*)&ts, 8);
                            streams[index].time_stamps.emplace_back(ts);
                        }
                        else
                        {
                            ts = streams[index].last_timestamp + streams[index].sampling_interval;
                            streams[index].time_stamps.emplace_back(ts);
                        }

                        streams[index].last_timestamp = ts;

                        //read the data
                        for (int v = 0; v < streams[index].info.channel_count; ++v)
                        {
                            int32_t data;
                            file.read((char*)&data, 4);
                            streams[index].time_series[v].emplace_back(data);
                        }
                    }
                }
                else if (streams[index].info.channel_format.compare("int64") == 0)
                {
                    //if the time series is empty
                    if (streams[index].time_series.empty())
                        streams[index].time_series.resize(streams[index].info.channel_count);

                    //for each sample
                    for (size_t i = 0; i < numSamp; i++)
                    {
                        //read or deduce time stamp
                        uint8_t tsBytes;
                        file.read(reinterpret_cast<char *>(&tsBytes), 1);

                        double ts;  //temporary time stamp

                        if (tsBytes == 8)
                        {
                            file.read((char*)&ts, 8);
                            streams[index].time_stamps.emplace_back(ts);
                        }
                        else
                        {
                            ts = streams[index].last_timestamp + streams[index].sampling_interval;
                            streams[index].time_stamps.emplace_back(ts);
                        }

                        streams[index].last_timestamp = ts;

                        //read the data
                        for (int v = 0; v < streams[index].info.channel_count; ++v)
                        {
                            int64_t data;
                            file.read((char*)&data, 8);
                            streams[index].time_series[v].emplace_back(data);
                        }
                    }
                }
                else if (streams[index].info.channel_format.compare("string") == 0)
                {
                    //for each event
                    for (size_t i = 0; i < numSamp; i++)
                    {
                        //read or deduce time stamp
                        uint8_t tsBytes;
                        file.read(reinterpret_cast<char *>(&tsBytes), 1);

                        double ts;  //temporary time stamp

                        if (tsBytes == 8)
                            file.read((char*)&ts, 8);
                        else
                            ts = streams[index].last_timestamp + streams[index].sampling_interval;

                        //read the event
                        int8_t bytes;
                        file.read((char*)&bytes, 1);
                        if (bytes == 1)
                        {
                            uint8_t length;
                            file.read((char*)&length, 1);
                            char* buffer = new char[length + 1];
                            file.read(buffer, length);
                            buffer[length] = '\0';
                            eventMap.emplace_back(std::make_pair(buffer, ts), index);
                            delete[] buffer;
                        }
                        else if (bytes == 4)
                        {
                            uint32_t length;
                            file.read((char*)&length, 4);
                            char* buffer = new char[length + 1];
                            file.read(buffer, length);
                            buffer[length] = '\0';
                            eventMap.emplace_back(std::make_pair(buffer, ts), index);
                            delete[] buffer;
                        }
                        else if (bytes == 8)
                        {
                            uint64_t length;
                            file.read((char*)&length, 8);
                            char* buffer = new char[length + 1];
                            file.read(buffer, length);
                            buffer[length] = '\0';
                            eventMap.emplace_back(std::make_pair(buffer, ts), index);
                            delete[] buffer;
                        }

                        streams[index].last_timestamp = ts;
                    }
                }
            }
                break;
            case 4: //read [ClockOffset] chunk
            {
                int streamID;
                int index;
                file.read((char*)&streamID, 4);
                std::vector<int>::iterator it {std::find(idmap.begin(),idmap.end(),streamID)};
                if (it == idmap.end())
                {
                    index = idmap.size();
                    idmap.emplace_back(streamID);
                    streams.emplace_back();
                }
                else
                    index = std::distance(idmap.begin(), it);


                double collectionTime;
                double offsetValue;

                file.read((char*)&collectionTime, 8);
                file.read((char*)&offsetValue, 8);

                streams[index].clock_times.emplace_back(collectionTime);
                streams[index].clock_values.emplace_back(offsetValue);
            }
                break;
            case 6: //read [StreamFooter] chunk
            {
                pugi::xml_document doc;

                //read [StreamID]
                int streamID;
                int index;
                file.read((char*)&streamID, 4);
                std::vector<int>::iterator it {std::find(idmap.begin(),idmap.end(),streamID)};
                if (it == idmap.end())
                {
                    index = idmap.size();
                    idmap.emplace_back(streamID);
                    streams.emplace_back();
                }
                else
                    index = std::distance(idmap.begin(), it);


                char* buffer = new char[ChLen - 6];
                file.read(buffer, ChLen - 6);
                streams[index].streamFooter = buffer;

                doc.load_buffer_inplace(buffer, ChLen - 6);

                pugi::xml_node info = doc.child("info");

                streams[index].info.first_timestamp = info.child("first_timestamp").text().as_double();
                streams[index].info.last_timestamp = info.child("last_timestamp").text().as_double();
                streams[index].info.measured_srate = info.child("measured_srate").text().as_double();
                streams[index].info.sample_count = info.child("sample_count").text().as_int();
                delete[] buffer;
            }
                break;
            case 5:	//skip other chunk types (Boundary, ...)
                file.seekg(ChLen - 2, file.cur);
                break;
            default:
                std::cout << "Unknown chunk encountered.\n";
                break;
            }
        }


        //calculate how much time it takes to read the data
        clock_t halfWay = clock() - time;

        std::cout << "it took " << halfWay << " clicks (" << ((float)halfWay) / CLOCKS_PER_SEC << " seconds)"
                  << " reading XDF data" << std::endl;


        //==========================================================
        //=============find the min and max time stamps=============
        //==========================================================

        findMinMax();

        findMajSR();

        getHighestSampleRate();

        loadSampleRateMap();

        calcTotalChannel();

        loadDictionary();

        calcEffectiveSrate();

        //loading finishes, close file
        file.close();
    }
    else
    {
        std::cout << "Unable to open file" << std::endl;
        return 1;
    }

    return 0;
}

void Xdf::resample(int userSrate)
{
    //if user entered a preferred sample rate, we resample all the channels to that sample rate
    //Otherwise, we resample all channels to the sample rate that has the most channels

    clock_t time = clock();

#define BUF_SIZE 8192
    for (auto &stream : streams)
    {
        if (!stream.time_series.empty() &&
                stream.info.nominal_srate != userSrate &&
                stream.info.nominal_srate != 0)
        {
            int fsin = stream.info.nominal_srate;       // input samplerate
            int fsout = userSrate;                      // output samplerate
            double bandwidth = 0.95;                    // bandwidth
            double rp = 0.1;                            // passband ripple factor
            double rs = 140;                            // stopband attenuation
            double tol = 0.000001;                      // tolerance

            // initialize smarc filter
            struct PFilter* pfilt = smarc_init_pfilter(fsin, fsout, bandwidth, rp,
                                                       rs, tol, NULL, 0);
            if (pfilt == NULL)
                continue;

            // initialize smarc filter state
            struct PState* pstate = smarc_init_pstate(pfilt);

            for (auto &row : stream.time_series)
            {
                // initialize buffers
                int read = 0;
                int written = 0;
                const int OUT_BUF_SIZE = (int) smarc_get_output_buffer_size(pfilt, row.size());
                double* inbuf = new double[row.size()];
                double* outbuf = new double[OUT_BUF_SIZE];


                std::copy(row.begin(), row.end(), inbuf);

                read = row.size();

                // resample signal block
                written = smarc_resample(pfilt, pstate, inbuf, read, outbuf, OUT_BUF_SIZE);

                // do what you want with your output
                row.resize(written);
                std::copy ( outbuf, outbuf+written, row.begin() );

                // flushing last values
                written = smarc_resample_flush(pfilt, pstate, outbuf, OUT_BUF_SIZE);

                // do what you want with your output
                row.resize(row.size() + written);
                std::copy ( outbuf, outbuf+written, row.begin() + row.size() - written );

                // you are done with converting your signal.
                // If you want to reuse the same converter to process another signal
                // just reset the state:
                smarc_reset_pstate(pstate,pfilt);

                delete[] inbuf;
                delete[] outbuf;
            }
            // release smarc filter state
            smarc_destroy_pstate(pstate);

            // release smarc filter
            smarc_destroy_pfilter(pfilt);
        }
    }
    //resampling finishes here


    //======================================================================
    //===========Calculating total length & total channel count=============
    //======================================================================


    calcTotalLength(userSrate);

    adjustTotalLength();

    time = clock() - time;

    std::cout << "it took " << time << " clicks (" << ((float)time) / CLOCKS_PER_SEC << " seconds)"
              << " resampling" << std::endl;
}

//function of reading the length of each chunk
uint64_t Xdf::readLength(std::ifstream &file)
{
    uint8_t bytes;
    file.read((char*)&bytes, 1);
    uint64_t length = 0;

    switch (bytes)
    {
    case 1:
    {
        uint8_t buffer = 0;
        file.read(reinterpret_cast<char *>(&buffer), 1);
        length = buffer;
        return length;
    }
    case 4:
    {
        uint32_t buffer = 0;
        file.read(reinterpret_cast<char *>(&buffer), 4);
        length = buffer;
        return length;
    }
    case 8:
    {
        file.read(reinterpret_cast<char *>(&length), 8);
        return length;
    }
    default:
        std::cout << "Invalid variable-length integer encountered.\n";
        return 0;
    }
}

void Xdf::findMinMax()
{
    //find the smallest timestamp of all streams
    for (auto const &stream : streams)
    {
        if (stream.info.first_timestamp != -1)
        {
            minTS = stream.info.first_timestamp;
            break;
        }
    }
    for (auto const &stream : streams)
    {
        if (stream.info.first_timestamp != -1 && stream.info.first_timestamp < minTS)
            minTS = stream.info.first_timestamp;
    }

    //find the max timestamp of all streams
    for (auto const &stream : streams)
    {
        if (stream.info.last_timestamp != 0 && stream.info.last_timestamp > maxTS)
            maxTS = stream.info.last_timestamp;
    }
}

void Xdf::findMajSR()
{
    // find out which sample rate has the most channels
    typedef int sampRate;
    typedef int numChannel;

    std::vector<std::pair<sampRate, numChannel> > srateMap; //<srate, numChannels> pairs of all the streams

    //find out whether a sample rate already exists in srateMap
    for (auto const &stream : streams)
    {
        if (stream.info.nominal_srate != 0)
        {
            std::vector<std::pair<sampRate, numChannel> >::iterator it {std::find_if(srateMap.begin(), srateMap.end(),
                                  [&](const std::pair<sampRate, numChannel> &element)
                                        {return element.first == stream.info.nominal_srate; })} ;
            //if it doesn't, add it here
            if (it == srateMap.end())
                srateMap.emplace_back(stream.info.nominal_srate, stream.info.channel_count);
            //if it already exists, add additional channel numbers to that sample rate
            else
            {
                int index (std::distance(srateMap.begin(),it)) ;
                srateMap[index].second += stream.info.channel_count;
            }
        }
    }

    //search the srateMap to see which sample rate has the most channels
    int index (std::distance(srateMap.begin(),
                             std::max_element(srateMap.begin(),srateMap.end(),
                                            [] (const std::pair<sampRate, numChannel> &largest,
                                            const std::pair<sampRate, numChannel> &first)
                                            { return largest.second < first.second; })));

    majSR = srateMap[index].first; //the sample rate that has the most channels
}

void Xdf::calcTotalChannel()
{
    //calculating total channel count, and indexing them onto streamMap
    for (size_t c = 0; c < streams.size(); c++)
    {
        if(!streams[c].time_series.empty())
        {
            totalCh += streams[c].info.channel_count;

            for (int i = 0; i < streams[c].info.channel_count; i++)
                streamMap.emplace_back(c);
        }
    }
}

void Xdf::calcTotalLength(int sampleRate)
{
    totalLen = (maxTS - minTS) * sampleRate;
}

void Xdf::freeUpTimeStamps()
{
    //free up as much memory as possible
    for (auto &stream : streams)
    {
        //we don't need to keep all the time stamps unless it's a stream with irregular samples
        //filter irregular streams and string streams
        if (stream.info.nominal_srate != 0 && !stream.time_stamps.empty() && stream.info.channel_format.compare("string"))
        {
            std::vector<double> nothing;
            //however we still need to keep the first time stamp of each stream to decide at which position the signal should start
            nothing.emplace_back(stream.time_stamps.front());
            stream.time_stamps.swap(nothing);
        }
    }
}

void Xdf::adjustTotalLength()
{
    for (auto const &stream : streams)
    {
        if(!stream.time_series.empty())
        {
            if (totalLen < stream.time_series.front().size())
                totalLen = stream.time_series.front().size();
        }
    }
}

void Xdf::getHighestSampleRate()
{
    for (auto const &stream : streams)
    {
        if (stream.info.nominal_srate > maxSR)
            maxSR = stream.info.nominal_srate;
    }
}

void Xdf::loadSampleRateMap()
{
    for (auto const &stream : streams)
        sampleRateMap.emplace(stream.info.nominal_srate);
}

void Xdf::detrend()
{
    for (auto &stream : streams)
    {
        for (auto &row : stream.time_series)
        {
            long double init = 0.0;
            long double mean = std::accumulate(row.begin(), row.end(), init) / row.size();
            std::transform(row.begin(), row.end(), row.begin(), bind2nd(std::minus<double>(), mean));
            offsets.emplace_back(mean);
        }
    }
}

void Xdf::calcEffectiveSrate()
{
    for (auto &stream : streams)
    {
        if (stream.info.nominal_srate)
        {
            try
            {
                stream.info.effective_sample_rate
                        = stream.info.sample_count /
                        (stream.info.last_timestamp - stream.info.first_timestamp);

                pugi::xml_document doc;
                doc.load_string(stream.streamFooter.c_str());
                pugi::xml_node sampleCount = doc.child("info").child("sample_count");
                pugi::xml_node effectiveSampleRate
                        = doc.child("info").insert_child_after("effective_sample_rate", sampleCount);
                effectiveSampleRate.append_child(pugi::node_pcdata)
                        .set_value(std::to_string(stream.info.effective_sample_rate).c_str());

                std::stringstream buffer;
                doc.save(buffer);

                stream.streamFooter = buffer.str();
            }
            catch (std::exception &e)
            {
                std::cerr << "Error calculating effective sample rate. "
                          << e.what() << std::endl;
            }
        }
    }
}

int Xdf::writeEventsToXDF(std::string file_path)
{
    if (userAddedStream)
    {
        std::fstream file;
        file.open(file_path, std::ios::app | std::ios::binary);

        if (file.is_open())
        {
            //start to append to new XDF file
            //first write a stream header chunk
            //Num Length Bytes
            file.put(4);
            //length
            int length = streams[userAddedStream].streamHeader.size() + 6; //+6 because of the length int itself and short int tag
            file.write((char*)&length, 4);

            //tag
            short tag = 2;
            file.write((char*)&tag, 2);
            //streamNumber
            int streamNumber = userAddedStream + 1; //+1 because the stream IDs in XDF are 1 based instead of 0 based
            file.write((char*)&streamNumber, 4);
            //content
            file.write(streams[userAddedStream].streamHeader.c_str(), length - 6);//length - 6 is the string length

            //write samples chunk
            //Num Length Bytes
            file.put(8);
            //length
            //add the bytes of all following actions together
            int64_t stringTotalLength = 0;
            for (auto const &event : userCreatedEvents)
                stringTotalLength += event.first.size();

            int64_t sampleChunkLength = 2 + 4 + 1 + 4 +
                    userCreatedEvents.size() *
                    (1 + 8 + 1 + 4) + stringTotalLength;
            file.write((char*)&sampleChunkLength, 8);


            //tag
            tag = 3;
            file.write((char*)&tag, 2);
            //streamNumber
            file.write((char*)&streamNumber, 4);
            //content
            //NumSamplesBytes
            file.put(4);

            //Num Samples
            int numSamples = userCreatedEvents.size();
            file.write((char*)&numSamples, 4);

            //samples
            for (auto const &event : userCreatedEvents)
            {
                //TimeStampBytes
                file.put(8);

                //Optional Time Stamp
                double timeStamp = event.second;
                file.write((char*)&timeStamp, 8);

                //Num Length Bytes
                file.put(4);

                //Length
                int stringLength = event.first.length();
                file.write((char*)&stringLength, 4);

                //String Content
                file.write(event.first.c_str(), stringLength);
            }

            file.close();
        }
        else
        {
            std::cerr << "Unable to open file." << std::endl;
            return -1; //Error
        }
    }

    std::cout << "Succesfully wrote to XDF file." << std::endl;

    return 0; //Success
}

void Xdf::createLabels()
{
    size_t channelCount = 0;

    for (size_t st = 0; st < streams.size(); st++)
    {
        if (streams[st].info.channels.size())
        {
            for (size_t ch = 0; ch < streams[st].info.channels.size(); ch++)
            {
                // +1 for 1 based numbers; for user convenience only. The internal computation is still 0 based
                std::string label = "Stream " + std::to_string(st + 1) + " - Channel " + std::to_string(ch + 1)
                        + " - " + std::to_string((int)streams[st].info.nominal_srate) + " Hz\n";

                label += streams[st].info.name + '\n';

                for (auto const &entry : streams[st].info.channels[ch])
                {
                    if (entry.second != "")
                        label += entry.first + " : " + entry.second + '\n';
                }
                if (offsets.size())
                {
                    if (offsets[channelCount] >= 0)
                        label.append("baseline +").append(std::to_string(offsets[channelCount]));
                    else
                        label.append("baseline ").append(std::to_string(offsets[channelCount]));
                }
                labels.emplace_back(label);

                channelCount++;
            }
        }
        else
        {
            for (size_t ch = 0; ch < streams[st].time_series.size(); ch++)
            {
                // +1 for 1 based numbers; for user convenience only. The internal computation is still 0 based
                std::string label = "Stream " + std::to_string(st + 1) +
                        " - Channel " + std::to_string(ch + 1) + " - " +
                        std::to_string((int)streams[st].info.nominal_srate) +
                        " Hz\n" + streams[st].info.name + '\n' + streams[st].info.type + '\n';

                label += streams[st].info.name + '\n';

                if (offsets.size())
                {
                    if (offsets[channelCount] >= 0)
                        label.append("baseline +").append(std::to_string(offsets[channelCount]));
                    else
                        label.append("baseline ").append(std::to_string(offsets[channelCount]));
                }

                labels.emplace_back(label);

                channelCount++;
            }
        }
    }
}

void Xdf::loadDictionary()
{
    //loop through the eventMap
    for (auto const &entry : eventMap)
    {
        //search the dictionary to see whether an event is already in it
        auto it = std::find(dictionary.begin(),dictionary.end(),entry.first.first);
        //if it isn't yet
        if (it == dictionary.end())
        {   //add it to the dictionary, also store its index into eventType vector for future use
            eventType.emplace_back(dictionary.size());
            dictionary.emplace_back(entry.first.first);
        }
        //if it's already in there
        else    //store its index into eventType vector
            eventType.emplace_back(std::distance(dictionary.begin(), it));
    }
}
