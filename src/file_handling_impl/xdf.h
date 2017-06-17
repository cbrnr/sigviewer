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

/*! \file xdf.h
 * \brief The header file of Xdf class
 */

#ifndef XDF_H
#define XDF_H

#include <string>
#include <vector>
#include <map>
#include <set>

/*! \class Xdf
 *
 * Xdf class is designed to store the data of an entire XDF file.
 * It comes with methods to read XDF files and containers to store
 * the data, as well as some additional methods e.g. resampling etc.
 */

class Xdf
{  
public:
    //! Default constructor with no parameter.
    Xdf();

    //subclass for single streams
    /*! \class Stream
     *
     * XDF files uses stream as the unit to store data. An XDF file usually
     * contains multiple streams, while each of them may contain one or more
     * channels.
     * The Stream class provides a handy way to store all the meta-data,
     * time-series, time-stamps and all other information of a single stream
     * from an XDF file.
     */
    struct Stream
    {
        //! A 2D vector which stores the time series of a stream. Each row represents a channel.
        std::vector<std::vector<float> > time_series;
        std::vector<double> time_stamps; /*!< A vector to store time stamps. */
        std::string streamHeader;   /*!< Raw XDF of stream header chunk. */
        std::string streamFooter;   /*!< Raw XDF of stream footer chunk. */

        struct
        {
            int channel_count;      /*!< Number of channels of the current stream */
            double nominal_srate;   /*!< The nominal sample rate of the current stream. */
            std::string name;       /*!< Name of the current stream. */
            std::string type;       /*!< Type of the current stream. */
            std::string channel_format;/*!< Channel format of the current stream. */

            std::vector<std::map<std::string, std::string> > channels;/*!< A vector to store the meta-data of the channels of the current stream. */

            std::vector<std::pair<double, double> > clock_offsets;  /*!< A vector to store clock offsets from the ClockOffset chunk. */

            double first_timestamp; /*!< First time stamp of the stream. */
            double last_timestamp;  /*!< Last time stamp of the stream. */
            int sample_count;       /*!< Sample count of the stream. */
            double measured_srate;  /*!< Measured sample rate of the stream. */
            double effective_sample_rate = 0;/*!< Effective sample rate. */
        } info; /*!< Meta-data from the stream header of the current stream. */

        double last_timestamp{ 0 };  /*!< For temporary use while loading the vector */
        double sampling_interval;    /*!< If srate > 0, sampling_interval = 1/srate; otherwise 0 */
        std::vector<double> clock_times;/*!< Vector of clock times from clock offset chunk (Tag 4). */
        std::vector<double> clock_values;/*!< Vector of clock values from clock offset chunk (Tag 4). */
    };

    //XDF properties=================================================================================

    std::vector<Stream> streams; /*!< A vector to store all the streams of the current XDF file. */
    float version;  /*!< The version of XDF file */

    uint64_t totalLen = 0;  /*!< The total length is the product of the range between the smallest
                             *time stamp and the largest multiplied by the major sample rate. */

    double minTS = 0;        /*!< The smallest time stamp across all streams. */
    double maxTS = 0;        /*!< The largest time stamp across all streams. */
    size_t totalCh = 0;     /*!< The total number of channel count. */
    int majSR = 0;          /*!< The sample rate that has the most channels across all streams. */
    int maxSR = 0;          /*!< Highest sample rate across all streams. */
    std::vector<int> streamMap;/*!< A vector indexes which channels belong to which stream.
                                * The index is the same as channel number; the actual content is the stream Number */

    /*!
     * \brief Used as `std::vector<std::pair<std::pair<eventName, eventTimeStamp>, int> >`
     * in eventMap.
     * \sa eventMap
     */
    typedef std::string eventName;
    /*!
     * \brief Used as `std::vector<std::pair<std::pair<eventName, eventTimeStamp>, int> >`
     * in eventMap.
     * \sa eventMap
     */
    typedef double eventTimeStamp;

    std::vector<std::pair<std::pair<eventName, eventTimeStamp>, int> > eventMap;/*!< The vector to store all the events across all streams.
                                                                                 * The format is <<events, timestamps>, streamNum>. */
    std::vector<std::string> dictionary;/*!< The vector to store unique event types with no repetitions. \sa eventMap */
    std::vector<uint16_t> eventType;    /*!< The vector to store events by their index in the dictionary.\sa dictionary, eventMap */
    std::vector<std::string> labels;    /*!< The vector to store descriptive labels of each channel. */
    std::set<double> sampleRateMap;  /*!< The vector to store all sample rates across all the streams. */
    std::vector<float> offsets;         /*!< Offsets of each channel after using subtractMean() function */

    std::string fileHeader;             /*!< Raw XML of the file header. */
    int userAddedStream { 0 };            /*!< For Sigviewer only: if user manually added events in Sigviewer,
                                          * the events will be stored in a new stream after all current streams.
                                          * The index will be userAddedStream.  */
    std::vector<std::pair<std::string, double> > userCreatedEvents;/*!< User created events in Sigviewer. */

