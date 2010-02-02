#include "calculate_event_mean_command.h"
#include "../base/signal_buffer.h"
#include "../base/data_block.h"
#include "../base/signal_event.h"
#include "../block_visualisation/blocks_visualisation_view.h"
#include "../block_visualisation/blocks_visualisation_model.h"

#include <QMap>

namespace BioSig_
{

//-----------------------------------------------------------------------------
CalculateEventMeanCommand::CalculateEventMeanCommand (QSharedPointer<SignalBrowserModel const>
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
void CalculateEventMeanCommand::execute ()
{
    SignalBuffer const& signal_buffer = signal_browser_model_->getSignalBuffer();
    QSharedPointer<BlocksVisualisationModel> bv_model = main_window_model_.createBlocksVisualisationView (tr("Mean"));

    uint32 samples_before_event = seconds_before_event_ * signal_buffer.getEventSamplerate();
    uint32 number_samples = length_in_seconds_ * signal_buffer.getEventSamplerate();

    for (unsigned index = 0; index < channels_.size(); index++)
    {
        std::list<DataBlock> data;

        QMap<int32, QSharedPointer<SignalEvent> > events (signal_buffer.getEvents(event_type_));

        for (QMap<int32, QSharedPointer<SignalEvent> >::const_iterator
             event = events.begin();
             event != events.end(); ++event)
        {
            data.push_back (signal_buffer.getSignalData(channels_[index],
                                                        event.value()->getPosition() - samples_before_event,
                                                        number_samples));
        }

        QSharedPointer<DataBlock> mean = QSharedPointer<DataBlock> (new DataBlock (DataBlock::calculateMean (data)));
        //QSharedPointer<DataBlock> standard_deviation = QSharedPointer<DataBlock> (new DataBlock (DataBlock::calculateStandardDeviation (data)));

        bv_model->visualiseBlock (mean, QSharedPointer<DataBlock>(0));//standard_deviation);
    }
    bv_model->updateLayout();
}


}
