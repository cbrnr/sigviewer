#include "adapt_channel_view_gui_command.h"
#include "gui_helper_functions.h"
#include "../application_context.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
QString const AdaptChannelViewGuiCommand::CHANNELS_ = "Channels...";
QString const AdaptChannelViewGuiCommand::AUTO_SCALE_ALL_ = "Auto Scale All";
QStringList const AdaptChannelViewGuiCommand::ACTIONS_ = QStringList() <<
                                                         AdaptChannelViewGuiCommand::CHANNELS_ <<
                                                         AdaptChannelViewGuiCommand::AUTO_SCALE_ALL_ ;

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
    bool file_open = (getApplicationState() == APP_STATE_FILE_OPEN);
    getQAction (CHANNELS_)->setEnabled (file_open);
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::autoScaleAll ()
{
    foreach (ChannelID id, currentVisModel()->getShownChannels())
        currentVisModel()->autoScaleChannel (id);
}



}
