#ifndef NEXT_EVENT_VIEW_UNDO_COMMAND_H
#define NEXT_EVENT_VIEW_UNDO_COMMAND_H

#include "signal_browser/signal_browser_model_4.h"

#include <QUndoCommand>

namespace BioSig_
{

namespace PortingToQT4_
{

class NextEventViewUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    NextEventViewUndoCommand(SignalBrowserModel& signal_browser_model);

    //-------------------------------------------------------------------------
    /// destructor
    virtual ~NextEventViewUndoCommand ();

    //-------------------------------------------------------------------------
    virtual void undo ();

    //-------------------------------------------------------------------------
    virtual void redo ();


private:
    SignalBrowserModel& signal_browser_model_;

    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    NextEventViewUndoCommand (NextEventViewUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    NextEventViewUndoCommand& operator= (NextEventViewUndoCommand const &);


};

}

}

#endif // NEXT_EVENT_VIEW_UNDO_COMMAND_H
