// © SigViewer developers
//
// License: GPL-3.0

#ifndef COMMAND_EXECUTER_H
#define COMMAND_EXECUTER_H

#include <QUndoCommand>

namespace sigviewer {

class CommandExecuter {
   public:
    virtual ~CommandExecuter() {}
    virtual void executeCommand(QUndoCommand* command) = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;
};

}  // namespace sigviewer

#endif  // COMMAND_EXECUTER_H
