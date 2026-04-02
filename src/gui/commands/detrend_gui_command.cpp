// © SigViewer developers
//
// License: GPL-3.0

#include "detrend_gui_command.h"

#include <QCoreApplication>
#include <QInputDialog>
#include <QSettings>

#include "gui/gui_helper_functions.h"
#include "gui/progress_bar.h"

namespace sigviewer {

//-----------------------------------------------------------------------------
namespace {

class DetrendGuiCommandFactory : public GuiActionCommandFactory {
   public:
    QSharedPointer<GuiActionCommand> createCommand() override {
        return QSharedPointer<DetrendGuiCommand>(new DetrendGuiCommand);
    }
};

}  // unnamed namespace

//-----------------------------------------------------------------------------
QString const DetrendGuiCommand::TOGGLE_DETREND_() {
    static QString value = tr("Remove Offset");
    return value;
}

QString const DetrendGuiCommand::SET_CUTOFF_() {
    static QString value = tr("Set High-Pass Cutoff...");
    return value;
}

QStringList const DetrendGuiCommand::ACTIONS_() {
    static QStringList result = {
        DetrendGuiCommand::TOGGLE_DETREND_(),
        DetrendGuiCommand::SET_CUTOFF_(),
    };
    return result;
}

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator DetrendGuiCommand::registrator_("Detrend",
    QSharedPointer<GuiActionCommandFactory>(new DetrendGuiCommandFactory));

//-----------------------------------------------------------------------------
DetrendGuiCommand::DetrendGuiCommand() : GuiActionCommand(ACTIONS_()) {}

//-----------------------------------------------------------------------------
void DetrendGuiCommand::init() {
    getQAction(TOGGLE_DETREND_())->setCheckable(true);
    getQAction(TOGGLE_DETREND_())->setIcon(QIcon::fromTheme("trending_down"));
    resetActionTriggerSlot(TOGGLE_DETREND_(), SLOT(toggleDetrend()));
    resetActionTriggerSlot(SET_CUTOFF_(), SLOT(setCutoff()));
}

//-----------------------------------------------------------------------------
void DetrendGuiCommand::evaluateEnabledness() {
    disableIfNoFileIsOpened(ACTIONS_());
    disableIfNoSignalIsVisualised(ACTIONS_());

    QSharedPointer<FileContext> ctx = currentFileContext();
    if (!ctx.isNull())
        getQAction(TOGGLE_DETREND_())->setChecked(ctx->isDetrendEnabled());
}

//-----------------------------------------------------------------------------
// Pre-compute all channels in the DetrendChannelManager so that the first
// render is instant.  Shows a progress dialog identical to the file-open one.
static void prewarmDetrend(QSharedPointer<FileContext> ctx,
    QSharedPointer<ApplicationContext> app_ctx) {
    ChannelManager& cm = ctx->getChannelManager();
    std::set<ChannelID> channels = cm.getChannels();
    int total = static_cast<int>(channels.size());
    ProgressBar::instance().initAndShow(total, QObject::tr("Applying filter..."), app_ctx);
    int done = 0;
    for (ChannelID id : channels) {
        ProgressBar::instance().increaseValue(1, cm.getChannelLabel(id));
        // Any getData call is enough to trigger and cache processedChannel().
        cm.getData(id, 0, 1);
        QCoreApplication::processEvents();
        ++done;
    }
    ProgressBar::instance().close();
}

//-----------------------------------------------------------------------------
void DetrendGuiCommand::toggleDetrend() {
    QSharedPointer<FileContext> ctx = currentFileContext();
    if (ctx.isNull()) return;

    bool new_state = !ctx->isDetrendEnabled();

    // When enabling, use the stored cutoff (persist across toggles).
    QSettings settings;
    double cutoff = settings.value("Detrend/cutoff_hz", 0.1).toDouble();

    bool rebuilt = ctx->setDetrendEnabled(new_state, cutoff);
    getQAction(TOGGLE_DETREND_())->setChecked(new_state);

    // Only show the progress dialog when the filter was actually (re)built.
    // If toggling back on with the same cutoff, the data cache is still valid.
    if (new_state && rebuilt) prewarmDetrend(ctx, applicationContext());

    QSharedPointer<SignalVisualisationModel> vis = ctx->getMainVisualisationModel();
    if (!vis.isNull()) {
        vis->update();
        vis->scaleChannel(UNDEFINED_CHANNEL);
    }
}

//-----------------------------------------------------------------------------
void DetrendGuiCommand::setCutoff() {
    QSharedPointer<FileContext> ctx = currentFileContext();
    if (ctx.isNull()) return;

    QSettings settings;
    double current_cutoff = settings.value("Detrend/cutoff_hz", 0.1).toDouble();

    bool ok = false;
    double new_cutoff = QInputDialog::getDouble(nullptr,
        tr("High-Pass Cutoff"),
        tr("Enter high-pass cutoff frequency in Hz\n"
           "(0 = remove mean only, no filtering):"),
        current_cutoff,
        0.0,      // min
        10000.0,  // max
        4,        // decimals
        &ok);

    if (!ok) return;

    settings.setValue("Detrend/cutoff_hz", new_cutoff);

    // Re-apply if detrend is currently active.
    if (ctx->isDetrendEnabled()) {
        bool rebuilt = ctx->setDetrendEnabled(true, new_cutoff);
        if (rebuilt) prewarmDetrend(ctx, applicationContext());

        QSharedPointer<SignalVisualisationModel> vis = ctx->getMainVisualisationModel();
        if (!vis.isNull()) {
            vis->update();
            vis->scaleChannel(UNDEFINED_CHANNEL);
        }
    }
}

}  // namespace sigviewer
