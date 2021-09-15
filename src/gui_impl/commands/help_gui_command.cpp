// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "help_gui_command.h"
#include "tests/tests_dialog.h"
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

QString const HelpGuiCommand::RUN_TESTS_()
{
    static QString value = tr("Run Tests...");

    return value;
}

QStringList const HelpGuiCommand::ACTIONS_()
{
    static QStringList result = {
        HelpGuiCommand::ABOUT_(),
        HelpGuiCommand::RUN_TESTS_(),
    };

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
    setIcon (ABOUT_(), QIcon(":/images/ic_help_outline_black_24dp.png"));
    resetActionTriggerSlot (RUN_TESTS_(), SLOT(runTests()));
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

//-----------------------------------------------------------------------------
void HelpGuiCommand::runTests ()
{
    TestsDialog tests_dialog;
    tests_dialog.exec ();
}

}
