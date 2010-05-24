#include "signal_processing_gui_command.h"
#include "gui_helper_functions.h"
#include "processed_signal_channel_manager.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
QString const SignalProcessingGuiCommand::MEAN_ = "Calculate Mean...";
QString const SignalProcessingGuiCommand::POWER_SPECTRUM_ = "Power Spectrum...";
QStringList const SignalProcessingGuiCommand::ACTIONS_ = QStringList () <<
                                                         SignalProcessingGuiCommand::MEAN_ <<
                                                         SignalProcessingGuiCommand::POWER_SPECTRUM_;

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
    resetActionTriggerSlot (POWER_SPECTRUM_, SLOT(calculatePowerSpectrum()));
}

//-----------------------------------------------------------------------------
void SignalProcessingGuiCommand::evaluateEnabledness ()
{
    disableIfNoFileIsOpened (ACTIONS_);
}


//-----------------------------------------------------------------------------
void SignalProcessingGuiCommand::calculateMeanAndStandardDeviation ()
{
    QSharedPointer<ChannelManager const> channel_manager = currentFileContext()->getChannelManager();
    QSharedPointer<EventManager> event_manager = currentFileContext()->getEventManager();
    QSharedPointer<EventTimeSelectionDialog> event_dialog = getFinishedEventTimeSelectionDialog();
    if (event_dialog.isNull())
        return;

    unsigned num_samples = channel_manager->getSampleRate() * event_dialog->getLengthInSeconds ();

    QSharedPointer<ProcessedSignalChannelManager> processed_channel_manager (new ProcessedSignalChannelManager(channel_manager->getSampleRate(),
                                                                                                               num_samples));
    QList<EventID> events (event_manager->getEvents(event_dialog->getSelectedEventType ()));
    foreach (ChannelID channel_id, event_dialog->getSelectedChannels ())
    {
        std::list<QSharedPointer<DataBlock const> > data;

        foreach (EventID event_id, events)
        {
            QSharedPointer<SignalEvent const> event = event_manager->getEvent (event_id);
            data.push_back (channel_manager->getData (channel_id,
                                                      event->getPosition(),
                                                      num_samples));
        }

        QSharedPointer<DataBlock> mean = QSharedPointer<DataBlock> (new DataBlock (DataBlock::calculateMean (data)));
//        QSharedPointer<DataBlock> standard_deviation = QSharedPointer<DataBlock> (new DataBlock (DataBlock::calculateStandardDeviation(data, *(mean.data()))));
//        processed_channel_manager->addChannel (channel_id, standard_deviation, tr("Standard Deviation\n") + channel_manager->getChannelLabel(channel_id));
        processed_channel_manager->addChannel (channel_id, mean, channel_manager->getChannelLabel(channel_id));
    }

    createVisualisation (tr("Mean"), processed_channel_manager);
}


//-------------------------------------------------------------------------
void SignalProcessingGuiCommand::calculatePowerSpectrum ()
{
    QSharedPointer<ChannelManager const> channel_manager = currentFileContext()->getChannelManager();
    QSharedPointer<EventManager> event_manager = currentFileContext()->getEventManager();
    QSharedPointer<EventTimeSelectionDialog> event_dialog = getFinishedEventTimeSelectionDialog();
    if (event_dialog.isNull())
        return;

    unsigned num_samples = channel_manager->getSampleRate() * event_dialog->getLengthInSeconds ();

    QSharedPointer<ProcessedSignalChannelManager> processed_channel_manager (new ProcessedSignalChannelManager(static_cast<float32>(num_samples) / channel_manager->getSampleRate(),
                                                                                                               num_samples / 2));
    QList<EventID> events (event_manager->getEvents(event_dialog->getSelectedEventType ()));

    foreach (ChannelID channel_id, event_dialog->getSelectedChannels ())
    {
        std::list<QSharedPointer<DataBlock const> > data;

        foreach (EventID event_id, events)
        {
            QSharedPointer<SignalEvent const> event = event_manager->getEvent (event_id);
            data.push_back (channel_manager->getData (channel_id,
                                                      event->getPosition(),
                                                      num_samples)->createPowerSpectrum ());
        }

        QSharedPointer<DataBlock> mean = QSharedPointer<DataBlock> (new DataBlock (DataBlock::calculateMean (data)));
        processed_channel_manager->addChannel (channel_id, mean, channel_manager->getChannelLabel(channel_id));
    }

    createVisualisation (tr("Power Spectrum"), processed_channel_manager);
}

//-------------------------------------------------------------------------
QSharedPointer<EventTimeSelectionDialog> SignalProcessingGuiCommand::getFinishedEventTimeSelectionDialog ()
{
    QSharedPointer<EventTimeSelectionDialog> event_dialog (
            new EventTimeSelectionDialog (currentFileContext()->getMainVisualisationModel()->getShownEventTypes(),
                                          currentFileContext()->getMainVisualisationModel()->getShownChannels(),
                                          currentFileContext()->getMainVisualisationModel()->getChannelManager(),
                                          currentFileContext()->getMainVisualisationModel()->getEventManager()));

    if (event_dialog->exec() == QDialog::Accepted)
        return event_dialog;
    else
        return QSharedPointer<EventTimeSelectionDialog> (0);
}

//-------------------------------------------------------------------------
void SignalProcessingGuiCommand::createVisualisation (QString const& title, QSharedPointer<ChannelManager> channel_manager)
{
    QSharedPointer<SignalVisualisationModel> signal_visualisation_model =
            applicationContext()->getMainWindowModel()->createSignalVisualisation (title, channel_manager);

    signal_visualisation_model->setShownChannels (channel_manager->getChannels());
    signal_visualisation_model->update();
}


}
