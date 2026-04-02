// © SigViewer developers
//
// License: GPL-3.0

#ifndef HELP_GUI_COMMAND_H
#define HELP_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"

namespace sigviewer {

class HelpGuiCommand : public GuiActionCommand {
    Q_OBJECT
   public:
    //-------------------------------------------------------------------------
    HelpGuiCommand();

    //-------------------------------------------------------------------------
    virtual ~HelpGuiCommand() {}

    //-------------------------------------------------------------------------
    virtual void init();

   private slots:
    //-------------------------------------------------------------------------
    void showAboutDialog();

   private:
    static QString const ABOUT_();
    static QStringList const ACTIONS_();

    static GuiActionFactoryRegistrator registrator_;

    QDialog* dialog_;
};

}  // namespace sigviewer

#endif  // HELP_GUI_COMMAND_H
