// © SigViewer developers
//
// License: GPL-3.0


#include "down_sampling_thread.h"
#include "base/fixed_data_block.h"
#include "gui/background_processes.h"
#include "gui/progress_bar.h"

#include <vector>
#include <limits>
#include <cmath>
#include <QDebug>

// SPUC headers define macros (E, PI, ...) that must come after Qt/STL headers
// to avoid corrupting Qt6's C++20 constructs in C++17 mode.
#include <fstream>
#include "signal_processing/SPUC/butterworth.h"

namespace SigViewer_
{

using namespace sigviewer;

QString DownSamplingThread::PROCESS_NAME_ ("Downsampling...");

//-----------------------------------------------------------------------------
DownSamplingThread::DownSamplingThread (QList<QSharedPointer<DataBlock> > data, unsigned downsampling_step, unsigned downsampling_max) :
    basis_data_ (data),
    data_ (data),
    downsampling_step_ (downsampling_step),
    downsampling_max_ (downsampling_max)
{

}

//-----------------------------------------------------------------------------
DownSamplingThread::DownSamplingThread (QSharedPointer<ChannelManager> channel_manager, unsigned downsampling_step, unsigned downsampling_max)
    : channel_manager_ (channel_manager),
      downsampling_step_ (downsampling_step),
      downsampling_max_ (downsampling_max)
{

}

//-----------------------------------------------------------------------------
DownSamplingThread::~DownSamplingThread ()
{
    BackgroundProcesses::instance().removeProcess (PROCESS_NAME_);
}

//-----------------------------------------------------------------------------
void DownSamplingThread::run ()
{
    running_ = true;

    qDebug () << "DownSamplingThread::run started downsampling; QThread::currentThread = " << QThread::currentThread();

    if (!channel_manager_.isNull())
        minMaxDownsampling ();
    else
        downsampleAllOnBasisData ();
    //downsampleOnDownsampledData ();

    qDebug () << "DownSamplingThread::run FINISHED!";

    running_ = false;
}

//-------------------------------------------------------------------------
void DownSamplingThread::runSynchronously ()
{
    minMaxDownsampling ();
}

//-------------------------------------------------------------------------
void DownSamplingThread::minMaxDownsampling ()
{
    std::set<ChannelID> channels = channel_manager_->getChannels ();
    size_t num_samples = channel_manager_->getNumberSamples ();
    double sample_rate = channel_manager_->getSampleRate ();

    for (ChannelID id : channels)
    {
        QSharedPointer<DataBlock const> full_data =
            channel_manager_->getData (id, 0, static_cast<unsigned>(num_samples));
        if (full_data.isNull () || full_data->size () == 0)
            continue;

        size_t cur_size = full_data->size ();

        // Seed working buffers from the raw samples.
        // NaN values propagate: a downsampled window is NaN only when every
        // raw sample inside it is NaN.
        // We also track the global per-channel min/max here so the ChannelManager
        // base-class cache is pre-populated and initMinMax() (an expensive second
        // full file scan) is never called.
        QVector<float32> cur_min (static_cast<int>(cur_size));
        QVector<float32> cur_max (static_cast<int>(cur_size));
        float32 global_min =  std::numeric_limits<float32>::max ();
        float32 global_max = -std::numeric_limits<float32>::max ();
        for (size_t i = 0; i < cur_size; i++)
        {
            float32 v = (*full_data)[i];
            cur_min[static_cast<int>(i)] = v;
            cur_max[static_cast<int>(i)] = v;
            if (!std::isnan (v))
            {
                if (v < global_min) global_min = v;
                if (v > global_max) global_max = v;
            }
        }
        if (global_min <= global_max)
            channel_manager_->setChannelMinMax (id,
                                                static_cast<double>(global_min),
                                                static_cast<double>(global_max));

        if (per_channel_hook_)
            per_channel_hook_ (id);
        else
            ProgressBar::instance().increaseValue (1, QObject::tr ("Searching for Min-Max"));

        // Build the min/max pyramid hierarchically: each level aggregates the
        // previous one by downsampling_step_, so total work is O(N) per channel
        // (geometric series: N + N/step + N/step^2 + ...).
        for (unsigned factor = downsampling_step_;
             factor < downsampling_max_ && cur_size > 1;
             factor *= downsampling_step_)
        {
            size_t next_size = (cur_size + downsampling_step_ - 1) / downsampling_step_;
            QVector<float32> next_min (static_cast<int>(next_size));
            QVector<float32> next_max (static_cast<int>(next_size));

            for (size_t i = 0; i < next_size; i++)
            {
                size_t j0 = i * downsampling_step_;
                size_t j1 = std::min (j0 + static_cast<size_t>(downsampling_step_), cur_size);

                float32 vmin = std::numeric_limits<float32>::quiet_NaN ();
                float32 vmax = std::numeric_limits<float32>::quiet_NaN ();

                for (size_t j = j0; j < j1; j++)
                {
                    float32 lo = cur_min[static_cast<int>(j)];
                    float32 hi = cur_max[static_cast<int>(j)];
                    if (!std::isnan (lo))
                    {
                        if (std::isnan (vmin))
                        {
                            vmin = lo;
                            vmax = hi;
                        }
                        else
                        {
                            if (lo < vmin) vmin = lo;
                            if (hi > vmax) vmax = hi;
                        }
                    }
                }

                next_min[static_cast<int>(i)] = vmin;
                next_max[static_cast<int>(i)] = vmax;
            }

            // Persist this level – copy into fresh QVectors so the working
            // buffers below can be moved cheaply on the next iteration.
            auto stored_min = QSharedPointer<QVector<float32>> (
                                  new QVector<float32> (next_min));
            auto stored_max = QSharedPointer<QVector<float32>> (
                                  new QVector<float32> (next_max));

            double ds_rate = sample_rate / factor;
            channel_manager_->addDownsampledMinMaxVersion (
                id,
                QSharedPointer<DataBlock const> (new FixedDataBlock (stored_min, ds_rate)),
                QSharedPointer<DataBlock const> (new FixedDataBlock (stored_max, ds_rate)),
                factor);

            // Advance working buffers to this level for the next iteration.
            cur_min = std::move (next_min);
            cur_max = std::move (next_max);
            cur_size = next_size;
        }
    }

    // All channels have been scanned – mark the base-class min/max cache as
    // fully built so that ChannelManager::initMinMax() is never triggered.
    channel_manager_->markMinMaxInitialized ();
}

//-------------------------------------------------------------------------
void DownSamplingThread::downsampleAllOnBasisData ()
{
    QMap<unsigned, QVector<QSharedPointer<QVector<float32> > > > raw_downsampled_data;
    QMap<unsigned, QVector<QSharedPointer<SPUC::butterworth<float32> > > > low_pass_filters;
    QMap<unsigned, QVector<float32> > sample_rates;
    unsigned max_channel_length = 0;

    for (unsigned downsampling_factor = downsampling_step_;
         downsampling_factor < downsampling_max_;
         downsampling_factor *= downsampling_step_)
    {
        for (int channel = 0; channel < data_.size(); channel++)
        {
            max_channel_length = std::max (max_channel_length, static_cast<unsigned>(basis_data_[channel]->size()));
            sample_rates[downsampling_factor].append (basis_data_[channel]->getSampleRatePerUnit() / downsampling_factor);

            QSharedPointer<QVector<float32> > raw_data_vector (new QVector<float32> (static_cast<int>(basis_data_[channel]->size () / downsampling_factor) + 1));

            raw_downsampled_data[downsampling_factor].append (raw_data_vector);
            low_pass_filters[downsampling_factor].append (QSharedPointer<SPUC::butterworth<float32> > (new SPUC::butterworth<float32> (0.5 / downsampling_factor, 4, 3)));
        }
    }

    BackgroundProcesses::instance().addProcess (PROCESS_NAME_, max_channel_length);

    for (unsigned sample_index = 0; sample_index < max_channel_length; sample_index++)
    {
        for (int channel = 0; channel < basis_data_.size(); channel++)
        {
            if (basis_data_[channel]->size() > sample_index)
            {
                foreach (unsigned downsampling_factor, low_pass_filters.keys())
                {
                    float32 value = low_pass_filters[downsampling_factor][channel]->clock (basis_data_[channel]->operator [](sample_index));
                    if ((sample_index + 1) % downsampling_factor == 0)
                        raw_downsampled_data[downsampling_factor][channel]->operator []((sample_index + 1) / downsampling_factor) = value;
                }
            }
        }
        if (sample_index % 100 == 0)
            BackgroundProcesses::instance().setProcessState (PROCESS_NAME_, sample_index);
    }
    BackgroundProcesses::instance().removeProcess (PROCESS_NAME_);
}

//-------------------------------------------------------------------------
void DownSamplingThread::downsampleOnDownsampledData ()
{
    for (unsigned downsampling_factor = downsampling_step_;
         downsampling_factor < downsampling_max_;
         downsampling_factor *= downsampling_step_)
    {
        unsigned max_channel_length = 0;
        QVector<QSharedPointer<QVector<float32> > > raw_downsampled_data;
        QVector<QSharedPointer<SPUC::butterworth<float32> > > low_pass_filters;
        QVector<float32> sample_rates_;

        for (int channel = 0; channel < data_.size(); channel++)
        {
            max_channel_length = std::max (max_channel_length, static_cast<unsigned>(data_[channel]->size()));
            raw_downsampled_data.append (QSharedPointer<QVector<float32> > (new QVector<float32> (static_cast<int>(data_[channel]->size () / downsampling_step_) + 1)));
            low_pass_filters.append (QSharedPointer<SPUC::butterworth<float32> > (new SPUC::butterworth<float32> (0.9 / downsampling_step_, 4, 3)));
            sample_rates_.append (data_[channel]->getSampleRatePerUnit() / downsampling_step_);
        }

        for (unsigned sample_index = 0; sample_index < max_channel_length; sample_index++)
        {
            for (int channel = 0; channel < data_.size(); channel++)
            {
                if (data_[channel]->size() > sample_index)
                {
                    float32 value = low_pass_filters[channel]->clock (data_[channel]->operator [](sample_index));
                    if ((sample_index + 1) % downsampling_step_ == 0)
                        raw_downsampled_data[channel]->operator []((sample_index + 1) / downsampling_step_) = value;
                }
            }
        }

        data_.clear ();
        low_pass_filters.clear();
        for (int channel = 0; channel < raw_downsampled_data.size(); channel++)
        {
            QSharedPointer<DataBlock> downsampled_data (new FixedDataBlock (raw_downsampled_data[channel], sample_rates_[channel]));
            data_.append (downsampled_data);
        }
    }
}



}
