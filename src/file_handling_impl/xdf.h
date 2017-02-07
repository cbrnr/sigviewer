/*! \file xdf.h
 * \brief The header file of Xdf class
 */

#ifndef XDF_H
#define XDF_H

#include <string>
#include <vector>
#include <map>

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
        std::vector<float> time_stamps; /*!< A vector to store time stamps. */
        std::string streamHeader;   /*!< Raw XDF of streamHeader. */

        struct
        {
            int channel_count;      /*!< Number of channels of the current stream */
            double nominal_srate;   /*!< The nominal sample rate of the current stream. */
            std::map<std::string, std::string> infoMap; /*!< This map stores all direct children of the _info_ section. */

            struct {
                //! The description of one channel, repeated (one for each channel in the time series)
                struct Channel
                {
                    std::map<std::string, std::string> channelInfoMap; /*!< The top-level meta-data of a single channel. */
                    /*!
                     * \brief  Measured location (note: this may be arbitrary
                     * but should then include well-known fiducials
                     * (landmarks) for co-registration).
                     */
                    std::map<std::string, std::string> location;
                    std::map<std::string, std::string> hardware;/*!< Information about hardware properties. */
                    struct {
                        std::map<std::string, std::string> highpass;    /*!< Highpass filter, if any. */
                        std::map<std::string, std::string> lowpass;     /*!< Lowpass filter, if any. */
                        std::map<std::string, std::string> notch;       /*!< Notch filter, if any. */
                    } filtering;/*!< Information about the hardware/software filters already applied to the data (e.g. notch). */
                };

                std::vector<Channel> channels;  /*!< A vector to store the meta-data of the channels of the current stream. */

                std::map<std::string, std::string> reference;       /*!< Signal referencing scheme. */
                std::map<std::string, std::string> cap;             /*!< EEG cap description. */
                std::map<std::string, std::string> location_measurement;/*!< Information about the sensor localization system/method. */
                std::map<std::string, std::string> acquisition;     /*!< Information about the acquisition system. */
                std::map<std::string, std::string> acquisitionDistortion;/*!< Distortion parameters of the camera used (using Brown's distortion model). */
                std::map<std::string, std::string> acquisitionSetting;/*!< Settings of the acquisition system. */
                std::map<std::string, std::string> provider;        /*!< Information about the audio provider. */
                std::map<std::string, std::string> facility;        /*!< Information about the recording facility (i.e. lab). */
                std::map<std::string, std::string> synchronization; /*!< Information about synchronization requirements. */
                std::map<std::string, std::string> encoding;        /*!< Specification of how each sample (frame) is encoded. */
                std::map<std::string, std::string> display;         /*!< Specification of how the images shall be displayed. */
                std::map<std::string, std::string> content;         /*!< Information about the video content. */
                std::map<std::string, std::string> filtering;       /*!< Filtering applied to the data. */

                struct Camera /*!< Information about a single camera (repeated for each camera). */
                {
                    std::map<std::string, std::string> cameraInfo;
                    std::map<std::string, std::string> position;    /*!< 3-d position of the camera, in meters (arbitrary coordinate system). */
                    std::map<std::string, std::string> orientation; /*!< Orientation of the camera, specified as a quaternion. */
                    std::map<std::string, std::string> settings;    /*!< Settings of the camera. */
                };

                struct {
                    bool initialized = false;
                    std::string name; /*!< Name of the setup. */
                    std::vector<std::map<std::string, std::string> > objects; /*!< Information about objects (e.g., rigid bodies). */
                    std::vector<std::map<std::string, std::string> > markers; /*!< Information about point markers in the setup. */
                    std::map<std::string, std::map<std::string, std::string> > bounds; /*!< Bounding box of the space/room (in the same coordinate system as all others). */
                    std::vector<Camera> cameras;    /*!< Camera setup. */
                    std::string camerasModel;       /*!< Camera model. */
                } setup; /*!< Information about the physical setup (e.g. room layout). */

                struct {
                    std::map<std::string, std::string> subjectInfo;/*!< All direct children of _subject_. \sa subject*/
                    std::map<std::string, std::string> medication;/*!< General information on medication and other substance effects. */
                } subject; /*!< Information about the human subject. */

                /*!
                 * \brief Information about a single fiducial, can be repeated.
                 */
                struct Fiducials
                {
                    std::string label;/*!< Label of the location (e.g., Nasion, Inion, LPF, RPF); can also cover Ground and Reference.*/
                    std::map<std::string, std::string> location; /*!< Measured location (same coordinate system as channel locations). */
                };

                std::vector<Fiducials> fiducials;/*!< Locations of fiducials (in the same space as the signal-carrying channels).*/

                struct {
                    std::map<std::string, std::string> settings;/*!< Settings. */
                } amplifier; /*!< Information about the used amplification (e.g. Gain, OpAmps/InAmps...). */
            } desc; /*!< Member struct to store descriptive information in addition to the infoMap. */

            //! A vector to store clock offsets from the ClockOffset chunk
            std::vector<std::pair<double, double> > clock_offsets;

            double first_timestamp; /*!< First time stamp of the stream. */
            double last_timestamp;  /*!< Last time stamp of the stream. */
            int sample_count;       /*!< Sample count of the stream. */
            double measured_srate;  /*!< Measured sample rate of the stream. */
        } info; /*!< Meta-data from the stream header of the current stream. */

        float last_timestamp{ 0 };      /*!< For temporary use while loading the vector */
        float sampling_interval;        /*!< If srate > 0, sampling_interval = 1/srate; otherwise 0 */
        std::vector<double> clock_times;/*!< Vector of clock times from clock offset chunk (Tag 4). */
        std::vector<double> clock_values;/*!< Vector of clock values from clock offset chunk (Tag 4). */
    };

    //===============================================================================================

    std::vector<Stream> streams; /*!< A vector to store all the streams of an XDF file. */
    struct
    {
        struct
        {
            float version;  /*!< The version of XDF file */
        }info;              /*!< Sub-struct of file header. Not to be confused with the _info_ section in stream header. */
    } fileHeader;           /*!< Member struct to store file header.
                             * \sa version */

    uint64_t totalLen = 0;  /*!< The total length is calculated from the smallest time stamp
                             *to the largest multiplies the major sample rate. */

    float minTS = 0;        /*!< The smallest time stamp across all streams. */
    float maxTS = 0;        /*!< The largest time stamp across all streams. */
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
    typedef float eventTimeStamp;

    /*! The vector to store all the events across all streams.
     * The format is <<events, timestamps>, streamNum>.
     */
    std::vector<std::pair<std::pair<eventName, eventTimeStamp>, int> > eventMap;

    std::vector<std::string> dictionary;/*!< The vector to store unique event types with no repetitions. \sa eventMap */
    std::vector<uint16_t> eventType;    /*!< The vector to store events by their index in the dictionary.\sa dictionary, eventMap */
    std::vector<std::string> labels;    /*!< The vector to store descriptive labels of each channel. */
    std::vector<double> sampleRateMap;  /*!< The vector to store all sample rates across all the streams. */
    std::vector<float> offsets;         /*!< Offsets of each channel after using subtractMean() function */

    std::string testFileHeader;         /*!< Raw XML of the file header. */

    //=============================================================================================

    /*!
     * \brief Create labels for each channel and store them in _labels_ vector.
     * \sa labels, offsets
     */
    void createLabels();

    /*!
     * \brief Copy all unique types of events from _eventMap_ to
     * _dictionary_ with no repeats.
     * \sa dictionary, eventMap
     */
    void loadDictionary();

    /*!
     * \brief The main function of loading an XDF file.
     * \param filename is the path to the file being loaded including the
     * file name.
     */
    void load_xdf(std::string filename);

    /*!
     * \brief Resample all streams and channel to a chosen sample rate
     * \param userSrate is recommended to be between integer 1 and
     * the highest sample rate of the current file.
     */
    void resampleXDF(int userSrate);

    //! This function will get the length of the upcoming chunk, or the number of samples.
    /*!
     * \brief While loading XDF file there are 2 cases where this function will be
     * needed. One is to get the length of each chunk, one is to get the
     * number of samples when loading the time series (Chunk tag 3).
     * \param file is the XDF file that is being loaded in the type of `std::ifstream`.
     * \return The length of the upcoming chunk (in bytes).
     */
    uint64_t readLength(std::ifstream &file);

    /*!
     * \brief Find the minimal and maximal time stamps across all streams.
     *
     * The results will be stored in member variables `minTS` and `maxTS` respectively.
     * \sa  minTS, maxTS, calcTotalLength(int sampleRate);
     */
    void findMinMax();

    //! Find the sample rate that has the most channels.
    /*!
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
     * \sa majSR, resampleXDF(int userSrate)
     */
    void findMajSR();

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
     * \brief Get the highest sample rate of all streams and store
     * it in `maxSR`.
     *
     * \sa maxSR
     */
    void getHighestSampleRate();

    /*!
     * \brief Load every sample rate appeared in the current file into
     * member variable `sampleRateMap`.
     * \sa sampleRateMap
     */
    void loadSampleRateMap();

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
    void subtractMean();
};

#endif // XDF_H
