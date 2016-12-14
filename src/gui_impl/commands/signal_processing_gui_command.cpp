#include "signal_processing_gui_command.h"
#include "gui_impl/gui_helper_functions.h"
#include "gui_impl/processed_signal_channel_manager.h"
#include "gui/progress_bar.h"
#include "base/fixed_data_block.h"

#include <QMessageBox>

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
    ChannelManager const& channel_manager = currentFileContext()->getChannelManager();
    QSharedPointer<EventManager> event_manager = currentFileContext()->getEventManager();
    QSharedPointer<EventTimeSelectionDialog> event_dialog = getFinishedEventTimeSelectionDialog();
    if (event_dialog.isNull())
        return;

    unsigned num_samples = channel_manager.getSampleRate() * event_dialog->getLengthInSeconds ();
    unsigned samples_before = channel_manager.getSampleRate() * event_dialog->getSecondsBeforeEvent ();

    ProcessedSignalChannelManager* processed_channel_manager (new ProcessedSignalChannelManager(channel_manager.getSampleRate(),
                                                                                                               num_samples, currentFileContext().data()));
    processed_channel_manager->setXAxisUnitLabel(channel_manager.getXAxisUnitLabel());
    ChannelID new_channel_id = 0;
    QList<EventID> events (event_manager->getEvents(event_dialog->getSelectedEventType ()));
    foreach (ChannelID channel_id, event_dialog->getSelectedChannels ())
    {
        std::list<QSharedPointer<DataBlock const> > data;

        foreach (EventID event_id, events)
        {
            QSharedPointer<SignalEvent const> event = event_manager->getEvent (event_id);

            if (event->getPosition () < samples_before)
            {
                QMessageBox::warning (0, "Warning", QString("Event at ").append(QString::number(event->getPositionInSec())).append("s will be ignored! (because no data can be added in front of this event)"));
                continue;
            }

            QSharedPointer<DataBlock const> data_block = channel_manager.getData (channel_id,
                                                                                   event->getPosition() - samples_before,
                                                                                   num_samples);
            if (!data_block.isNull())
                data.push_back (data_block);
        }

        QSharedPointer<DataBlock> mean = FixedDataBlock::calculateMean (data);
        QSharedPointer<DataBlock> standard_deviation = FixedDataBlock::calculateStandardDeviation (data, mean);

        processed_channel_manager->addExtraChannel (new_channel_id, standard_deviation, tr("Standard Deviation\n") + channel_manager.getChannelLabel(channel_id),
                                                    channel_manager.getChannelYUnitString(channel_id));
        new_channel_id++;
        processed_channel_manager->addChannel (new_channel_id, mean, channel_manager.getChannelLabel(channel_id), channel_manager.getChannelYUnitString(channel_id));
        new_channel_id++;
        //applicationContext()->getEventColorManager()->setChannelColor(stddev_id,
        //                                                              applicationContext()->getEventColorManager()->getChannelColor(channel_id));
    }

    createVisualisation (tr("Mean"), *processed_channel_manager);
}


//-------------------------------------------------------------------------
void SignalProcessingGuiCommand::calculatePowerSpectrum ()
{
    ChannelManager const& channel_manager = currentFileContext()->getChannelManager();
    QSharedPointer<EventManager> event_manager = currentFileContext()->getEventManager();
    QSharedPointer<EventTimeSelectionDialog> event_dialog = getFinishedEventTimeSelectionDialog();
    if (event_dialog.isNull())
        return;

    unsigned num_samples = channel_manager.getSampleRate() * event_dialog->getLengthInSeconds ();
    unsigned samples_before = channel_manager.getSampleRate() * event_dialog->getSecondsBeforeEvent ();

    unsigned fft_samples = 1;
    while (fft_samples < num_samples)
        fft_samples *= 2;

    ProcessedSignalChannelManager* processed_channel_manager (new ProcessedSignalChannelManager(static_cast<float32>(fft_samples) / channel_manager.getSampleRate(),
                                                                                                               fft_samples / 2, currentFileContext().data()));
    processed_channel_manager->setXAxisUnitLabel ("Hz");
    QList<EventID> events (event_manager->getEvents(event_dialog->getSelectedEventType ()));

    ProgressBar::instance().initAndShow (event_dialog->getSelectedChannels ().size(), tr("Fourier Transformation"),
                                         applicationContext());
    foreach (ChannelID channel_id, event_dialog->getSelectedChannels ())
    {
        ProgressBar::instance().increaseValue (1, channel_manager.getChannelLabel(channel_id));
        std::list<QSharedPointer<DataBlock const> > data;

        foreach (EventID event_id, events)
        {
            QSharedPointer<SignalEvent const> event = event_manager->getEvent (event_id);
            if (event->getPosition () < samples_before)
            {
                QMessageBox::warning (0, "Warning", QString("Event at ").append(QString::number(event->getPositionInSec())).append("s will be ignored! (because no data can be added in front of this event)"));
                continue;
            }
            QSharedPointer<DataBlock const> data_block = channel_manager.getData (channel_id,
                                                                                   event->getPosition() - samples_before,
                                                                                   num_samples);
            if (!data_block.isNull())
                data.push_back (FixedDataBlock::createPowerSpectrum(data_block));
        }

        QSharedPointer<DataBlock> mean = FixedDataBlock::calculateMean (data);
        QString unit = QString("log(").append(channel_manager.getChannelYUnitString (channel_id))
                                     .append(QChar(0xb2))
                                     .append("/Hz)");
        processed_channel_manager->addChannel (channel_id, mean, channel_manager.getChannelLabel(channel_id),
                                               unit);
    }
    ProgressBar::instance().close();

    createVisualisation (tr("Power Spectrum"), *processed_channel_manager);

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
void SignalProcessingGuiCommand::createVisualisation (QString const& title, ChannelManager const& channel_manager)
{
    QSharedPointer<SignalVisualisationModel> signal_visualisation_model =
            applicationContext()->getMainWindowModel()->createSignalVisualisation (title, channel_manager);

    signal_visualisation_model->setShownChannels (channel_manager.getChannels());
    signal_visualisation_model->update();
}


}
