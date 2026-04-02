// © SigViewer developers
//
// License: GPL-3.0

#include "downsampling_thread.h"

#include <QCoreApplication>
#include <QDebug>
#include <QRunnable>
#include <QThread>
#include <QThreadPool>
#include <atomic>
#include <cmath>
#include <limits>
#include <vector>

#include "base/fixed_data_block.h"
#include "gui/background_processes.h"
#include "gui/progress_bar.h"

// SPUC headers define macros (E, PI, ...) that must come after Qt/STL headers
// to avoid corrupting Qt6's C++20 constructs in C++17 mode.
#include <fstream>

#include "signal_processing/SPUC/butterworth.h"

namespace SigViewer_ {

using namespace sigviewer;

QString DownSamplingThread::PROCESS_NAME_("Downsampling...");

DownSamplingThread::DownSamplingThread(QList<QSharedPointer<DataBlock>> data,
    unsigned downsampling_step,
    unsigned downsampling_max)
    : basis_data_(data),
      data_(data),
      downsampling_step_(downsampling_step),
      downsampling_max_(downsampling_max) {}

DownSamplingThread::DownSamplingThread(QSharedPointer<ChannelManager> channel_manager,
    unsigned downsampling_step,
    unsigned downsampling_max)
    : channel_manager_(channel_manager),
      downsampling_step_(downsampling_step),
      downsampling_max_(downsampling_max) {}

DownSamplingThread::~DownSamplingThread() {
    BackgroundProcesses::instance().removeProcess(PROCESS_NAME_);
}

void DownSamplingThread::run() {
    running_ = true;

    qDebug()
        << "DownSamplingThread::run started downsampling; QThread::currentThread = "
        << QThread::currentThread();

    if (!channel_manager_.isNull())
        minMaxDownsampling();
    else
        downsampleAllOnBasisData();
    // downsampleOnDownsampledData ();

    qDebug() << "DownSamplingThread::run FINISHED!";

    running_ = false;
}

void DownSamplingThread::runSynchronously() { minMaxDownsampling(); }

void DownSamplingThread::minMaxDownsampling() {
    std::set<ChannelID> channels = channel_manager_->getChannels();
    size_t num_samples = channel_manager_->getNumberSamples();
    double sample_rate = channel_manager_->getSampleRate();

    if (channels.empty()) return;

    // Pre-trigger bufferAllChannels() on the calling thread so that subsequent
    // getData() calls from worker threads are O(1) mutex-guarded map lookups
    // rather than a file read.  The first call reads all channels in one shot;
    // subsequent calls just return a cached QSharedPointer.
    channel_manager_->getData(*channels.begin(), 0, static_cast<unsigned>(num_samples));

    // Per-channel results collected by worker threads, merged serially afterwards.
    struct PerChannelResult {
        ChannelID id = 0;
        QSharedPointer<DataBlock const> full_data;
        float32 global_min = std::numeric_limits<float32>::max();
        float32 global_max = -std::numeric_limits<float32>::max();
        struct Level {
            unsigned factor;
            QSharedPointer<DataBlock const> min_block;
            QSharedPointer<DataBlock const> max_block;
        };
        QVector<Level> levels;
    };

    QVector<PerChannelResult> results;
    results.resize(static_cast<int>(channels.size()));
    {
        int i = 0;
        for (ChannelID id : channels) results[i++].id = id;
    }

    std::atomic<int> n_done{0};
    auto* pool = QThreadPool::globalInstance();
    int n_channels = results.size();

    for (int i = 0; i < n_channels; ++i) {
        PerChannelResult* r = &results[i];
        unsigned ds_step = downsampling_step_;
        unsigned ds_max = downsampling_max_;

        pool->start(QRunnable::create([this, r, num_samples, sample_rate, ds_step, ds_max, &n_done]() {
            r->full_data = channel_manager_
                               ->getData(r->id, 0, static_cast<unsigned>(num_samples));
            if (r->full_data.isNull() || r->full_data->size() == 0) {
                n_done.fetch_add(1, std::memory_order_release);
                return;
            }

            size_t cur_size = r->full_data->size();
            QVector<float32> cur_min(static_cast<int>(cur_size));
            QVector<float32> cur_max(static_cast<int>(cur_size));

            for (size_t k = 0; k < cur_size; ++k) {
                float32 v = (*r->full_data)[static_cast<int>(k)];
                cur_min[static_cast<int>(k)] = v;
                cur_max[static_cast<int>(k)] = v;
                if (!std::isnan(v)) {
                    if (v < r->global_min) r->global_min = v;
                    if (v > r->global_max) r->global_max = v;
                }
            }

            for (unsigned factor = ds_step; factor < ds_max && cur_size > 1; factor *= ds_step) {
                size_t next_size = (cur_size + ds_step - 1) / ds_step;
                QVector<float32> next_min(static_cast<int>(next_size));
                QVector<float32> next_max(static_cast<int>(next_size));

                for (size_t ii = 0; ii < next_size; ++ii) {
                    size_t j0 = ii * ds_step;
                    size_t j1 = std::min(j0 + static_cast<size_t>(ds_step), cur_size);

                    float32 vmin = std::numeric_limits<float32>::quiet_NaN();
                    float32 vmax = std::numeric_limits<float32>::quiet_NaN();

                    for (size_t j = j0; j < j1; ++j) {
                        float32 lo = cur_min[static_cast<int>(j)];
                        float32 hi = cur_max[static_cast<int>(j)];
                        if (!std::isnan(lo)) {
                            if (std::isnan(vmin)) {
                                vmin = lo;
                                vmax = hi;
                            } else {
                                if (lo < vmin) vmin = lo;
                                if (hi > vmax) vmax = hi;
                            }
                        }
                    }
                    next_min[static_cast<int>(ii)] = vmin;
                    next_max[static_cast<int>(ii)] = vmax;
                }

                double ds_rate = sample_rate / factor;
                // Store a copy for the pyramid block, then advance working buffers.
                auto stored_min =
                    QSharedPointer<QVector<float32>>(new QVector<float32>(next_min));
                auto stored_max =
                    QSharedPointer<QVector<float32>>(new QVector<float32>(next_max));
                r->levels.append({factor,
                    QSharedPointer<DataBlock const>(new FixedDataBlock(stored_min, ds_rate)),
                    QSharedPointer<DataBlock const>(new FixedDataBlock(stored_max, ds_rate))});

                cur_min = std::move(next_min);
                cur_max = std::move(next_max);
                cur_size = next_size;
            }

            n_done.fetch_add(1, std::memory_order_release);
        }));
    }

    // Poll while the thread pool works; update progress bar as channels complete.
    int last_reported = 0;
    while (last_reported < n_channels) {
        int n = n_done.load(std::memory_order_acquire);
        if (n > last_reported) {
            ProgressBar::instance()
                .increaseValue(n - last_reported, QObject::tr("Processing channels..."));
            last_reported = n;
        }
        if (last_reported < n_channels) {
            QCoreApplication::processEvents();
            QThread::msleep(8);
        }
    }
    pool->waitForDone();

    // Serial merge: write results to channel_manager_ (its maps are not thread-safe).
    for (auto& r : results) {
        if (r.global_min <= r.global_max)
            channel_manager_->setChannelMinMax(r.id,
                static_cast<double>(r.global_min),
                static_cast<double>(r.global_max));
        for (auto& lv : r.levels)
            channel_manager_
                ->addDownsampledMinMaxVersion(r.id, lv.min_block, lv.max_block, lv.factor);
        if (per_channel_hook_) per_channel_hook_(r.id, r.full_data);
    }

    // All channels processed — mark base-class min/max cache as fully built.
    channel_manager_->markMinMaxInitialized();
}

void DownSamplingThread::downsampleAllOnBasisData() {
    QMap<unsigned, QVector<QSharedPointer<QVector<float32>>>> raw_downsampled_data;
    QMap<unsigned, QVector<QSharedPointer<SPUC::butterworth<float32>>>> low_pass_filters;
    QMap<unsigned, QVector<float32>> sample_rates;
    unsigned max_channel_length = 0;

    for (unsigned downsampling_factor = downsampling_step_; downsampling_factor < downsampling_max_;
        downsampling_factor *= downsampling_step_) {
        for (int channel = 0; channel < data_.size(); channel++) {
            max_channel_length = std::max(max_channel_length,
                static_cast<unsigned>(basis_data_[channel]->size()));
            sample_rates[downsampling_factor]
                .append(basis_data_[channel]->getSampleRatePerUnit() / downsampling_factor);

            QSharedPointer<QVector<float32>> raw_data_vector(new QVector<float32>(
                static_cast<int>(basis_data_[channel]->size() / downsampling_factor) + 1));

            raw_downsampled_data[downsampling_factor].append(raw_data_vector);
            low_pass_filters[downsampling_factor]
                .append(QSharedPointer<SPUC::butterworth<float32>>(
                    new SPUC::butterworth<float32>(0.5 / downsampling_factor, 4, 3)));
        }
    }

    BackgroundProcesses::instance().addProcess(PROCESS_NAME_, max_channel_length);

    for (unsigned sample_index = 0; sample_index < max_channel_length; sample_index++) {
        for (int channel = 0; channel < basis_data_.size(); channel++) {
            if (basis_data_[channel]->size() > sample_index) {
                foreach (unsigned downsampling_factor, low_pass_filters.keys()) {
                    float32 value = low_pass_filters[downsampling_factor][channel]->clock(
                        basis_data_[channel]->operator[](sample_index));
                    if ((sample_index + 1) % downsampling_factor == 0)
                        raw_downsampled_data[downsampling_factor][channel]
                            ->operator[]((sample_index + 1) / downsampling_factor) = value;
                }
            }
        }
        if (sample_index % 100 == 0)
            BackgroundProcesses::instance().setProcessState(PROCESS_NAME_, sample_index);
    }
    BackgroundProcesses::instance().removeProcess(PROCESS_NAME_);
}

void DownSamplingThread::downsampleOnDownsampledData() {
    for (unsigned downsampling_factor = downsampling_step_; downsampling_factor < downsampling_max_;
        downsampling_factor *= downsampling_step_) {
        unsigned max_channel_length = 0;
        QVector<QSharedPointer<QVector<float32>>> raw_downsampled_data;
        QVector<QSharedPointer<SPUC::butterworth<float32>>> low_pass_filters;
        QVector<float32> sample_rates_;

        for (int channel = 0; channel < data_.size(); channel++) {
            max_channel_length =
                std::max(max_channel_length, static_cast<unsigned>(data_[channel]->size()));
            raw_downsampled_data.append(QSharedPointer<QVector<float32>>(new QVector<float32>(
                static_cast<int>(data_[channel]->size() / downsampling_step_) + 1)));
            low_pass_filters.append(QSharedPointer<SPUC::butterworth<float32>>(
                new SPUC::butterworth<float32>(0.9 / downsampling_step_, 4, 3)));
            sample_rates_.append(data_[channel]->getSampleRatePerUnit() / downsampling_step_);
        }

        for (unsigned sample_index = 0; sample_index < max_channel_length; sample_index++) {
            for (int channel = 0; channel < data_.size(); channel++) {
                if (data_[channel]->size() > sample_index) {
                    float32 value = low_pass_filters[channel]
                                        ->clock(data_[channel]->operator[](sample_index));
                    if ((sample_index + 1) % downsampling_step_ == 0)
                        raw_downsampled_data[channel]
                            ->operator[]((sample_index + 1) / downsampling_step_) = value;
                }
            }
        }

        data_.clear();
        low_pass_filters.clear();
        for (int channel = 0; channel < raw_downsampled_data.size(); channel++) {
            QSharedPointer<DataBlock> downsampled_data(
                new FixedDataBlock(raw_downsampled_data[channel], sample_rates_[channel]));
            data_.append(downsampled_data);
        }
    }
}

}  // namespace SigViewer_