    //Public Functions==============================================================================================

    /*!
     * \brief Adjust `totalLen` to avoid possible deviation
     *
     * `totalLen` is calculated by multiplying the difference between max time
     * stamp and minimal time stamp by the `majSR` (major sample rate).
     * However, this can be inaccurate. In case any channel is longer than
     * `totalLen`, this function will make `totalLen` match the length of
     * that channel.
     *
     * \sa totalLen, majSR, calcTotalLength()
     */
    void adjustTotalLength();

    /*!
     * \brief Calculate the globle length (in samples).
     *
     * This is calculated by multiplying the rage from the earliest
     * time stamp to the last time stamp across all channels by the
     * parameter sampleRate.
     *
     * \param sampleRate is the sample rate you wish to use to calculate the
     * total length.
     */
    void calcTotalLength(int sampleRate);

    /*!
     * \brief Create labels for each channel and store them in _labels_ vector.
     * \sa labels, offsets
     */
    void createLabels();

    /*!
     * \brief Subtract the entire channel by its mean.
     *
     * Sigviewer displays both the channel signals as well as the zero baseline.
     * Thus when the mean of a channel is too high or too low it would be very
     * hard to see the fluctuation. Subtract the entire channel by its mean
     * will make the signal fluctuate around the zero baseline, and has better
     * visual effect. The mean of each channel times `-1` will be stored in
     * member vector `offsets`
     *
     * \sa offsets
     */
    void detrend();

    /*!
     * \brief Delete the time stamps vectors when no longer needed to
     * release some memory.
     *
     * Sigviewer doesn't demand time stamps to display signals except
     * irregular sample rate channels, events, and the first time stamp
     * of each channel (used to decide where does a channel start when
     * putting all streams together). In this case we can delete the time
     * stamps when no longer needed to free up some memory.
     */
    void freeUpTimeStamps();

    /*!
     * \brief The main function of loading an XDF file.
     * \param filename is the path to the file being loaded including the
     * file name.
     */
    int load_xdf(std::string filename);

    /*!
     * \brief Resample all streams and channel to a chosen sample rate
     * \param userSrate is recommended to be between integer 1 and
     * the highest sample rate of the current file.
     */
    void resample(int userSrate);

    /*!
     * \brief writeEventsToXDF
     *
     * If user added some markups and events in Sigviewer, this function can
     * store those user created events back to the XDF file in a new stream
     */
    int writeEventsToXDF(std::string file_path);

    //Private Functions: Not intended to be used by external programs======================================

private:

    /*!
     * \brief calcEffectiveSrate
     */
    void calcEffectiveSrate();

    /*!
     * \brief Calculate the total channel count and store the result
     * in `totalCh`.
     *
     * Channels of both regular and irregular sample rates are included.
     * The streams with the channel format `string` are excluded, and are
     * stored in `eventMap` instead.
     *
     * \sa totalCh, eventMap
     */
    void calcTotalChannel();

    /*!
     * \brief Find the sample rate that has the most channels.
     *
     * XDF format supports different sample rates across streams, but
     * Sigviewer needs to display all channels in a unified sample rate.
     * Thus if there are more than one sample rate in the file, some channels
     * need to be resampled in order to be displayed.
     *
     * Libxdf uses third party _smarc_ library to do the resampling task.
     * While _smarc_ library is powerful in the sense that it can resample
     * signals to almost any sample rate, it's fairly slow, and for performance
     * reason it's better to minimize the resampling process.
     *
     * findMajSR() will find which sample rate currently has the most channels
     * in the file. If only those channels with a different sample rate are to
     * be resampled, the resampling process will be finished in the shortest
     * possible period.
     *
     * \sa majSR, resample(int userSrate)
     */
    void findMajSR();

    /*!
     * \brief Find the minimal and maximal time stamps across all streams.
     *
     * The results will be stored in member variables `minTS` and `maxTS` respectively.
     * \sa  minTS, maxTS, calcTotalLength(int sampleRate);
     */
    void findMinMax();

    /*!
     * \brief Get the highest sample rate of all streams and store
     * it in `maxSR`.
     *
     * \sa maxSR
     */
    void getHighestSampleRate();

    /*!
     * \brief Copy all unique types of events from _eventMap_ to
     * _dictionary_ with no repeats.
     * \sa dictionary, eventMap
     */
    void loadDictionary();

    /*!
     * \brief Load every sample rate appeared in the current file into
     * member variable `sampleRateMap`.
     * \sa sampleRateMap
     */
    void loadSampleRateMap();

    /*!
     * \brief This function will get the length of the upcoming chunk, or the number of samples.
     *
     * While loading XDF file there are 2 cases where this function will be
     * needed. One is to get the length of each chunk, one is to get the
     * number of samples when loading the time series (Chunk tag 3).
     * \param file is the XDF file that is being loaded in the type of `std::ifstream`.
     * \return The length of the upcoming chunk (in bytes).
     */
    uint64_t readLength(std::ifstream &file);
};

#endif // XDF_H
