#include "calculcate_frequency_spectrum_command.h"

#include "../base/data_block.h"
#include "../block_visualisation/blocks_visualisation_model.h"
//#include "fftw++.h"

#include <QSharedPointer>

#include <vector>
#include <cmath>

namespace BioSig_
{

CalculcateFrequencySpectrumCommand::CalculcateFrequencySpectrumCommand(SignalBrowserModel const&
                                                                       signal_browser_model,
                                                                       MainWindowModel&
                                                                       main_window_model)
    : signal_browser_model_ (signal_browser_model),
      main_window_model_ (main_window_model)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void CalculcateFrequencySpectrumCommand::execute ()
{
    /*
    QSharedPointer<BlocksVisualisationModel> bv_model = main_window_model_.createBlocksVisualisationView ();

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
