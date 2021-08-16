// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "adapt_channel_view_gui_command.h"
#include "gui_impl/gui_helper_functions.h"
#include "gui_impl/dialogs/scale_channel_dialog.h"

#include <QColorDialog>
#include <QSettings>
#include <QInputDialog>

namespace sigviewer
{

//-----------------------------------------------------------------------------

QString const AdaptChannelViewGuiCommand::CHANNELS_()
{
    static QString value = tr("Channels...");

    return value;
}

QString const AdaptChannelViewGuiCommand::CHANGE_COLOR_()
{
    static QString value = tr("Change Color...");

    return value;
}

QString const AdaptChannelViewGuiCommand::SCALE_()
{
    static QString value = tr("Scale...");

    return value;
}

QString const AdaptChannelViewGuiCommand::APPLY_SCALE_TO_OTHER_CHANNELS_()
{
    static QString value = tr("Apply Scale to Other Channels");

    return value;
}

QString const AdaptChannelViewGuiCommand::HIDE_()
{
    static QString value = tr("Hide Channel");

    return value;
}

QString const AdaptChannelViewGuiCommand::SCALE_ALL_()
{
    static QString value = tr("Scale All...");

    return value;
}

QString const AdaptChannelViewGuiCommand::SET_AUTO_SCALE_MAX_TO_MAX_()
{
    static QString value = tr("Zero Line Centered");

    return value;
}

QString const AdaptChannelViewGuiCommand::SET_AUTO_SCALE_MIN_TO_MAX_()
{
    static QString value = tr("Zero Line Fitted");

    return value;
}

QString const AdaptChannelViewGuiCommand::ANIMATIONS_()
{
    static QString value = tr("Animations");

    return value;
}

QString const AdaptChannelViewGuiCommand::SET_ANIMATION_DURATION_()
{
    static QString value = tr("Set Animation Duration");

    return value;
}

QStringList const AdaptChannelViewGuiCommand::ACTIONS_()
{
    static QStringList result = {
        AdaptChannelViewGuiCommand::CHANNELS_(),
        AdaptChannelViewGuiCommand::SCALE_ALL_(),
        AdaptChannelViewGuiCommand::SET_AUTO_SCALE_MAX_TO_MAX_(),
        AdaptChannelViewGuiCommand::SET_AUTO_SCALE_MIN_TO_MAX_(),
        AdaptChannelViewGuiCommand::CHANGE_COLOR_(),
        AdaptChannelViewGuiCommand::SCALE_(),
        AdaptChannelViewGuiCommand::HIDE_(),
        AdaptChannelViewGuiCommand::ANIMATIONS_(),
        AdaptChannelViewGuiCommand::SET_ANIMATION_DURATION_(),
    };

    return result;
}

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator registrator_ ("Adapt Channel View",
                                          QSharedPointer<AdaptChannelViewGuiCommand> (new AdaptChannelViewGuiCommand));

//-----------------------------------------------------------------------------
AdaptChannelViewGuiCommand::AdaptChannelViewGuiCommand ()
    : GuiActionCommand (ACTIONS_())
{
    // nothing to do here
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::init ()
{
    setIcon (CHANNELS_(), QIcon(":/images/ic_reorder_black_24dp.png"));
    setIcon (SCALE_ALL_(), QIcon(":/images/ic_autoscale_black_24dp.png"));
    resetActionTriggerSlot (CHANNELS_(), SLOT(selectShownChannels()));
    resetActionTriggerSlot (SCALE_ALL_(), SLOT(scaleAll()));
    resetActionTriggerSlot (CHANGE_COLOR_(), SLOT(changeColor()));
    resetActionTriggerSlot (SCALE_(), SLOT(scale()));
    resetActionTriggerSlot (HIDE_(), SLOT(hide()));
    setShortcut (CHANNELS_(), tr("Ctrl+C"));
    //setShortcut (SCALE_ALL_(), tr("Ctrl+A"));

    QActionGroup* scale_mode_action_group = new QActionGroup (this);
    scale_mode_action_group->setExclusive(true);
    scale_mode_action_group->addAction (getQAction(SET_AUTO_SCALE_MAX_TO_MAX_()));
    scale_mode_action_group->addAction (getQAction(SET_AUTO_SCALE_MIN_TO_MAX_()));
    getQAction(SET_AUTO_SCALE_MAX_TO_MAX_())->setCheckable(true);
    getQAction(SET_AUTO_SCALE_MIN_TO_MAX_())->setCheckable(true);
    getQAction(ANIMATIONS_())->setCheckable(true);
    QSettings settings;
    settings.beginGroup("Animations");
    getQAction(ANIMATIONS_())->setChecked (settings.value("activated", false).toBool());
    settings.endGroup();

    resetActionTriggerSlot (SET_AUTO_SCALE_MAX_TO_MAX_(), SLOT(setScaleModeZeroCentered()));
    resetActionTriggerSlot (SET_AUTO_SCALE_MIN_TO_MAX_(), SLOT(setScaleModeZeroFitted()));
    resetActionTriggerSlot (ANIMATIONS_(), SLOT(toggleAnimations()));
    resetActionTriggerSlot (SET_ANIMATION_DURATION_(), SLOT(setAnimationDuration()));
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::selectShownChannels ()
{
    QSharedPointer<SignalVisualisationModel> sv_model = currentVisModel ();
    std::set<ChannelID> previous_shown_channels = sv_model->getShownChannels ();
    std::set<ChannelID> new_shown_channels = GuiHelper::selectChannels (sv_model->getChannelManager(),
                                                                        applicationContext()->getEventColorManager(),
                                                                        QSharedPointer<BasicHeader>(0),
                                                                        sv_model);
    if (previous_shown_channels != new_shown_channels)
        sv_model->setShownChannels (new_shown_channels);
    sv_model->update ();
}


//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::evaluateEnabledness ()
{
    QStringList disabled_actions_if_no_file = ACTIONS_();
    disabled_actions_if_no_file.removeAll(ANIMATIONS_());
    disabled_actions_if_no_file.removeAll(SET_ANIMATION_DURATION_());
    disableIfNoFileIsOpened (disabled_actions_if_no_file);
    disableIfNoSignalIsVisualised (disabled_actions_if_no_file);

    if (!currentVisModel().isNull())
    {
        getQAction (SET_AUTO_SCALE_MAX_TO_MAX_())->setChecked (currentVisModel()->getAutoScaleMode()
                                                             == MAX_TO_MAX);
        getQAction (SET_AUTO_SCALE_MIN_TO_MAX_())->setChecked (currentVisModel()->getAutoScaleMode()
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
    color_dialog.setOption(QColorDialog::ShowAlphaChannel);

    if (color_dialog.exec() == QDialog::Accepted)
    {
        color_manager->setChannelColor (channel, color_dialog.selectedColor());
        color_manager->saveSettings ();
        currentVisModel()->update();
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
        currentVisModel()->scaleChannel (channel,
                                         scale_dialog.lowerValue(),
                                         scale_dialog.upperValue());
    }
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::applyScaleToOtherChannels ()
{
    ChannelID channel = currentVisModel()->getSelectedChannel();
    if (channel == UNDEFINED_CHANNEL)
        return;

    // TODO: implement!
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
void AdaptChannelViewGuiCommand::scaleAll ()
{
    ScaleChannelDialog scale_dialog (UNDEFINED_CHANNEL, currentVisModel()->getShownChannels(),
                                     currentVisModel()->getChannelManager());
    if (scale_dialog.exec() == QDialog::Accepted)
    {
        if (scale_dialog.autoScaling())
        {
            currentVisModel()->setAutoScaleMode (MIN_TO_MAX);
            currentVisModel()->scaleChannel(UNDEFINED_CHANNEL);
        }
        else
        {
            currentVisModel()->scaleChannel
                    (UNDEFINED_CHANNEL, scale_dialog.lowerValue(), scale_dialog.upperValue());
        }
    }
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

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::toggleAnimations ()
{
    QSettings settings;
    settings.beginGroup ("Animations");
    settings.setValue ("activated", getQAction(ANIMATIONS_())->isChecked ());
    settings.endGroup ();
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::setAnimationDuration ()
{
    QSettings settings;
    settings.beginGroup ("Animations");

    bool ok = false;
    int new_duration = QInputDialog::getInt
            (0, tr("Set Animation Duration"), tr("Milliseconds:"),
             settings.value("duration", 200).toInt(), 0, 1000, 100, &ok);
    if (ok)
        settings.setValue ("duration", new_duration);
    settings.endGroup ();
}

}
