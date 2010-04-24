#ifndef MACRO_UNDO_COMMAND_H
#define MACRO_UNDO_COMMAND_H

#include <QUndoCommand>
#include <QSharedPointer>

namespace BioSig_
{

class MacroUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    /// constructor
    MacroUndoCommand (QList<QSharedPointer<QUndoCommand> > const& commands);

    //-------------------------------------------------------------------------
    /// destructor
    virtual ~MacroUndoCommand () {}

    //-------------------------------------------------------------------------
    /// undos the all commands
    virtual void undo ();

    //-------------------------------------------------------------------------
    /// executes the macro
    virtual void redo ();


private:
    QList<QSharedPointer<QUndoCommand> > commands_;

    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    MacroUndoCommand (MacroUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    MacroUndoCommand& operator= (MacroUndoCommand const &);

};

}

#endif // MACRO_UNDO_COMMAND_H
