// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "undo_redo_gui_command.h"

namespace sigviewer
{

//-----------------------------------------------------------------------------
QString const UndoRedoGuiCommand::UNDO_ = "Undo";
QString const UndoRedoGuiCommand::REDO_ = "Redo";
QStringList const UndoRedoGuiCommand::ACTIONS_ = QStringList () <<
                                                 UndoRedoGuiCommand::UNDO_ <<
                                                 UndoRedoGuiCommand::REDO_;

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator UndoRedoGuiCommand::registrator_ ("UndoRedo",
                                                              QSharedPointer<UndoRedoGuiCommand> (new UndoRedoGuiCommand));



//-----------------------------------------------------------------------------
UndoRedoGuiCommand::UndoRedoGuiCommand ()
    : GuiActionCommand (ACTIONS_)
{
    // nothing to do here
}


//-----------------------------------------------------------------------------
void UndoRedoGuiCommand::init ()
{
    setShortcut (UNDO_, QKeySequence::Undo);
    setShortcut (REDO_, QKeySequence::Redo);
    setIcon (UNDO_, QIcon (":/images/baseline-undo-24px.svg"));
    setIcon (REDO_, QIcon (":/images/baseline-redo-24px.svg"));

    resetActionTriggerSlot (UNDO_, SLOT(undo()));
    resetActionTriggerSlot (REDO_, SLOT(redo()));
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

    getQAction (UNDO_)->setEnabled (can_undo);
    getQAction (REDO_)->setEnabled (can_redo);
}

}
