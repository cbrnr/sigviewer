#include "close_file_gui_command.h"
#include "../application_context.h"

#include <QMessageBox>

namespace BioSig_
{

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator CloseFileGuiCommand::registrator_ ("Close File",
                                                               QSharedPointer<CloseFileGuiCommand> (new CloseFileGuiCommand));
//-----------------------------------------------------------------------------
CloseFileGuiCommand::CloseFileGuiCommand ()
    : GuiActionCommand ("Close... (new impl)")
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
CloseFileGuiCommand::~CloseFileGuiCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void CloseFileGuiCommand::init ()
{
    //getQAction()->setShortcut (QKeySequence::Close);
    getQAction()->setIcon (QIcon(":/images/icons/fileclose.png"));
}


//-----------------------------------------------------------------------------
void CloseFileGuiCommand::trigger ()
{
    QSharedPointer<FileContext> current_file_context =
            ApplicationContext::getInstance()->getCurrentFileContext();

    if (current_file_context.isNull())
        return;

    if (current_file_context->getState ()
        == FILE_STATE_CHANGED)
    {
        QString file_name = current_file_context->getFileName ();
        QMessageBox::StandardButton pressed_button =  QMessageBox::question (0, tr("Really close?"),
            tr("Changes in '%1' are not saved!!").arg(file_name) + "\n" +
            tr("Really close?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if (pressed_button == QMessageBox::No)
            return;
    }

    ApplicationContext::getInstance()->getMainWindowModel ()->closeCurrentFileTabs ();
    ApplicationContext::getInstance()->removeCurrentFileContext ();
    ApplicationContext::getInstance()->setState (APP_STATE_NO_FILE_OPEN);
}

//-----------------------------------------------------------------------------
void CloseFileGuiCommand::applicationStateChanged (ApplicationState state)
{
    if (state == APP_STATE_NO_FILE_OPEN)
        emit qActionEnabledChanged (false);
    else
        emit qActionEnabledChanged (true);
}


} // namespace BioSig_
