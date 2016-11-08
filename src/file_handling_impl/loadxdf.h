
#ifndef LOADXDF
#define LOADXDF

//#include <iostream>
//#include <fstream>
#include <string>
//#include <stdlib.h>
//#include <stdio.h>
//#include <cmath>
#include <vector>
//#include "pugixml.hpp" //pugi XML parser
//#include <sstream>
//#include <algorithm>


//options
class Opts
{
public:
    bool onChunk;
    bool verbose;
    bool handleClockSynchronization;
    bool handleClockResets;
    bool handleJitterRemoval;
    double jitterBreakThresholdSeconds;
    int jitterBreakThresholdSamples;
    int clockResetThresholdSeconds;
    int clockResetThresholdStds;
    int clockResetThresholdOffsetSeconds;
    int clockResetThresholdOffsetStds;
    double winsorThreshold;
    int clockResetMaxJitter;
    bool correctStreamLags;
    double frameRateAccuracy;

    Opts() : onChunk { false },
    verbose{ false },
    handleClockSynchronization{ true },
    handleClockResets{ true },
    handleJitterRemoval{ true },
    jitterBreakThresholdSeconds{ 1 },
    jitterBreakThresholdSamples{ 500 },
    clockResetThresholdSeconds{ 5 },
    clockResetThresholdStds{ 5 },
    clockResetThresholdOffsetSeconds{ 1 },
    clockResetThresholdOffsetStds{ 10 },
    winsorThreshold{ 0.0001 },//???
    clockResetMaxJitter{ 5 }, //double check??
    correctStreamLags{ true },
    frameRateAccuracy{ 0.05 }
    {
    }
    ~Opts()
    {

    }
    //OnChunk onChunkFunc(OnChunk input);  //need help
};

struct Stream
{
    std::vector<std::vector<float> > time_series;
    std::vector<std::vector<std::string> > time_seriesStr;
    std::vector<float> time_stamps;
    struct
    {
        std::string name;
        std::string type;
        int channel_count;
        double nominal_srate;
        std::string channel_format;
        std::string source_id;
        float version;
        long double created_at;
        std::string uid;
        std::string session_id;
        std::string hostname;
        std::string v4address;
        int v4data_port;
        int v4service_port;
        std::string v6address;
        int v6data_port;
        int v6service_port;
        std::string desc;
        std::vector<std::pair<double, double> > clock_offsets;
        double first_timestamp;
        double last_timestamp;
        int sample_count;
        double measured_srate;
    } info;
    struct
    {
        int num_samples;
        std::array<int, 2> index_range;
        double t_begin;
        double t_end;
        double duration;
        double effective_srate;
    } segment;
    float last_timestamp{ 0 };   //for temporary use
    float sampling_interval;//if srate>0,sampling_interval=1/srate;otherwise 0
    std::vector<double> clock_times;
    std::vector<double> clock_values;
};


struct XDFdataStruct	//final return value
{
    std::vector<Stream> streams;
    struct
    {
        struct
        {
            float version;
        }info;
    } fileHeader;
    uint64_t totalLen;
    float minTS{0};
    float maxTS{0};
    int totalCh = 0;
    //int maxSR {0};  //max sample rate;
    int majSR;      //the sample rate that has the most channels;
    //std::vector<float> generalTS;
    std::vector<std::pair<int, unsigned> > streamMap; //streamNum, channel count

    typedef std::string eventName;
    typedef float eventTimeStamp;

    std::vector<std::pair<eventName, eventTimeStamp> > eventMap; //copy all the events of all streams to here <events, timestamps>
};


XDFdataStruct load_xdf(std::string filename);



#endif // !LOADXDF
