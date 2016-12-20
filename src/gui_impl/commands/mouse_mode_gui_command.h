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


#ifndef MOUSE_MODE_GUI_COMMAND_H
#define MOUSE_MODE_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"
#include "gui/signal_visualisation_modes.h"

#include <QStringList>
#include <QActionGroup>

namespace sigviewer
{

class MouseModeGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    MouseModeGuiCommand ();
    virtual ~MouseModeGuiCommand ();

    virtual void init ();

public slots:
    //-------------------------------------------------------------------------
    virtual void trigger (QString const& action_name);

protected:
    //-------------------------------------------------------------------------
    virtual void applicationStateChanged ();

    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness ();


private:
    static QString const NEW_MODE_TEXT_;
    static QString const POINTER_MODE_TEXT_;
    static QString const HAND_MODE_TEXT_;
    static QString const VIEW_OPTIONS_TEXT_;
    static QString const INFO_MODE_TEXT_;
    static QStringList const MODE_TEXTS_;

    static GuiActionFactoryRegistrator registrator_;

    QMap<QString, SignalVisualisationMode> action_to_mode_map_;
};

}

#endif // MOUSE_MODE_GUI_COMMAND_H
