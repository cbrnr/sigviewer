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


#ifndef ZOOM_GUI_COMMAND_H
#define ZOOM_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"

namespace SigViewer_
{

class ZoomGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    ZoomGuiCommand ();

    virtual void init ();

protected slots:
    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness ();

private slots:
    //-------------------------------------------------------------------------
    void goTo ();

    //-------------------------------------------------------------------------
    void zoomInHorizontal ();

    //-------------------------------------------------------------------------
    void zoomOutHorizontal ();

    //-------------------------------------------------------------------------
    void zoomInVertical ();

    //-------------------------------------------------------------------------
    void zoomOutVertical ();

    //-------------------------------------------------------------------------
    void autoZoomVertical ();

    //-------------------------------------------------------------------------
    void scaleXAxis ();

    //-------------------------------------------------------------------------
    void setChannelsPerPage ();

private:
    //-------------------------------------------------------------------------
    unsigned maxChannelHeight ();

    //-------------------------------------------------------------------------
    unsigned minChannelHeight ();

    //-------------------------------------------------------------------------
    float32 maxPixelPerSample ();

    //-------------------------------------------------------------------------
    float32 minPixelPerSample ();

    static int const ZOOM_FACTOR_ = 2;
    static int const MAX_HORIZONTAL_ZOOM_IN_ = 32;

    static QString const GOTO_;
    static QString const ZOOM_IN_VERTICAL_;
    static QString const ZOOM_OUT_VERTICAL_;
    static QString const ZOOM_IN_HORIZONTAL_;
    static QString const ZOOM_OUT_HORIZONTAL_;
    static QString const SCALE_X_AXIS_;
    static QString const CHANNEL_PER_PAGE_;
    static QString const AUTO_ZOOM_VERTICAL_;
    static QStringList const ACTIONS_;

    static GuiActionFactoryRegistrator registrator_;
};

}

#endif // ZOOM_GUI_COMMAND_H
