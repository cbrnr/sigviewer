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


#ifndef SAVE_GUI_COMMAND_H
#define SAVE_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"

namespace SigViewer_
{

//-----------------------------------------------------------------------------
/// SaveGuiCommand
///
/// command for saving a file and converting to other file format
class SaveGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    SaveGuiCommand ();

    //-------------------------------------------------------------------------
    virtual void init ();

public slots:
    //-------------------------------------------------------------------------
    void saveAs ();

    //-------------------------------------------------------------------------
    void save ();

    //-------------------------------------------------------------------------
    void exportToPNG ();

    //-------------------------------------------------------------------------
    void exportToGDF ();

    //-------------------------------------------------------------------------
    void exportEvents ();

protected:
    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness ();


private:
    //-------------------------------------------------------------------------


    static QString const SAVE_AS_;
    static QString const SAVE_;
    static QString const EXPORT_TO_PNG_;
    static QString const EXPORT_TO_GDF_;
    static QString const EXPORT_EVENTS_;
    static QStringList const ACTIONS_;

    static GuiActionFactoryRegistrator registrator_;
};

}

#endif // SAVE_GUI_COMMAND_H
