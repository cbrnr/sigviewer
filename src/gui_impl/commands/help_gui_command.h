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


#ifndef HELP_GUI_COMMAND_H
#define HELP_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"

namespace SigViewer_
{

class HelpGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    HelpGuiCommand ();

    //-------------------------------------------------------------------------
    virtual ~HelpGuiCommand () {}

    //-------------------------------------------------------------------------
    virtual void init ();

private slots:
    //-------------------------------------------------------------------------
    void showAboutDialog ();

    //-------------------------------------------------------------------------
    void runTests ();

private:
    static QString const ABOUT_;
    static QString const RUN_TESTS_;
    static QStringList const ACTIONS_;

    static GuiActionFactoryRegistrator registrator_;

};

} // namespace SigViewer_

#endif // HELP_GUI_COMMAND_H
