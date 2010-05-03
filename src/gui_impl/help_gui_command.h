#ifndef HELP_GUI_COMMAND_H
#define HELP_GUI_COMMAND_H

#include "../gui/gui_action_command.h"
#include "../gui/gui_action_factory_registrator.h"

namespace BioSig_
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

public slots:
    //-------------------------------------------------------------------------
    void showAboutDialog ();

private:
    static QString const ABOUT_;
    static QStringList const ACTIONS_;

    static GuiActionFactoryRegistrator registrator_;

};

} // namespace BioSig_

#endif // HELP_GUI_COMMAND_H
