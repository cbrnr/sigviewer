#ifndef CALCULATE_EVENT_MEAN_COMMAND_H
#define CALCULATE_EVENT_MEAN_COMMAND_H

#include "signal_browser_model_4.h"
#include "../main_window_model.h"

#include <QObject>
#include <QSharedPointer>

#include <vector>

namespace BioSig_
{

class CalculateEventMeanCommand : public QObject
{
    Q_OBJECT

public:
    CalculateEventMeanCommand (QSharedPointer<SignalBrowserModel const> signal_browser_model,
                               MainWindowModel& main_window_model,
                               uint16 event_type,
                               std::vector<uint32> channels,
                               float seconds_before_event,
                               float length_in_seconds);

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

#endif // CALCULATE_EVENT_MEAN_COMMAND_H
