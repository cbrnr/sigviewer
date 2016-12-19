// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


#ifndef VIEW_OPTIONS_GUI_COMMAND_H
#define VIEW_OPTIONS_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"

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
