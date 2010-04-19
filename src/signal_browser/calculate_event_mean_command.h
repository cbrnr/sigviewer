#ifndef CALCULATE_EVENT_MEAN_COMMAND_H
#define CALCULATE_EVENT_MEAN_COMMAND_H

#include "../main_window_model_impl.h"
#include "../file_handling/event_manager.h"
#include "../file_handling/channel_manager.h"

#include <QObject>
#include <QSharedPointer>

#include <vector>

namespace BioSig_
{

class CalculateEventMeanCommand : public QObject
{
    Q_OBJECT

public:
    CalculateEventMeanCommand (QSharedPointer<EventManager const> event_manager,
                               QSharedPointer<ChannelManager const> channel_manager,
                               MainWindowModelImpl& main_window_model,
                               uint16 event_type,
                               std::vector<uint32> channels,
                               float seconds_before_event,
                               float length_in_seconds);

public slots:
    void execute ();

private:
    QSharedPointer<EventManager const> event_manager_;
    QSharedPointer<ChannelManager const> channel_manager_;
    MainWindowModelImpl& main_window_model_;
    uint16 event_type_;
    std::vector<uint32> channels_;
    float seconds_before_event_;
    float length_in_seconds_;
};

}

#endif // CALCULATE_EVENT_MEAN_COMMAND_H
