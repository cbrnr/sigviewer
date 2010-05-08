#include "adapt_channel_view_gui_command.h"
#include "gui_helper_functions.h"
#include "../application_context.h"

#include <QProgressDialog>

namespace BioSig_
{

//-----------------------------------------------------------------------------
QString const AdaptChannelViewGuiCommand::CHANNELS_ = "Channels...";
QString const AdaptChannelViewGuiCommand::AUTO_SCALE_ALL_ = "Auto Scale All";
QString const AdaptChannelViewGuiCommand::SET_AUTO_SCALE_MAX_TO_MAX_ = "Zero Line Centered";
QString const AdaptChannelViewGuiCommand::SET_AUTO_SCALE_MIN_TO_MAX_ = "Zero Line Fitted";
QStringList const AdaptChannelViewGuiCommand::ACTIONS_ = QStringList() <<
                                                         AdaptChannelViewGuiCommand::CHANNELS_ <<
                                                         AdaptChannelViewGuiCommand::AUTO_SCALE_ALL_  <<
                                                         AdaptChannelViewGuiCommand::SET_AUTO_SCALE_MAX_TO_MAX_ <<
                                                         AdaptChannelViewGuiCommand::SET_AUTO_SCALE_MIN_TO_MAX_;

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator registrator_ ("Adapt Channel View",
                                          QSharedPointer<AdaptChannelViewGuiCommand> (new AdaptChannelViewGuiCommand));

//-----------------------------------------------------------------------------
AdaptChannelViewGuiCommand::AdaptChannelViewGuiCommand ()
    : GuiActionCommand (ACTIONS_)
{
    // nothing to do here
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::init ()
{
    setIcon (CHANNELS_, QIcon(":/images/channels_22x22.png"));
    setIcon (AUTO_SCALE_ALL_, QIcon(":/images/icons/autoscale.png"));
    resetActionTriggerSlot (CHANNELS_, SLOT(selectShownChannels()));
    resetActionTriggerSlot (AUTO_SCALE_ALL_, SLOT(autoScaleAll()));

    QActionGroup* scale_mode_action_group = new QActionGroup (this);
    scale_mode_action_group->setExclusive(true);
    scale_mode_action_group->addAction (getQAction(SET_AUTO_SCALE_MAX_TO_MAX_));
    scale_mode_action_group->addAction (getQAction(SET_AUTO_SCALE_MIN_TO_MAX_));
    getQAction(SET_AUTO_SCALE_MAX_TO_MAX_)->setCheckable(true);
    getQAction(SET_AUTO_SCALE_MIN_TO_MAX_)->setCheckable(true);

    resetActionTriggerSlot (SET_AUTO_SCALE_MAX_TO_MAX_, SLOT(setScaleModeZeroCentered()));
    resetActionTriggerSlot (SET_AUTO_SCALE_MIN_TO_MAX_, SLOT(setScaleModeZeroFitted()));
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::selectShownChannels ()
{
    QSharedPointer<SignalVisualisationModel> sv_model = currentVisModel ();
    QSharedPointer<FileContext> file_ctx = ApplicationContext::getInstance()->getCurrentFileContext();
    std::set<ChannelID> previous_shown_channels = sv_model->getShownChannels ();
    std::set<ChannelID> new_shown_channels = GuiHelper::selectChannels (sv_model->getChannelManager(),
                                                                        "",
                                                                        sv_model);
    if (previous_shown_channels != new_shown_channels)
    {
        sv_model->setShownChannels (new_shown_channels);
        sv_model->updateLayout ();
    }
}


//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::evaluateEnabledness ()
{
    disableIfNoFileIsOpened (ACTIONS_);
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::autoScaleAll ()
{
//    std::set<ChannelID> shown_channels = currentVisModel()->getShownChannels();
//
//    QProgressDialog progress_bar;
//    progress_bar.setMaximum (shown_channels.size());
//    progress_bar.setMinimum (0);
//    progress_bar.setModal (true);
//    progress_bar.setLabelText (tr("Auto Scaling..."));
//
//    foreach (ChannelID id, shown_channels)
//    {
//        currentVisModel()->autoScaleChannel (id);
//        progress_bar.setValue (progress_bar.value()+1);
//    }
//    progress_bar.setValue (progress_bar.maximum());
    currentVisModel()->autoScaleChannel(UNDEFINED_CHANNEL);
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::setScaleModeZeroCentered ()
{
    currentVisModel()->setAutoScaleMode (MAX_TO_MAX);
    currentVisModel()->autoScaleChannel(UNDEFINED_CHANNEL);
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::setScaleModeZeroFitted ()
{
    currentVisModel()->setAutoScaleMode (MIN_TO_MAX);
    currentVisModel()->autoScaleChannel(UNDEFINED_CHANNEL);
}




}
