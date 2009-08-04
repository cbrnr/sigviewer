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
    /// cares for destruction of the given command
    void executeCommand (QUndoCommand* command);

    //-------------------------------------------------------------------------
    void undoLastCommand ();

    //-------------------------------------------------------------------------
    void redoLastUndoneCommand ();

private:
    CommandStack();

    QUndoStack undo_stack_;
};

}

#endif // COMMAND_STACK_H
