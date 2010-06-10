#include "adapt_channel_view_gui_command.h"
#include "../gui_helper_functions.h"
#include "../dialogs/scale_channel_dialog.h"

#include <QColorDialog>

namespace BioSig_
{

//-----------------------------------------------------------------------------
QString const AdaptChannelViewGuiCommand::CHANNELS_ = "Channels...";
QString const AdaptChannelViewGuiCommand::CHANGE_COLOR_ = "Change Color...";
QString const AdaptChannelViewGuiCommand::SCALE_ = "Scale...";
QString const AdaptChannelViewGuiCommand::HIDE_ = "Hide Channel";
QString const AdaptChannelViewGuiCommand::AUTO_SCALE_ALL_ = "Auto Scale All";
QString const AdaptChannelViewGuiCommand::SET_AUTO_SCALE_MAX_TO_MAX_ = "Zero Line Centered";
QString const AdaptChannelViewGuiCommand::SET_AUTO_SCALE_MIN_TO_MAX_ = "Zero Line Fitted";
QStringList const AdaptChannelViewGuiCommand::ACTIONS_ = QStringList() <<
                                                         AdaptChannelViewGuiCommand::CHANNELS_ <<
                                                         AdaptChannelViewGuiCommand::AUTO_SCALE_ALL_  <<
                                                         AdaptChannelViewGuiCommand::SET_AUTO_SCALE_MAX_TO_MAX_ <<
                                                         AdaptChannelViewGuiCommand::SET_AUTO_SCALE_MIN_TO_MAX_ <<
                                                         AdaptChannelViewGuiCommand::CHANGE_COLOR_ <<
                                                         AdaptChannelViewGuiCommand::SCALE_ <<
                                                         AdaptChannelViewGuiCommand::HIDE_;

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
    resetActionTriggerSlot (CHANGE_COLOR_, SLOT(changeColor()));
    resetActionTriggerSlot (SCALE_, SLOT(scale()));
    resetActionTriggerSlot (HIDE_, SLOT(hide()));

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
    std::set<ChannelID> previous_shown_channels = sv_model->getShownChannels ();
    std::set<ChannelID> new_shown_channels = GuiHelper::selectChannels (sv_model->getChannelManager(),
                                                                        applicationContext()->getEventColorManager(),
                                                                        "",
                                                                        sv_model);
    if (previous_shown_channels != new_shown_channels)
    {
        sv_model->setShownChannels (new_shown_channels);
        sv_model->update ();
    }
}


//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::evaluateEnabledness ()
{
    disableIfNoFileIsOpened (ACTIONS_);
    if (!currentVisModel().isNull())
    {
        getQAction (SET_AUTO_SCALE_MAX_TO_MAX_)->setChecked (currentVisModel()->getAutoScaleMode()
                                                             == MAX_TO_MAX);
        getQAction (SET_AUTO_SCALE_MIN_TO_MAX_)->setChecked (currentVisModel()->getAutoScaleMode()
                                                             == MIN_TO_MAX);
    }
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::changeColor ()
{
    if (currentVisModel().isNull())
        return;
    ChannelID channel = currentVisModel()->getSelectedChannel();
    if (channel == UNDEFINED_CHANNEL)
        return;
    QSharedPointer<ColorManager> color_manager = applicationContext()->getEventColorManager();
    QColor old_color = color_manager->getChannelColor(channel);
    QColorDialog color_dialog (old_color);

    if (color_dialog.exec() == QDialog::Accepted)
    {
        color_manager->setChannelColor (channel, color_dialog.selectedColor());
        color_manager->saveSettings ();
    }
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::scale ()
{
    ChannelID channel = currentVisModel()->getSelectedChannel();
    if (channel == UNDEFINED_CHANNEL)
        return;
    ScaleChannelDialog scale_dialog (channel, currentVisModel()->getShownChannels(),
                                     currentVisModel()->getChannelManager());
    if (scale_dialog.exec() == QDialog::Accepted)
    {
        currentVisModel()->scaleChannel (scale_dialog.channel(),
                                         scale_dialog.lowerValue(),
                                         scale_dialog.upperValue());
    }
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::hide ()
{
    ChannelID channel = currentVisModel()->getSelectedChannel();
    if (channel == UNDEFINED_CHANNEL)
        return;

    std::set<ChannelID> shown_channels = currentVisModel()->getShownChannels();
    shown_channels.erase (channel);
    currentVisModel()->setShownChannels (shown_channels);
    currentVisModel()->update();
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::autoScaleAll ()
{
    currentVisModel()->scaleChannel(UNDEFINED_CHANNEL);
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::setScaleModeZeroCentered ()
{
    currentVisModel()->setAutoScaleMode (MAX_TO_MAX);
    currentVisModel()->scaleChannel(UNDEFINED_CHANNEL);
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::setScaleModeZeroFitted ()
{
    currentVisModel()->setAutoScaleMode (MIN_TO_MAX);
    currentVisModel()->scaleChannel(UNDEFINED_CHANNEL);
}




}
