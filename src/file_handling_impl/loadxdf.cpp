// load_xdf.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <fstream>
#include <stdlib.h>     //EXIT_FAILURE, NULL, abs
//#include <stdio.h>      //size_t
//#include <cmath>        //abs
//#include <vector>
#include "pugixml.hpp"  //pugi XML parser
#include <sstream>
#include <algorithm>
#include "loadxdf.h"
#include "smarc.h"      //resampling library
//#include <cstring>
//#include <string>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */


//======================= Functions ==============================

//function of reading the length of each chunk
uint64_t readLength(std::ifstream &file)
{
    uint8_t bytes;
    file.read((char*)&bytes, 1);
    uint64_t length;

    switch (bytes)
    {
    case 1:
    {
        uint8_t buffer;
        file.read(reinterpret_cast<char *>(&buffer), 1);
        length = buffer;
        return length;
    }
    case 4:
    {
        uint32_t buffer;
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

/*
template <typename T>
double getMedian(std::vector<T> vector)
{
    std::sort(vector.begin(), vector.end());
    if (vector.size() % 2 != 0)
        return vector[vector.size() / 2];
    else
        return (double)(vector[vector.size() / 2] + vector[vector.size() / 2 - 1])/2;
}

template <typename T>
double getMad(std::vector<T> vector) //MEDIAN absolute deviation
{
    double median{ getMedian(vector) };
    std::vector<double> temp;

    for (auto item : vector)
        temp.push_back(abs(item - median));

    return getMedian(temp);
}
*/


XDFdataStruct load_xdf(std::string filename)
{
    clock_t time;
    time = clock();


    Opts options;

    if (options.verbose == true)
        std::cout << "Importing XDF file " << filename << "...\n";


    /*	//uncompress if necessary
    char ext[_MAX_EXT]; //for file extension

    _splitpath_s ( argv[1], NULL, NULL, NULL, NULL, NULL, NULL, ext, NULL );
    if (strcmp(ext, ".xdfz") == 0)
    {
    //uncompress
    }
    */


    XDFdataStruct XDFdata; //final return data;

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

        if (magicNumber.compare("XDF:") != 0)
        {
            std::cout << "This is not a valid XDF file.('" << filename << "')\n";
            exit(EXIT_FAILURE);
        }

        //for each chunk
        while (1)
        {
            uint64_t ChLen; //Chunk Length

            ChLen = readLength(file);
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

                pugi::xml_document doc;

                pugi::xml_parse_result result = doc.load_buffer_inplace(buffer, ChLen - 2);

                pugi::xml_node info = doc.child("info");

                XDFdata.fileHeader.info.version = info.child("version").text().as_float();

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
                    XDFdata.streams.emplace_back();
                }
                else
                    index = std::distance(idmap.begin(), it);


                pugi::xml_document doc;

                //read [Content]
                char* buffer = new char[ChLen - 6];
                file.read(buffer, ChLen - 6);
                pugi::xml_parse_result result = doc.load_buffer_inplace(buffer, ChLen - 6);

                pugi::xml_node info = doc.child("info");

                XDFdata.streams[index].info.name = info.child("name").text().get();
                XDFdata.streams[index].info.type = info.child("type").text().get();
                XDFdata.streams[index].info.channel_count = info.child("channel_count").text().as_int();
                XDFdata.streams[index].info.nominal_srate = info.child("nominal_srate").text().as_double();
                XDFdata.streams[index].info.channel_format = info.child("channel_format").text().get();
                XDFdata.streams[index].info.source_id = info.child("source_id").text().get();
                XDFdata.streams[index].info.version = info.child("version").text().as_float();
                XDFdata.streams[index].info.created_at = info.child("created_at").text().as_double();
                XDFdata.streams[index].info.uid = info.child("uid").text().get();
                XDFdata.streams[index].info.session_id = info.child("session_id").text().get();
                XDFdata.streams[index].info.hostname = info.child("hostname").text().get();
                XDFdata.streams[index].info.v4address = info.child("v4address").text().get();
                XDFdata.streams[index].info.v4data_port = info.child("v4data_port").text().as_int();
                XDFdata.streams[index].info.v4service_port = info.child("v4service_port").text().as_int();
                XDFdata.streams[index].info.v6address = info.child("v6address").text().get();
                XDFdata.streams[index].info.v6data_port = info.child("v6data_port").text().as_int();
                XDFdata.streams[index].info.v6service_port = info.child("v6service_port").text().as_int();
                //XDFdata.streams[index].info.desc = info.child("desc").text().get();//need further work

                if (XDFdata.streams[index].info.nominal_srate > 0)
                    XDFdata.streams[index].sampling_interval = 1 / XDFdata.streams[index].info.nominal_srate;
                else
                    XDFdata.streams[index].sampling_interval = 0;

                if (options.verbose)
                    std::cout << "Found stream "
                    << XDFdata.streams[index].info.name << "\n";

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
                    XDFdata.streams.emplace_back();
                }
                else
                    index = std::distance(idmap.begin(), it);


                //read [NumSampleBytes], [NumSamples]
                uint64_t numSamp = readLength(file);

                //check the data type
                if (XDFdata.streams[index].info.channel_format.compare("float32") == 0)
                {
                    //if the time series is empty
                    if (XDFdata.streams[index].time_series.empty())
                        XDFdata.streams[index].time_series.resize(XDFdata.streams[index].info.channel_count);

                    //for each sample
                    for (size_t i = 0; i < numSamp; i++)
                    {
                        //read or deduce time stamp
                        uint8_t tsBytes;
                        file.read(reinterpret_cast<char *>(&tsBytes), 1);
                        if (tsBytes == 8)
                        {
                            double ts;
                            file.read((char*)&ts, 8);
                            XDFdata.streams[index].time_stamps.emplace_back(ts);
                        }
                        else
                        {
                            double ts = XDFdata.streams[index].last_timestamp + XDFdata.streams[index].sampling_interval;
                            XDFdata.streams[index].time_stamps.emplace_back(ts);
                        }

                        //read the data
                        for (int v = 0; v < XDFdata.streams[index].info.channel_count; ++v)
                        {
                            float data;
                            file.read((char*)&data, 4);
                            XDFdata.streams[index].time_series[v].emplace_back(data);
                        }
                    }
                }
                else if (XDFdata.streams[index].info.channel_format.compare("double64") == 0)
                {
                    //if the time series is empty
                    if (XDFdata.streams[index].time_series.empty())
                        XDFdata.streams[index].time_series.resize(XDFdata.streams[index].info.channel_count);

                    //for each sample
                    for (size_t i = 0; i < numSamp; i++)
                    {
                        //read or deduce time stamp
                        uint8_t tsBytes;
                        file.read(reinterpret_cast<char *>(&tsBytes), 1);
                        if (tsBytes == 8)
                        {
                            double ts;
                            file.read((char*)&ts, 8);
                            XDFdata.streams[index].time_stamps.emplace_back(ts);
                        }
                        else
                        {
                            double ts = XDFdata.streams[index].last_timestamp + XDFdata.streams[index].sampling_interval;
                            XDFdata.streams[index].time_stamps.emplace_back(ts);
                        }

                        //read the data
                        for (int v = 0; v < XDFdata.streams[index].info.channel_count; ++v)
                        {
                            double data;
                            file.read((char*)&data, 8);
                            XDFdata.streams[index].time_series[v].emplace_back(data);
                        }
                    }
                }
                else if (XDFdata.streams[index].info.channel_format.compare("int8") == 0)
                {
                    //if the time series is empty
                    if (XDFdata.streams[index].time_series.empty())
                        XDFdata.streams[index].time_series.resize(XDFdata.streams[index].info.channel_count);

                    //for each sample
                    for (size_t i = 0; i < numSamp; i++)
                    {
                        //read or deduce time stamp
                        uint8_t tsBytes;
                        file.read(reinterpret_cast<char *>(&tsBytes), 1);
                        if (tsBytes == 8)
                        {
                            double ts;
                            file.read((char*)&ts, 8);
                            XDFdata.streams[index].time_stamps.emplace_back(ts);
                        }
                        else
                        {
                            double ts = XDFdata.streams[index].last_timestamp + XDFdata.streams[index].sampling_interval;
                            XDFdata.streams[index].time_stamps.emplace_back(ts);
                        }

                        //read the data
                        for (int v = 0; v < XDFdata.streams[index].info.channel_count; ++v)
                        {
                            int8_t data;
                            file.read((char*)&data, 1);
                            XDFdata.streams[index].time_series[v].emplace_back(data);
                        }
                    }
                }
                else if (XDFdata.streams[index].info.channel_format.compare("int16") == 0)
                {
                    //if the time series is empty
                    if (XDFdata.streams[index].time_series.empty())
                        XDFdata.streams[index].time_series.resize(XDFdata.streams[index].info.channel_count);

                    //for each sample
                    for (size_t i = 0; i < numSamp; i++)
                    {
                        //read or deduce time stamp
                        uint8_t tsBytes;
                        file.read(reinterpret_cast<char *>(&tsBytes), 1);
                        if (tsBytes == 8)
                        {
                            double ts;
                            file.read((char*)&ts, 8);
                            XDFdata.streams[index].time_stamps.emplace_back(ts);
                        }
                        else
                        {
                            double ts = XDFdata.streams[index].last_timestamp + XDFdata.streams[index].sampling_interval;
                            XDFdata.streams[index].time_stamps.emplace_back(ts);
                        }

                        //read the data
                        for (int v = 0; v < XDFdata.streams[index].info.channel_count; ++v)
                        {
                            int16_t data;
                            file.read((char*)&data, 2);
                            XDFdata.streams[index].time_series[v].emplace_back(data);
                        }
                    }
                }
                else if (XDFdata.streams[index].info.channel_format.compare("int32") == 0)
                {
                    //if the time series is empty
                    if (XDFdata.streams[index].time_series.empty())
                        XDFdata.streams[index].time_series.resize(XDFdata.streams[index].info.channel_count);

                    //for each sample
                    for (size_t i = 0; i < numSamp; i++)
                    {
                        //read or deduce time stamp
                        uint8_t tsBytes;
                        file.read(reinterpret_cast<char *>(&tsBytes), 1);
                        if (tsBytes == 8)
                        {
                            double ts;
                            file.read((char*)&ts, 8);
                            XDFdata.streams[index].time_stamps.emplace_back(ts);
                        }
                        else
                        {
                            double ts = XDFdata.streams[index].last_timestamp + XDFdata.streams[index].sampling_interval;
                            XDFdata.streams[index].time_stamps.emplace_back(ts);
                        }

                        //read the data
                        for (int v = 0; v < XDFdata.streams[index].info.channel_count; ++v)
                        {
                            int32_t data;
                            file.read((char*)&data, 4);
                            XDFdata.streams[index].time_series[v].emplace_back(data);
                        }
                    }
                }
                else if (XDFdata.streams[index].info.channel_format.compare("int64") == 0)
                {
                    //if the time series is empty
                    if (XDFdata.streams[index].time_series.empty())
                        XDFdata.streams[index].time_series.resize(XDFdata.streams[index].info.channel_count);

                    //for each sample
                    for (size_t i = 0; i < numSamp; i++)
                    {
                        //read or deduce time stamp
                        uint8_t tsBytes;
                        file.read(reinterpret_cast<char *>(&tsBytes), 1);
                        if (tsBytes == 8)
                        {
                            double ts;
                            file.read((char*)&ts, 8);
                            XDFdata.streams[index].time_stamps.emplace_back(ts);
                        }
                        else
                        {
                            double ts = XDFdata.streams[index].last_timestamp + XDFdata.streams[index].sampling_interval;
                            XDFdata.streams[index].time_stamps.emplace_back(ts);
                        }

                        //read the data
                        for (int v = 0; v < XDFdata.streams[index].info.channel_count; ++v)
                        {
                            int64_t data;
                            file.read((char*)&data, 8);
                            XDFdata.streams[index].time_series[v].emplace_back(data);
                        }
                    }
                }
                else if (XDFdata.streams[index].info.channel_format.compare("string") == 0)
                {
                    //for each event
                    for (size_t i = 0; i < numSamp; i++)
                    {
                        //read or deduce time stamp
                        uint8_t tsBytes;
                        file.read(reinterpret_cast<char *>(&tsBytes), 1);
                        if (tsBytes == 8)
                        {
                            double ts;
                            file.read((char*)&ts, 8);
                            XDFdata.streams[index].time_stamps.emplace_back(ts);
                        }
                        else
                        {
                            double ts = XDFdata.streams[index].last_timestamp + XDFdata.streams[index].sampling_interval;
                            XDFdata.streams[index].time_stamps.emplace_back(ts);
                        }

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
                            XDFdata.eventMap.emplace_back(buffer, XDFdata.streams[index].time_stamps.back());
                            delete[] buffer;
                        }
                        else if (bytes == 4)
                        {
                            uint32_t length;
                            file.read((char*)&length, 4);
                            char* buffer = new char[length + 1];
                            file.read(buffer, length);
                            buffer[length] = '\0';
                            XDFdata.eventMap.emplace_back(buffer, XDFdata.streams[index].time_stamps.back());
                            delete[] buffer;
                        }
                        else if (bytes == 8)
                        {
                            uint64_t length;
                            file.read((char*)&length, 8);
                            char* buffer = new char[length + 1];
                            file.read(buffer, length);
                            buffer[length] = '\0';
                            XDFdata.eventMap.emplace_back(buffer, XDFdata.streams[index].time_stamps.back());
                            delete[] buffer;
                        }
                    }

                    XDFdata.streams[index].last_timestamp = XDFdata.streams[index].time_stamps.back();

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
                    XDFdata.streams.emplace_back();
                }
                else
                    index = std::distance(idmap.begin(), it);


                double collectionTime;
                double offsetValue;

                file.read((char*)&collectionTime, 8);
                file.read((char*)&offsetValue, 8);

                XDFdata.streams[index].clock_times.emplace_back(collectionTime);
                XDFdata.streams[index].clock_values.emplace_back(offsetValue);
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
                    XDFdata.streams.emplace_back();
                }
                else
                    index = std::distance(idmap.begin(), it);


                char* buffer = new char[ChLen - 6];
                file.read(buffer, ChLen - 6);
                pugi::xml_parse_result result = doc.load_buffer_inplace(buffer, ChLen - 6);

                pugi::xml_node info = doc.child("info");

                XDFdata.streams[index].info.first_timestamp = info.child("first_timestamp").text().as_double();
                XDFdata.streams[index].info.last_timestamp = info.child("last_timestamp").text().as_double();
                XDFdata.streams[index].info.measured_srate = info.child("measured_srate").text().as_double();
                XDFdata.streams[index].info.sample_count = info.child("sample_count").text().as_int();
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


        /*
        // ====================================================================
        // === perform(fault - tolerant) clock synchronization if requested ===
        // ====================================================================

        if (options.handleClockSynchronization)
        {
        if (options.verbose)
        {
        std::cout << "Performing clock synchronization...\n";
        }
        for (size_t i = 0; i < temps.size(); i++)
        {
        if (!temps[i].time_stamps.empty())
        {
        std::vector<long double> clock_times;
        std::vector<long double> clock_values;

        try
        {
        std::vector<double> clock_times = temps[i].clock_times;
        std::vector<double> clock_values = temps[i].clock_values;

        }
        catch (const std::exception&)
        {
        std::cout << "No clock offsets were available for stream "
        << XDFdata.streams[i].info.name << std::endl;
        }

        if (options.handleClockResets)
        {
        std::vector<double> time_diff;
        for (size_t a = 0; a < clock_times.size(); a++)
        {
        time_diff.push_back(clock_times[a + 1] - clock_times[a]);
        }
        std::vector<double> value_diff;
        for (size_t b = 0; b < clock_values.size(); b++)
        {
        value_diff.push_back(abs(clock_values[b + 1] - clock_values[b]));
        }
        std::vector<bool> time_glitch;
        for each (auto var in time_diff)
        {
        if (var >= 0)
        time_glitch.push_back(0);
        else
        time_glitch.push_back(1);
        }
        //omitted the complicated algorithm
        std::vector<bool> value_glitch;
        double median{ getMedian(value_diff) };
        double mad{ getMad(value_diff) }; //MEDIAN absolute deviation
        for (auto item : value_diff)
        {
        if (item - median / mad > options.clockResetThresholdOffsetStds && item - median > options.clockResetThresholdOffsetSeconds)
        value_glitch.push_back(true);
        else
        value_glitch.push_back(false);
        }
        std::vector<bool> resets_at;
        for (size_t i = 0; i < time_glitch.size(); i++)
        {
        if (time_glitch[i] == true && value_glitch[i] == true)
        resets_at.push_back(true);
        else
        resets_at.push_back(false);
        }

        std::vector<bool> tmp;
        for (auto item : resets_at)
        {
        if (item == true)
        {
        //?????????????????
        }
        }

        }
        }


        }
        }

        //===========================================
        //===perform jitter removal if requested  ===
        //===========================================

        // jitter removal is a bonus feature that yields linearly increasing timestamps from data
        // where samples had been time stamped with some jitter(e.g., due to operating system
        // delays)

        if (options.handleJitterRemoval)
        {
        if (options.verbose)
        {
        std::cout << "Performing jitter removal...\n";
        }
        for (size_t i = 0; i < temps.size(); i++)
        {
        if (temps[i].time_stamps.empty() && temps[i].srate)
        {
        std::vector<double> diffs;
        for (size_t j = 0; j < temps[i].time_stamps.size(); j++)
        {
        double diff{ temps[i].time_stamps[j + 1] - temps[i].time_stamps[j] };
        diffs.push_back(diff);
        }
        double max{ std::max(options.jitterBreakThresholdSeconds, options.jitterBreakThresholdSamples*temps[i].sampling_interval) };
        std::vector<double> breaks_at;
        for (size_t i = 0; i < diffs.size(); i++)
        {
        if (diffs[i]>max)
        {
        breaks_at.push_back(1);
        }
        else
        {
        breaks_at.push_back(0);
        }
        }
        for (size_t i = 0; i < breaks_at.size(); i++)
        {
        //????????
        }
        }

        //process each segment separately
        Segment_Stream segment;
        }
        }*/
/*
        //calculate effective sampling rate
        for (size_t k = 0; k < temps.size(); k++)
        {
            temps[k].effective_srate = temps[k].time_stamps.size() / (temps[k].time_stamps.back() - temps[k].time_stamps[0]);
            XDFdata.streams[k].segment.effective_srate = temps[k].effective_srate;
        }*/


        //copy the information into the output

/*		for (size_t i = 0; i < temps.size(); i++)
        {
            XDFdata.streams[i].time_series = temps[i].time_series;
            XDFdata.streams[i].time_seriesStr = temps[i].time_seriesStr;
            XDFdata.streams[i].time_stamps = temps[i].time_stamps;
        }*/


        //calculate how much time it takes to read the data
        clock_t halfWay = clock() - time;

        std::cout << "it took " << halfWay << " clicks (" << ((float)halfWay) / CLOCKS_PER_SEC << " seconds)"
                  << " reading XDF data" << std::endl;

        //try to free up some memory
        for (size_t st = 0; st < XDFdata.streams.size(); st++)
        {
            XDFdata.streams[st].time_series.shrink_to_fit();
            for (size_t ch = 0; ch < XDFdata.streams[st].time_series.size(); ch++)
            {
                XDFdata.streams[st].time_series[ch].shrink_to_fit();
            }
        }



        //==========================================================
        //=============find the min and max time stamps=============
        //==========================================================

        //find the smallest timestamp of all streams
        for (size_t i = 0; i < XDFdata.streams.size(); i++)
        {
            if (!XDFdata.streams[i].time_stamps.empty())
            {
                XDFdata.minTS = XDFdata.streams[i].time_stamps.front();
                break;
            }
        }
        for (size_t i = 0; i < XDFdata.streams.size(); i++)
        {
            if (!XDFdata.streams[i].time_stamps.empty() &&
                    XDFdata.streams[i].time_stamps.front() < XDFdata.minTS)
                XDFdata.minTS = XDFdata.streams[i].time_stamps.front();
        }

        //find the max timestamp of all streams
        for (size_t i = 0; i < XDFdata.streams.size(); i++)
        {
            if (!XDFdata.streams[i].time_stamps.empty() &&
                    XDFdata.streams[i].time_stamps.back() > XDFdata.maxTS)
                XDFdata.maxTS = XDFdata.streams[i].time_stamps.back();
        }


        //=============================================================================
        //===resampling all channels to the sample rate that has the most channels=====
        //=============================================================================

        //first find out which sample rate has the most channels
        typedef int sampRate;
        typedef int numChannel;

        std::vector<std::pair<sampRate, numChannel> > srateMap; //<srate, numChannels> pairs of all the streams

        //find out whether a sample rate already exists in srateMap
        for (size_t st = 0; st < XDFdata.streams.size(); st++)
        {
            if (XDFdata.streams[st].info.nominal_srate!=0)
            {
                std::vector<std::pair<sampRate, numChannel> >::iterator it {std::find_if(srateMap.begin(), srateMap.end(),
                                      [&](const std::pair<sampRate, numChannel> &element)
                                            {return element.first == XDFdata.streams[st].info.nominal_srate; })} ;
                //if it doesn't, add it here
                if (it == srateMap.end())
                    srateMap.emplace_back(XDFdata.streams[st].info.nominal_srate, XDFdata.streams[st].info.channel_count);
                //if it already exists, add additional channel numbers to that sample rate
                else
                {
                    int index {std::distance(srateMap.begin(),it)} ;
                    srateMap[index].second += XDFdata.streams[st].info.channel_count;
                }
            }
        }

        //search the srateMap to see which sample rate has the most channels
        int index {std::distance(srateMap.begin(),
                                 std::max_element(srateMap.begin(),srateMap.end(),
                                                [] (const std::pair<sampRate, numChannel> &largest,
                                                const std::pair<sampRate, numChannel> &first)
                                                { return largest.second < first.second; }))};
        XDFdata.majSR = srateMap[index].first; //the sample rate that has the most channels



        //resampling starts, using smarc resampling library "smarc.c"
#define BUF_SIZE 8192
        for (size_t x = 0; x< XDFdata.streams.size(); x++)
        {
            if (!XDFdata.streams[x].time_series.empty() &&
                     XDFdata.streams[x].info.nominal_srate != XDFdata.majSR &&
                     XDFdata.streams[x].info.nominal_srate != 0)
            {
                int fsin = XDFdata.streams[x].info.nominal_srate;       // input samplerate
                int fsout = XDFdata.majSR;                              // output samplerate
                double bandwidth = 0.95;                                // bandwidth
                double rp = 0.1;                                        // passband ripple factor
                double rs = 140;                                        // stopband attenuation
                double tol = 0.000001;                                  // tolerance

                // initialize smarc filter
                struct PFilter* pfilt = smarc_init_pfilter(fsin, fsout, bandwidth, rp,
                    rs, tol, NULL, 0);
                if (pfilt == NULL)
                    continue;

                // initialize smarc filter state
                struct PState* pstate = smarc_init_pstate(pfilt);


                for (size_t w = 0; w < XDFdata.streams[x].time_series.size(); w++)
                {
                    // initialize buffers
                    int read = 0;
                    int written = 0;
                    const int IN_BUF_SIZE = BUF_SIZE;
                    const int OUT_BUF_SIZE = (int) smarc_get_output_buffer_size(pfilt,XDFdata.streams[x].time_series[w].size());
                    double* inbuf = new double[XDFdata.streams[x].time_series[w].size()];
                    double* outbuf = new double[OUT_BUF_SIZE];


                    std::copy(XDFdata.streams[x].time_series[w].begin(),
                              XDFdata.streams[x].time_series[w].end(),
                              inbuf);

                    read = XDFdata.streams[x].time_series[w].size();


                    // resample signal block
                    written = smarc_resample(pfilt, pstate, inbuf, read, outbuf,
                            OUT_BUF_SIZE);

                    // do what you want with your output
                    XDFdata.streams[x].time_series[w].resize(written);
                    std::copy ( outbuf, outbuf+written, XDFdata.streams[x].time_series[w].begin() );

                    // flushing last values
                    written = smarc_resample_flush(pfilt, pstate, outbuf,
                            OUT_BUF_SIZE);

                    // do what you want with your output
                    XDFdata.streams[x].time_series[w].resize(XDFdata.streams[x].time_series[w].size()+written);
                    std::copy ( outbuf, outbuf+written,
                                XDFdata.streams[x].time_series[w].begin()
                                + XDFdata.streams[x].time_series[w].size() - written );


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

               XDFdata.streams[x].info.nominal_srate = XDFdata.majSR;
            }
        }
        //resampling finishes here


        //======================================================================
        //===========Calculating total length & total channel count=============
        //======================================================================


        //calculating total channel count, and indexing them onto streamMap
        for (size_t c = 0; c < XDFdata.streams.size(); c++)
        {
            if(!XDFdata.streams[c].time_series.empty())
            {
                XDFdata.totalCh += XDFdata.streams[c].info.channel_count;
                if (XDFdata.streamMap.empty())
                    XDFdata.streamMap.emplace_back(c,XDFdata.streams[c].info.channel_count);
                else
                    XDFdata.streamMap.emplace_back(c,XDFdata.streamMap.back().second + XDFdata.streams[c].info.channel_count);
            }
        }

        //global length
        XDFdata.totalLen = (XDFdata.maxTS - XDFdata.minTS) * XDFdata.majSR;

        //beware of possible deviation
        for (size_t st = 0; st < XDFdata.streams.size(); st++)
        {
            if(!XDFdata.streams[st].time_series.empty())
            {
                if (XDFdata.totalLen<XDFdata.streams[st].time_series.front().size())
                    XDFdata.totalLen = XDFdata.streams[st].time_series.front().size();
            }
        }

        //free up as much memory as possible
        for (size_t st = 0; st < XDFdata.streams.size(); st++)
        {
            //we don't need to keep all the time stamps unless it's a stream with irregular samples
            if (XDFdata.streams[st].info.nominal_srate != 0)
            {
                std::vector<float> nothing;
                //however we still need to keep the first time stamp of each stream to decide at which position the signal should start
                nothing.emplace_back(XDFdata.streams[st].time_stamps.front());
                XDFdata.streams[st].time_stamps.swap(nothing);
            }
            //else if this is a stream containing only strings
            else if (XDFdata.streams[st].info.nominal_srate == 0 &&
                     XDFdata.streams[st].time_series.empty() &&
                     !XDFdata.streams[st].time_stamps.empty())
            {
                //we don't need these anymore because string timestamps are duplicated in eventMap
                std::vector<float> nothing;
                XDFdata.streams[st].time_stamps.swap(nothing);
            }
        }

        time = clock() - time;

        std::cout << "it took " << time << " clicks (" << ((float)time) / CLOCKS_PER_SEC << " seconds)"
                  << " reading and resampling in total" << std::endl;


        //loading finishes, close file and return XDFdata
        file.close();

        return XDFdata;

    }
    else
    {
        std::cout << "Unable to open file";
        exit(EXIT_FAILURE);
    }
}

