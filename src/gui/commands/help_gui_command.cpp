// © SigViewer developers
//
// License: GPL-3.0


#include "help_gui_command.h"
#include "ui_about_dialog.h"

#include <QMessageBox>


namespace sigviewer
{

//-----------------------------------------------------------------------------
namespace {

class HelpGuiCommandFactory: public GuiActionCommandFactory
{
public:
    QSharedPointer<GuiActionCommand> createCommand() override
    {
        return QSharedPointer<HelpGuiCommand> (new HelpGuiCommand);
    }
};

} // unnamed namespace

QString const HelpGuiCommand::ABOUT_()
{
    static QString value = tr("About");
    return value;
}

QStringList const HelpGuiCommand::ACTIONS_()
{
    static QStringList result = {HelpGuiCommand::ABOUT_()};
    return result;
}

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator HelpGuiCommand::registrator_ ("Help",
                                                          QSharedPointer<HelpGuiCommandFactory>(new HelpGuiCommandFactory));

//-----------------------------------------------------------------------------
HelpGuiCommand::HelpGuiCommand ()
    : GuiActionCommand (ACTIONS_())
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void HelpGuiCommand::init ()
{
    resetActionTriggerSlot (ABOUT_(), SLOT(showAboutDialog()));
    setIcon (ABOUT_(), QIcon::fromTheme("help"));
}

//-----------------------------------------------------------------------------
void HelpGuiCommand::showAboutDialog ()
{
    dialog_ = new QDialog(0);
    Ui::aboutDialog ui;
    ui.setupUi (&(*dialog_));
    ui.aboutLabel->setText(ui.aboutLabel->text().replace(QString("VERSION_MAJOR"), QString::number(VERSION_MAJOR))
                                                .replace(QString("VERSION_MINOR"), QString::number(VERSION_MINOR))
                                                .replace(QString("VERSION_BUILD"), QString::number(VERSION_BUILD)));
    dialog_->setFixedSize(dialog_->minimumSizeHint());
    dialog_->show();
}

}
