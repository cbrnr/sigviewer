// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "undo_redo_gui_command.h"

namespace sigviewer
{

//-----------------------------------------------------------------------------
namespace {

class UndoRedoGuiCommandFactory: public GuiActionCommandFactory
{
public:
    QSharedPointer<GuiActionCommand> createCommand() override
    {
        return QSharedPointer<UndoRedoGuiCommand> (new UndoRedoGuiCommand);
    }
};

} // unnamed namespace

QString const UndoRedoGuiCommand::UNDO_()
{
    static QString value = tr("Undo");

    return value;
}

QString const UndoRedoGuiCommand::REDO_()
{
    static QString value = tr("Redo");

    return value;
}

QStringList const UndoRedoGuiCommand::ACTIONS_()
{
    static QStringList result = {
        UndoRedoGuiCommand::UNDO_(),
        UndoRedoGuiCommand::REDO_(),
    };

    return result;
}

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator UndoRedoGuiCommand::registrator_ ("UndoRedo",
                                                              QSharedPointer<UndoRedoGuiCommandFactory> (new UndoRedoGuiCommandFactory));



//-----------------------------------------------------------------------------
UndoRedoGuiCommand::UndoRedoGuiCommand ()
    : GuiActionCommand (ACTIONS_())
{
    // nothing to do here
}


//-----------------------------------------------------------------------------
void UndoRedoGuiCommand::init ()
{
    setShortcut (UNDO_(), QKeySequence::Undo);
    setShortcut (REDO_(), QKeySequence::Redo);
    setIcon (UNDO_(), QIcon (":/images/ic_undo_black_24dp.png"));
    setIcon (REDO_(), QIcon (":/images/ic_redo_black_24dp.png"));

    resetActionTriggerSlot (UNDO_(), SLOT(undo()));
    resetActionTriggerSlot (REDO_(), SLOT(redo()));
}

//-----------------------------------------------------------------------------
void UndoRedoGuiCommand::undo ()
{
    applicationContext()->getCurrentCommandExecuter()->undo();
}

//-----------------------------------------------------------------------------
void UndoRedoGuiCommand::redo ()
{
    applicationContext()->getCurrentCommandExecuter()->redo();
}

//-------------------------------------------------------------------------
void UndoRedoGuiCommand::evaluateEnabledness ()
{
    bool can_undo = getTabEditState() != TAB_STATE_NO_UNDO &&
                    getTabEditState() != TAB_STATE_NO_REDO_NO_UNDO &&
                    getTabEditState() != NO_TAB_EDIT_STATE &&
                    getApplicationState() == APP_STATE_FILE_OPEN;

    bool can_redo = getTabEditState() != TAB_STATE_NO_REDO &&
                    getTabEditState() != TAB_STATE_NO_REDO_NO_UNDO &&
                    getTabEditState() != NO_TAB_EDIT_STATE &&
                    getApplicationState() == APP_STATE_FILE_OPEN;

    getQAction (UNDO_())->setEnabled (can_undo);
    getQAction (REDO_())->setEnabled (can_redo);
}

}
