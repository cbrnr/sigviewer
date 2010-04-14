#include "calculate_event_mean_command.h"
#include "../base/data_block.h"
#include "../base/signal_event.h"
#include "../block_visualisation/blocks_visualisation_view.h"
#include "../block_visualisation/blocks_visualisation_model.h"

#include <QMap>

namespace BioSig_
{

//-----------------------------------------------------------------------------
CalculateEventMeanCommand::CalculateEventMeanCommand (QSharedPointer<EventManager const>
                                                      event_manager,
                                                      QSharedPointer<ChannelManager const>
                                                      channel_manager,
                                                      MainWindowModel&
                                                      main_window_model,
                                                      uint16 event_type,
                                                      std::vector<uint32> channels,
                                                      float seconds_before_event,
                                                      float length_in_seconds)
    : event_manager_ (event_manager),
      channel_manager_ (channel_manager),
      main_window_model_ (main_window_model),
      event_type_ (event_type),
      channels_ (channels),
      seconds_before_event_ (seconds_before_event),
      length_in_seconds_ (length_in_seconds)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void CalculateEventMeanCommand::execute ()
{
    QSharedPointer<BlocksVisualisationModel> bv_model = main_window_model_.createBlocksVisualisationView (tr("Mean"));

    //uint32 samples_before_event = seconds_before_event_ * event_manager_.getSampleRate();
    //uint32 number_samples = length_in_seconds_ * event_manager_.getSampleRate();

    for (unsigned index = 0; index < channels_.size(); index++)
    {
        std::list<QSharedPointer<DataBlock const> > data;

        QList<EventID> events (event_manager_->getEvents(event_type_));

        for (QList<EventID>::const_iterator
             event_id = events.begin();
             event_id != events.end(); ++event_id)
        {
            QSharedPointer<SignalEvent const> event = event_manager_->getEvent(*event_id);
            data.push_back (channel_manager_->getData (channels_[index],
                                                      event->getPosition(),
                                                      event->getDuration()));
        }

        QSharedPointer<DataBlock> mean = QSharedPointer<DataBlock> (new DataBlock (DataBlock::calculateMean (data)));
        mean->setXUnitLabel ("s");
        //QSharedPointer<DataBlock> standard_deviation = QSharedPointer<DataBlock> (new DataBlock (DataBlock::calculateStandardDeviation (data)));

        bv_model->visualiseBlock (mean, QSharedPointer<DataBlock>(0));//standard_deviation);
    }
    bv_model->updateLayout();
}


}
