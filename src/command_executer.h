// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef COMMAND_EXECUTER_H
#define COMMAND_EXECUTER_H

#include <QUndoCommand>


namespace sigviewer
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
