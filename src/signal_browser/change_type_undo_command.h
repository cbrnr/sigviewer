#ifndef CHANGE_TYPE_UNDO_COMMAND_H
#define CHANGE_TYPE_UNDO_COMMAND_H


#include <QUndoCommand>
#include <QSharedPointer>

namespace BioSig_
{

namespace PortingToQT4_
{

class ChangeTypeUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    ChangeTypeUndoCommand();

    //-------------------------------------------------------------------------
    virtual ~ChangeTypeUndoCommand();

    //-------------------------------------------------------------------------
    /// recreates the deleted event
    virtual void undo ();

    //-------------------------------------------------------------------------
    /// deletes the event
    virtual void redo ();

private:
    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    ChangeTypeUndoCommand (ChangeTypeUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    ChangeTypeUndoCommand& operator= (ChangeTypeUndoCommand const &);
};

}

}


#endif // CHANGE_TYPE_UNDO_COMMAND_H
