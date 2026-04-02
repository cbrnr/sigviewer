// © SigViewer developers
//
// License: GPL-3.0

#ifndef VIEW_OPTIONS_GUI_COMMAND_H
#define VIEW_OPTIONS_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"

namespace sigviewer {

//-----------------------------------------------------------------------------
class AdaptChannelViewGuiCommand : public GuiActionCommand {
    Q_OBJECT
   public:
    //-------------------------------------------------------------------------
    AdaptChannelViewGuiCommand();

    //-------------------------------------------------------------------------
    virtual ~AdaptChannelViewGuiCommand() {}

    //-------------------------------------------------------------------------
    virtual void init();

   protected:
    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness();

   private slots:
    //-------------------------------------------------------------------------
    void selectShownChannels();

    //-------------------------------------------------------------------------
    void changeColor();

    //-------------------------------------------------------------------------
    void scale();

    //-------------------------------------------------------------------------
    void hide();

    //-------------------------------------------------------------------------
    void toggleAnimations();

    //-------------------------------------------------------------------------
    void setAnimationDuration();

   private:
    static QString const CHANNELS_();
    static QString const CHANGE_COLOR_();
    static QString const SCALE_();
    static QString const HIDE_();
    static QString const ANIMATIONS_();
    static QString const SET_ANIMATION_DURATION_();
    static QStringList const ACTIONS_();

    static GuiActionFactoryRegistrator registrator_;
};

}  // namespace sigviewer

#endif  // VIEW_OPTIONS_GUI_COMMAND_H
