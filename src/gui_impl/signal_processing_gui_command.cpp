#include "signal_processing_gui_command.h"
#include "dialogs/event_time_selection_dialog.h"
#include "gui_helper_functions.h"
#include "processed_signal_channel_manager.h"
#include "../application_context.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
QString const SignalProcessingGuiCommand::MEAN_ = "Calculate Mean...";
QStringList const SignalProcessingGuiCommand::ACTIONS_ = QStringList () <<
                                                         SignalProcessingGuiCommand::MEAN_;

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator SignalProcessingGuiCommand::registrator_ ("Signal Processing",
                                                                      QSharedPointer<GuiActionCommand>(new SignalProcessingGuiCommand));

//-----------------------------------------------------------------------------
SignalProcessingGuiCommand::SignalProcessingGuiCommand ()
    : GuiActionCommand (ACTIONS_)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void SignalProcessingGuiCommand::init ()
{
    resetActionTriggerSlot (MEAN_, SLOT(calculateMeanAndStandardDeviation()));
}

//-----------------------------------------------------------------------------
void SignalProcessingGuiCommand::calculateMeanAndStandardDeviation ()
{
    QSharedPointer<ChannelManager> channel_manager = ApplicationContext::getInstance()->getCurrentFileContext()->getChannelManager();
    QSharedPointer<EventManager> event_manager = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager();
    std::set<EventType> event_types = currentVisModel()->getShownEventTypes();
    std::set<ChannelID> file_channels = currentVisModel()->getShownChannels();
    EventTimeSelectionDialog event_dialog (event_types, file_channels, channel_manager, event_manager);
    if (event_dialog.exec () != QDialog::Accepted)
        return;


    std::set<ChannelID> channels = event_dialog.getSelectedChannels ();
    EventType event_type = event_dialog.getSelectedEventType ();
    unsigned num_samples = channel_manager->getSampleRate() * event_dialog.getLengthInSeconds ();

    QSharedPointer<ProcessedSignalChannelManager> processed_channel_manager (new ProcessedSignalChannelManager(channel_manager->getSampleRate(),
                                                                                                               num_samples));

    foreach (ChannelID channel_id, channels)
    {
        std::list<QSharedPointer<DataBlock const> > data;

        QList<EventID> events (event_manager->getEvents(event_type));

        foreach (EventID event_id, events)
        {
            QSharedPointer<SignalEvent const> event = event_manager->getEvent (event_id);
            data.push_back (channel_manager->getData (channel_id,
                                                      event->getPosition(),
                                                      num_samples));
        }

        QSharedPointer<DataBlock> mean = QSharedPointer<DataBlock> (new DataBlock (DataBlock::calculateMean (data)));
        processed_channel_manager->addChannel (channel_id, mean, channel_manager->getChannelLabel(channel_id));
    }

    QSharedPointer<SignalVisualisationModel> signal_visualisation_model =
            ApplicationContext::getInstance()->getMainWindowModel()->createSignalVisualisation (processed_channel_manager);

    signal_visualisation_model->setShownChannels (channels);
    signal_visualisation_model->updateLayout();
}


}
