// © SigViewer developers
//
// License: GPL-3.0

#include "adapt_channel_view_gui_command.h"

#include <QColorDialog>
#include <QInputDialog>
#include <QSettings>
#include <cmath>

#include "gui/dialogs/scale_channel_dialog.h"
#include "gui/gui_helper_functions.h"

namespace sigviewer {

//-----------------------------------------------------------------------------

namespace {

class AdaptChannelViewGuiCommandFactory : public GuiActionCommandFactory {
   public:
    QSharedPointer<GuiActionCommand> createCommand() override {
        return QSharedPointer<AdaptChannelViewGuiCommand>(new AdaptChannelViewGuiCommand);
    }
};

}  // unnamed namespace

QString const AdaptChannelViewGuiCommand::CHANNELS_() {
    static QString value = tr("Channels...");

    return value;
}

QString const AdaptChannelViewGuiCommand::CHANGE_COLOR_() {
    static QString value = tr("Change Color...");

    return value;
}

QString const AdaptChannelViewGuiCommand::SCALE_() {
    static QString value = tr("Scale...");

    return value;
}

QString const AdaptChannelViewGuiCommand::HIDE_() {
    static QString value = tr("Hide Channel");

    return value;
}

QString const AdaptChannelViewGuiCommand::ANIMATIONS_() {
    static QString value = tr("Animations");

    return value;
}

QString const AdaptChannelViewGuiCommand::SET_ANIMATION_DURATION_() {
    static QString value = tr("Set Animation Duration");

    return value;
}

QStringList const AdaptChannelViewGuiCommand::ACTIONS_() {
    static QStringList result = {
        AdaptChannelViewGuiCommand::CHANNELS_(),
        AdaptChannelViewGuiCommand::SCALE_(),
        AdaptChannelViewGuiCommand::CHANGE_COLOR_(),
        AdaptChannelViewGuiCommand::HIDE_(),
        AdaptChannelViewGuiCommand::ANIMATIONS_(),
        AdaptChannelViewGuiCommand::SET_ANIMATION_DURATION_(),
    };

    return result;
}

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator registrator_("Adapt Channel View",
    QSharedPointer<AdaptChannelViewGuiCommandFactory>(new AdaptChannelViewGuiCommandFactory));

//-----------------------------------------------------------------------------
AdaptChannelViewGuiCommand::AdaptChannelViewGuiCommand()
    : GuiActionCommand(ACTIONS_()) {
    // nothing to do here
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::init() {
    setIcon(CHANNELS_(), QIcon::fromTheme("reorder"));
    setIcon(SCALE_(), QIcon::fromTheme("fit_page_height"));
    resetActionTriggerSlot(CHANNELS_(), SLOT(selectShownChannels()));
    resetActionTriggerSlot(SCALE_(), SLOT(scale()));
    resetActionTriggerSlot(CHANGE_COLOR_(), SLOT(changeColor()));
    resetActionTriggerSlot(HIDE_(), SLOT(hide()));
    setShortcut(CHANNELS_(), tr("Ctrl+C"));

    getQAction(ANIMATIONS_())->setCheckable(true);
    QSettings settings;
    settings.beginGroup("Animations");
    getQAction(ANIMATIONS_())->setChecked(settings.value("activated", false).toBool());
    settings.endGroup();

    resetActionTriggerSlot(ANIMATIONS_(), SLOT(toggleAnimations()));
    resetActionTriggerSlot(SET_ANIMATION_DURATION_(), SLOT(setAnimationDuration()));
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::selectShownChannels() {
    QSharedPointer<SignalVisualisationModel> sv_model = currentVisModel();
    std::set<ChannelID> previous_shown_channels = sv_model->getShownChannels();
    std::set<ChannelID> new_shown_channels =
        GuiHelper::selectChannels(sv_model->getChannelManager(),
            applicationContext()->getEventColorManager(),
            QSharedPointer<BasicHeader>(0),
            sv_model);
    if (previous_shown_channels != new_shown_channels)
        sv_model->setShownChannels(new_shown_channels);
    sv_model->update();
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::evaluateEnabledness() {
    QStringList disabled_actions_if_no_file = ACTIONS_();
    disabled_actions_if_no_file.removeAll(ANIMATIONS_());
    disabled_actions_if_no_file.removeAll(SET_ANIMATION_DURATION_());
    disableIfNoFileIsOpened(disabled_actions_if_no_file);
    disableIfNoSignalIsVisualised(disabled_actions_if_no_file);
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::changeColor() {
    if (currentVisModel().isNull()) return;
    ChannelID channel = currentVisModel()->getSelectedChannel();
    if (channel == UNDEFINED_CHANNEL) return;
    QSharedPointer<ColorManager> color_manager = applicationContext()->getEventColorManager();
    QColor old_color = color_manager->getChannelColor(channel);
    QColorDialog color_dialog(old_color);
    color_dialog.setOption(QColorDialog::ShowAlphaChannel);

    if (color_dialog.exec() == QDialog::Accepted) {
        color_manager->setChannelColor(channel, color_dialog.selectedColor());
        color_manager->saveSettings();
        currentVisModel()->update();
    }
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::scale() {
    QSharedPointer<SignalVisualisationModel> sv_model = currentVisModel();
    if (sv_model.isNull()) return;

    ChannelID selected = sv_model->getSelectedChannel();
    std::set<ChannelID> preselect;
    if (selected != UNDEFINED_CHANNEL) preselect = {selected};

    ScaleChannelDialog dialog(sv_model->getShownChannels(), sv_model->getChannelManager(), preselect);
    if (dialog.exec() != QDialog::Accepted) return;

    std::set<ChannelID> channels = dialog.selectedChannels();
    if (channels.empty()) return;

    switch (dialog.scalingMode()) {
        case ScaleChannelDialog::AUTO_PER_CHANNEL: {
            ScaleMode mode = dialog.symmetric() ? MAX_TO_MAX : MIN_TO_MAX;
            sv_model->setAutoScaleMode(mode);
            for (ChannelID ch : channels) sv_model->scaleChannel(ch);
            break;
        }
        case ScaleChannelDialog::AUTO_SHARED_RANGE: {
            ChannelManager const& cm = sv_model->getChannelManager();
            float64 lo = cm.getMinValue(channels);
            float64 hi = cm.getMaxValue(channels);
            if (dialog.symmetric()) {
                double absMax = std::max(std::fabs(lo), std::fabs(hi));
                lo = -absMax;
                hi = absMax;
            }
            for (ChannelID ch : channels)
                sv_model->scaleChannel(ch, static_cast<float32>(lo), static_cast<float32>(hi));
            break;
        }
        case ScaleChannelDialog::MANUAL:
            for (ChannelID ch : channels)
                sv_model->scaleChannel(ch,
                    static_cast<float32>(dialog.lowerValue()),
                    static_cast<float32>(dialog.upperValue()));
            break;
    }
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::hide() {
    ChannelID channel = currentVisModel()->getSelectedChannel();
    if (channel == UNDEFINED_CHANNEL) return;

    std::set<ChannelID> shown_channels = currentVisModel()->getShownChannels();
    shown_channels.erase(channel);
    currentVisModel()->setShownChannels(shown_channels);
    currentVisModel()->update();
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::toggleAnimations() {
    QSettings settings;
    settings.beginGroup("Animations");
    settings.setValue("activated", getQAction(ANIMATIONS_())->isChecked());
    settings.endGroup();
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::setAnimationDuration() {
    QSettings settings;
    settings.beginGroup("Animations");

    bool ok = false;
    int new_duration = QInputDialog::getInt(0,
        tr("Set Animation Duration"),
        tr("Milliseconds:"),
        settings.value("duration", 200).toInt(),
        0,
        1000,
        100,
        &ok);
    if (ok) settings.setValue("duration", new_duration);
    settings.endGroup();
}

}  // namespace sigviewer
