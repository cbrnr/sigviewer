#include "down_sampling_thread.h"
#include "signal_processing/SPUC/chebyshev.h"
#include "base/fixed_data_block.h"

#include <vector>
#include <QMessageBox>
#include <QDebug>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
DownSamplingThread::DownSamplingThread (QList<QSharedPointer<DataBlock> > data, unsigned downsampling_step, unsigned downsampling_max) :
    basis_data_ (data),
    data_ (data),
    downsampling_step_ (downsampling_step),
    downsampling_max_ (downsampling_max)
{

}

//-----------------------------------------------------------------------------
void DownSamplingThread::run ()
{
    running_ = true;

    qDebug () << "DownSamplingThread::run started downsampling; QThread::currentThread = " << QThread::currentThread();


    for (unsigned downsampling_factor = downsampling_step_;
         downsampling_factor < downsampling_max_;
         downsampling_factor *= downsampling_step_)
    {
        unsigned max_channel_length = 0;
        QVector<QSharedPointer<std::vector<float32> > > raw_downsampled_data;
        QVector<QSharedPointer<SPUC::chebyshev<float32> > > low_pass_filters;
        QVector<float32> sample_rates_;

        for (unsigned channel = 0; channel < data_.size(); channel++)
        {
            max_channel_length = std::max (max_channel_length, data_[channel]->size());
            raw_downsampled_data.append (QSharedPointer<std::vector<float32> > (new std::vector<float32> (data_[channel]->size () / downsampling_step_)));
            low_pass_filters.append (QSharedPointer<SPUC::chebyshev<float32> > (new SPUC::chebyshev<float32> (0.75 / downsampling_factor, 8, 0.01)));
            sample_rates_.append (data_[channel]->getSampleRatePerUnit() / downsampling_step_);
        }

        for (unsigned sample_index = 0; sample_index < max_channel_length; sample_index++)
        {
            for (unsigned channel = 0; channel < data_.size(); channel++)
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
        for (unsigned channel = 0; channel < raw_downsampled_data.size(); channel++)
        {
            QSharedPointer<DataBlock> downsampled_data (new FixedDataBlock (raw_downsampled_data[channel], sample_rates_[channel]));
            basis_data_[channel]->addDownSampledVersion (downsampled_data, downsampling_factor);
            data_.append (downsampled_data);
        }
    }

    qDebug () << "DownSamplingThread::run FINISHED!";

    running_ = false;
}


}
