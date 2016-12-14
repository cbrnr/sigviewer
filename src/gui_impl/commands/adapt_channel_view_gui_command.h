#ifndef VIEW_OPTIONS_GUI_COMMAND_H
#define VIEW_OPTIONS_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"

namespace SigViewer_
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

    static GuiActionFactoryRegistrator registrator_;
};

}

#endif // VIEW_OPTIONS_GUI_COMMAND_H
