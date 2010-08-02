#include "close_file_gui_command.h"

#include <QApplication>
#include <QMessageBox>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
QString const CloseFileGuiCommand::CLOSE_FILE_ = "Close";
QString const CloseFileGuiCommand::EXIT_APPLICATION_ = "Exit";
QStringList const CloseFileGuiCommand::ACTIONS_ = QStringList() <<
                                                  CloseFileGuiCommand::CLOSE_FILE_ <<
                                                  CloseFileGuiCommand::EXIT_APPLICATION_;


//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator CloseFileGuiCommand::registrator_ ("Closing",
                                                               QSharedPointer<CloseFileGuiCommand> (new CloseFileGuiCommand));

//-----------------------------------------------------------------------------
CloseFileGuiCommand::CloseFileGuiCommand ()
    : GuiActionCommand (ACTIONS_)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void CloseFileGuiCommand::init ()
{
    getQAction(CLOSE_FILE_)->setShortcut (QKeySequence::Close);
    getQAction(CLOSE_FILE_)->setIcon (QIcon(":/images/icons/fileclose.png"));

#if QT_VERSION >= 0x040600
    getQAction(EXIT_APPLICATION_)->setShortcut (QKeySequence::Quit);
#endif
    getQAction(EXIT_APPLICATION_)->setIcon (QIcon(":/images/icons/exit.png"));

    resetActionTriggerSlot(CLOSE_FILE_, SLOT(closeFile()));
    resetActionTriggerSlot(EXIT_APPLICATION_, SLOT(exitApplication()));
}


//-------------------------------------------------------------------------
bool CloseFileGuiCommand::closeCurrentFile ()
{
    QSharedPointer<FileContext> current_file_context =
            applicationContext()->getCurrentFileContext();

    if (current_file_context.isNull())
        return true;

    if (current_file_context->getState ()
        == FILE_STATE_CHANGED)
    {
        QString file_name = current_file_context->getFileName ();
        QMessageBox::StandardButton pressed_button =  QMessageBox::question (0, tr("Really close?"),
            tr("Changes in '%1' are not saved!!").arg(file_name) + "\n" +
            tr("Really close?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if (pressed_button == QMessageBox::No)
            return false;
    }

    applicationContext()->getMainWindowModel ()->closeCurrentFileTabs ();
    applicationContext()->removeCurrentFileContext ();
    return true;
}

//-------------------------------------------------------------------------
void CloseFileGuiCommand::closeFile ()
{
    closeCurrentFile ();
}

//-------------------------------------------------------------------------
void CloseFileGuiCommand::exitApplication ()
{
    if (closeCurrentFile ())
        QApplication::exit ();
}


//-------------------------------------------------------------------------
void CloseFileGuiCommand::evaluateEnabledness ()
{
    disableIfNoFileIsOpened (QStringList() << CLOSE_FILE_);
}

} // namespace SigViewer_
