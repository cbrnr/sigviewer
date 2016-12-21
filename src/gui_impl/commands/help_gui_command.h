// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef HELP_GUI_COMMAND_H
#define HELP_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"

namespace sigviewer
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

}

#endif // HELP_GUI_COMMAND_H
