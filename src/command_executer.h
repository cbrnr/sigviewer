#ifndef COMMAND_EXECUTER_H
#define COMMAND_EXECUTER_H

#include <QUndoCommand>

namespace BioSig_
{

class CommandExecuter
{
public:
    virtual ~CommandExecuter () {}
    virtual void executeCommand (QUndoCommand* command) = 0;
    virtual void undo () = 0;
    virtual void redo () = 0;
};

}

#endif // COMMAND_EXECUTER_H
