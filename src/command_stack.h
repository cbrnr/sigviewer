#ifndef COMMAND_STACK_H
#define COMMAND_STACK_H

#include <QUndoStack>
#include <QUndoCommand>

namespace BioSig_
{

//-----------------------------------------------------------------------------
/// CommandStack
///
/// singleton which controlls the execution and undo of commands

class CommandStack
{
public:
    //-------------------------------------------------------------------------
    static CommandStack& instance ();

    //-------------------------------------------------------------------------
    void clearStacks ();

    //-------------------------------------------------------------------------
    /// cares for destruction of the given command
    void executeEditCommant (QUndoCommand* command);

    //-------------------------------------------------------------------------
    void undoLastEditCommand ();

    //-------------------------------------------------------------------------
    void redoLastUndoneEditCommand ();

    //-------------------------------------------------------------------------
    bool emptyEditCommandStack ();


    //-------------------------------------------------------------------------
    /// cares for destruction of the given command
    void executeViewCommand (QUndoCommand* command);

    //-------------------------------------------------------------------------
    void undoLastViewCommand ();

    //-------------------------------------------------------------------------
    void redoLastUndoneViewCommand ();


private:
    CommandStack();

    QUndoStack edit_command_undo_stack_;
    QUndoStack view_command_undo_stack_;
};

}

#endif // COMMAND_STACK_H
