// © SigViewer developers
//
// License: GPL-3.0

#ifndef DETREND_GUI_COMMAND_H
#define DETREND_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"

namespace sigviewer {

/// DetrendGuiCommand
///
/// Provides two menu actions under Tools:
///  - "Remove Offset"            – toggle (checkable)
///  - "Set High-Pass Cutoff..."  – opens an input dialog to configure the
///                                  cutoff frequency in Hz (0 = mean-only)
class DetrendGuiCommand : public GuiActionCommand {
    Q_OBJECT
   public:
    DetrendGuiCommand();
    virtual ~DetrendGuiCommand() {}

    virtual void init() override;

   protected:
    virtual void evaluateEnabledness() override;

   private slots:
    void toggleDetrend();
    void setCutoff();

   private:
    static QString const TOGGLE_DETREND_();
    static QString const SET_CUTOFF_();
    static QStringList const ACTIONS_();

    static GuiActionFactoryRegistrator registrator_;
};

}  // namespace sigviewer

#endif  // DETREND_GUI_COMMAND_H
