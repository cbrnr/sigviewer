#ifndef CALCULATE_EVENT_MEAN_COMMAND_H
#define CALCULATE_EVENT_MEAN_COMMAND_H

#include "../main_window_model.h"

#include <QObject>
#include <QSharedPointer>

#include <vector>

namespace BioSig_
{

class EventManager;

class CalculateEventMeanCommand : public QObject
{
    Q_OBJECT

public:
    CalculateEventMeanCommand (EventManager const& event_manager,
                               ChannelManager const& channel_manager,
                               MainWindowModel& main_window_model,
                               uint16 event_type,
                               std::vector<uint32> channels,
                               float seconds_before_event,
                               float length_in_seconds);

public slots:
    void execute ();

private:
    EventManager const& event_manager_;
    ChannelManager const& channel_manager_;
    MainWindowModel& main_window_model_;
    uint16 event_type_;
    std::vector<uint32> channels_;
    float seconds_before_event_;
    float length_in_seconds_;
};

}

#endif // CALCULATE_EVENT_MEAN_COMMAND_H
