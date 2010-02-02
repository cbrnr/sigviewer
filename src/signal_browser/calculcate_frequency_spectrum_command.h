#ifndef CALCULCATE_FREQUENCY_SPECTRUM_COMMAND_H
#define CALCULCATE_FREQUENCY_SPECTRUM_COMMAND_H

#include "signal_browser_model_4.h"
#include "../main_window_model.h"

#include <QObject>
#include <QSharedPointer>

namespace BioSig_
{

class CalculcateFrequencySpectrumCommand : public QObject
{
Q_OBJECT
public:
    CalculcateFrequencySpectrumCommand (QSharedPointer<SignalBrowserModel const> signal_browser_model,
                                        MainWindowModel& main_window_model,
                                        uint16 event_type,
                                        std::vector<uint32> channels,
                                        float seconds_before_event,
                                        float length_in_seconds);

signals:

public slots:
    void execute ();

private:
    QSharedPointer<SignalBrowserModel const> signal_browser_model_;
    MainWindowModel& main_window_model_;
    uint16 event_type_;
    std::vector<uint32> channels_;
    float seconds_before_event_;
    float length_in_seconds_;
};

}

#endif // CALCULCATE_FREQUENCY_SPECTRUM_COMMAND_H
