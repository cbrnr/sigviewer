#ifndef CALCULCATE_FREQUENCY_SPECTRUM_COMMAND_H
#define CALCULCATE_FREQUENCY_SPECTRUM_COMMAND_H

#include "signal_browser_model_4.h"
#include "../main_window_model.h"

#include <QObject>

namespace BioSig_
{

class CalculcateFrequencySpectrumCommand : public QObject
{
Q_OBJECT
public:
    CalculcateFrequencySpectrumCommand (SignalBrowserModel const& signal_browser_model,
                                        MainWindowModel& main_window_model);

signals:

public slots:
    void execute ();

private:
    SignalBrowserModel const& signal_browser_model_;
    MainWindowModel& main_window_model_;
};

}

#endif // CALCULCATE_FREQUENCY_SPECTRUM_COMMAND_H
