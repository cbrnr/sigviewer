// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef VIEW_OPTIONS_GUI_COMMAND_H
#define VIEW_OPTIONS_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"
#include "gui_impl/dialogs/hotkey_dialog.h"

namespace sigviewer
{

//-----------------------------------------------------------------------------
class AdaptChannelViewGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    AdaptChannelViewGuiCommand ();

    //-------------------------------------------------------------------------
    virtual ~AdaptChannelViewGuiCommand () {}

    //-------------------------------------------------------------------------
    virtual void init ();

protected:
    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness ();

private slots:
    //-------------------------------------------------------------------------
    void selectShownChannels ();

    //-------------------------------------------------------------------------
    void changeColor ();

    //-------------------------------------------------------------------------
    void scale ();

    //-------------------------------------------------------------------------
    void applyScaleToOtherChannels ();

    //-------------------------------------------------------------------------
    void hide ();

    //-------------------------------------------------------------------------
    void scaleAll ();

    //-------------------------------------------------------------------------
    void setScaleModeZeroCentered ();

    //-------------------------------------------------------------------------
    void setScaleModeZeroFitted ();

    //-------------------------------------------------------------------------
    void toggleAnimations ();

    //-------------------------------------------------------------------------
    void setAnimationDuration ();

    //-------------------------------------------------------------------------
    void showHotkeyDialog ();

private:
    static QString const CHANNELS_;
    static QString const CHANGE_COLOR_;
    static QString const SCALE_;
    static QString const APPLY_SCALE_TO_OTHER_CHANNELS_;
    static QString const HIDE_;
    static QString const SCALE_ALL_;
    static QString const SET_AUTO_SCALE_MAX_TO_MAX_;
    static QString const SET_AUTO_SCALE_MIN_TO_MAX_;
    static QString const ANIMATIONS_;
    static QString const SET_ANIMATION_DURATION_;
    static QStringList const ACTIONS_;
    static QString const HOTKEYS_;

    static GuiActionFactoryRegistrator registrator_;

    HotkeyDialog* hotkey_dialog_;
};

}

#endif // VIEW_OPTIONS_GUI_COMMAND_H
