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


#include "undo_redo_gui_command.h"

namespace SigViewer_
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
    setIcon (UNDO_, QIcon (":/images/icons/undo.png"));
    setIcon (REDO_, QIcon (":/images/icons/redo.png"));

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




} // namespace SigViewer_
