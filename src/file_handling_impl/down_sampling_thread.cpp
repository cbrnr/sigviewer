#include "down_sampling_thread.h"

#include <fstream>

#include "signal_processing/SPUC/butterworth.h"
#include "base/fixed_data_block.h"
#include "gui/background_processes.h"

#include <vector>
#include <QMessageBox>
#include <QDebug>

namespace SigViewer_
{

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
void DownSamplingThread::minMaxDownsampling ()
{

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
            max_channel_length = std::max (max_channel_length, basis_data_[channel]->size());
            sample_rates[downsampling_factor].append (basis_data_[channel]->getSampleRatePerUnit() / downsampling_factor);

            QSharedPointer<QVector<float32> > raw_data_vector (new QVector<float32> (static_cast<int>(basis_data_[channel]->size () / downsampling_factor) + 1));

            QSharedPointer<DataBlock> downsampled_data (new FixedDataBlock (raw_data_vector, sample_rates[downsampling_factor][channel]));
            basis_data_[channel]->addDownSampledVersion (downsampled_data, downsampling_factor);

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
            max_channel_length = std::max (max_channel_length, data_[channel]->size());
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
            basis_data_[channel]->addDownSampledVersion (downsampled_data, downsampling_factor);
            data_.append (downsampled_data);
        }
    }
}



}
