#include "calculcate_frequency_spectrum_command.h"

#include "../base/data_block.h"
#include "../block_visualisation/blocks_visualisation_model.h"
#include "../signal_processing/fftw++.h"

#include <QSharedPointer>

#include <vector>
#include <cmath>

namespace BioSig_
{

CalculcateFrequencySpectrumCommand::CalculcateFrequencySpectrumCommand(QSharedPointer<SignalBrowserModel const>
                                                                       signal_browser_model,
                                                                       MainWindowModel&
                                                                       main_window_model,
                                                                       uint16 event_type,
                                                                       std::vector<uint32> channels,
                                                                       float seconds_before_event,
                                                                       float length_in_seconds)
                     : signal_browser_model_ (signal_browser_model),
                       main_window_model_ (main_window_model),
                       event_type_ (event_type),
                       channels_ (channels),
                       seconds_before_event_ (seconds_before_event),
                       length_in_seconds_ (length_in_seconds)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void CalculcateFrequencySpectrumCommand::execute ()
{
    SignalBuffer const& signal_buffer = signal_browser_model_->getSignalBuffer();
    QSharedPointer<BlocksVisualisationModel> bv_model = main_window_model_.createBlocksVisualisationView (tr("Power Spectrum"));

    uint32 samples_before_event = seconds_before_event_ * signal_buffer.getEventSamplerate();
    uint32 number_samples = length_in_seconds_ * signal_buffer.getEventSamplerate();

    for (unsigned index = 0; index < channels_.size(); index++)
    {
        std::list<DataBlock> data;
        std::list<DataBlock> lowpassed_data;

        QMap<int32, QSharedPointer<SignalEvent> > events (signal_buffer.getEvents(event_type_));

        for (QMap<int32, QSharedPointer<SignalEvent> >::const_iterator
             event = events.begin();
             event != events.end(); ++event)
        {
            DataBlock signal_data = signal_buffer.getSignalData(channels_[index],
                                                        event.value()->getPosition() - samples_before_event,
                                                        number_samples);

            // calculate frequency spectrum
            unsigned num_samples = signal_data.size();
            double* data_in = new double[num_samples];
            for (unsigned x = 0; x < num_samples; x++)
                data_in[x] = signal_data[x];

            Complex* data_out = FFTWComplex((num_samples / 2) + 1);

            rcfft1d forward (num_samples, data_in, data_out);
            forward.fft0Normalized (data_in, data_out);

            std::vector<float32> spectrum_data;

            for (unsigned x = 1; x < (num_samples / 2) + 1; x++)
                spectrum_data.push_back(pow(data_out[x].real(),2) + pow(data_out[x].imag(), 2));

            DataBlock frequency_data (spectrum_data, static_cast<float32>(num_samples) / signal_data.getSampleRatePerUnit());

            delete[] data_in;
            FFTWdelete(data_out);
            // finished calculation

            data.push_back (frequency_data);
        }

        QSharedPointer<DataBlock> mean = QSharedPointer<DataBlock> (new DataBlock (DataBlock::calculateMean (data)));
        mean->setLabel (signal_browser_model_->getShownChannels()[channels_[index]].toStdString());
        mean->setXUnitLabel ("Hz");
        //QSharedPointer<DataBlock> standard_deviation = QSharedPointer<DataBlock> (new DataBlock (DataBlock::calculateStandardDeviation (data)));

        bv_model->visualiseBlock (mean, QSharedPointer<DataBlock>(0));//standard_deviation);
    }
    bv_model->updateLayout();
/*    QSharedPointer<BlocksVisualisationModel> bv_model = main_window_model_.createBlocksVisualisationView ();

    QSharedPointer<DataBlock> data = QSharedPointer<DataBlock> (new DataBlock ());

    std::vector<float32> raw_data;

    for (unsigned x = 0; x < 1000; x++)
        raw_data.push_back(sin((double)x * 0.5 / 3.14));
    for (unsigned x = 0; x < 1000; x++)
        raw_data[x] += sin(((double)x*9.0 / (3.14))) / 2.0;
    for (unsigned x = 0; x < 1000; x++)
        raw_data[x] += sin(((double)x*10.0 / (3.14))) / 3.0;
    for (unsigned x = 0; x < 1000; x++)
        raw_data[x] += sin(((double)x*13.0 / (3.14))) / 4.0;


    data->setData (raw_data);

    double* data_in = new double[raw_data.size()];
    for (unsigned x = 0; x < raw_data.size(); x++)
        data_in[x] = raw_data[x];

    Complex* data_out = FFTWComplex((raw_data.size() / 2) + 1);

    rcfft1d forward(raw_data.size(), data_in, data_out);
    forward.fft0Normalized (data_in, data_out);

    std::vector<float32> spectrum_data;

    for (unsigned x = 0; x < (raw_data.size() / 2) + 1; x++)
        spectrum_data.push_back(sqrt(pow(data_out[x].real(),2) + pow(data_out[x].imag(), 2)));


    for (unsigned x = (raw_data.size() / 6); x < (raw_data.size() / 2) + 1; x++)
    {
        data_out[x].real() = 0;
        data_out[x].imag() = 0;
    }

    crfft1d backward (raw_data.size(), data_out, data_in);
    backward.fft (data_out, data_in);

    std::vector<float32> back_data;
    for (unsigned x = 0; x < raw_data.size(); x++)
        back_data.push_back (data_in[x]);


    delete[] data_in;
    FFTWdelete(data_out);

    QSharedPointer<DataBlock> data_spec = QSharedPointer<DataBlock> (new DataBlock (spectrum_data));
    QSharedPointer<DataBlock> data_spec_back = QSharedPointer<DataBlock> (new DataBlock (back_data));


    bv_model->visualiseBlock (data, QSharedPointer<DataBlock>(0));
    bv_model->visualiseBlock (data_spec, QSharedPointer<DataBlock>(0));
    bv_model->visualiseBlock (data_spec_back, QSharedPointer<DataBlock>(0));

    bv_model->updateLayout();*/
}


}
