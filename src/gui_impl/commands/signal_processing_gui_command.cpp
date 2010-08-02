#include "signal_processing_gui_command.h"
#include "../gui_helper_functions.h"
#include "../processed_signal_channel_manager.h"
#include "gui/progress_bar.h"

namespace SigViewer_
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
    processed_channel_manager->setXAxisUnitLabel(channel_manager->getXAxisUnitLabel());
    ChannelID new_channel_id = 0;
    QList<EventID> events (event_manager->getEvents(event_dialog->getSelectedEventType ()));
    foreach (ChannelID channel_id, event_dialog->getSelectedChannels ())
    {
        std::list<QSharedPointer<DataBlock const> > data;

        foreach (EventID event_id, events)
        {
            QSharedPointer<SignalEvent const> event = event_manager->getEvent (event_id);
            QSharedPointer<DataBlock const> data_block = channel_manager->getData (channel_id,
                                                                                   event->getPosition(),
                                                                                   num_samples);
            if (!data_block.isNull())
                data.push_back (data_block);
        }

        QSharedPointer<DataBlock> mean = QSharedPointer<DataBlock> (new DataBlock (DataBlock::calculateMean (data)));
        QSharedPointer<DataBlock> standard_deviation = DataBlock::calculateStandardDeviation(data, *(mean.data()));

        processed_channel_manager->addExtraChannel (new_channel_id, standard_deviation, tr("Standard Deviation\n") + channel_manager->getChannelLabel(channel_id),
                                                    channel_manager->getChannelYUnitString(channel_id));
        new_channel_id++;
        processed_channel_manager->addChannel (new_channel_id, mean, channel_manager->getChannelLabel(channel_id), channel_manager->getChannelYUnitString(channel_id));
        new_channel_id++;
        //applicationContext()->getEventColorManager()->setChannelColor(stddev_id,
        //                                                              applicationContext()->getEventColorManager()->getChannelColor(channel_id));
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
    unsigned fft_samples = 1;
    while (fft_samples < num_samples)
        fft_samples *= 2;

    QSharedPointer<ProcessedSignalChannelManager> processed_channel_manager (new ProcessedSignalChannelManager(static_cast<float32>(fft_samples) / channel_manager->getSampleRate(),
                                                                                                               fft_samples / 2));
    processed_channel_manager->setXAxisUnitLabel ("Hz");
    QList<EventID> events (event_manager->getEvents(event_dialog->getSelectedEventType ()));

    ProgressBar::instance().initAndShow (event_dialog->getSelectedChannels ().size(), tr("Fourier Transformation"),
                                         applicationContext());
    foreach (ChannelID channel_id, event_dialog->getSelectedChannels ())
    {
        ProgressBar::instance().increaseValue (1, channel_manager->getChannelLabel(channel_id));
        std::list<QSharedPointer<DataBlock const> > data;

        foreach (EventID event_id, events)
        {
            QSharedPointer<SignalEvent const> event = event_manager->getEvent (event_id);
            QSharedPointer<DataBlock const> data_block = channel_manager->getData (channel_id,
                                                                                   event->getPosition(),
                                                                                   num_samples);
            if (!data_block.isNull())
                data.push_back (data_block->createPowerSpectrum ());
        }

        QSharedPointer<DataBlock> mean = QSharedPointer<DataBlock> (new DataBlock (DataBlock::calculateMean (data)));
        QString unit = QString("log(").append(channel_manager->getChannelYUnitString (channel_id))
                                     .append(QChar(0xb2))
                                     .append("/Hz)");
        processed_channel_manager->addChannel (channel_id, mean, channel_manager->getChannelLabel(channel_id),
                                               unit);
    }
    ProgressBar::instance().close();

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
