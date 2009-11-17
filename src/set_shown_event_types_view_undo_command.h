#ifndef SET_SHOWN_EVENT_TYPES_VIEW_UNDO_COMMAND_H
#define SET_SHOWN_EVENT_TYPES_VIEW_UNDO_COMMAND_H

#include "signal_browser/signal_browser_model_4.h"
#include <QUndoCommand>

namespace BioSig_
{

class SetShownEventTypesViewUndoCommand  : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    SetShownEventTypesViewUndoCommand (SignalBrowserModel& signal_browser_model, SignalBrowserModel::IntList const& event_types_to_show);

    //-------------------------------------------------------------------------
    /// destructor
    virtual ~SetShownEventTypesViewUndoCommand ();

    //-------------------------------------------------------------------------
    virtual void undo ();

    //-------------------------------------------------------------------------
    virtual void redo ();

private:
    SignalBrowserModel& signal_browser_model_;
    SignalBrowserModel::IntList new_event_types_to_show_;
    SignalBrowserModel::IntList previously_shown_event_types_;

    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    SetShownEventTypesViewUndoCommand (SetShownEventTypesViewUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    SetShownEventTypesViewUndoCommand& operator= (SetShownEventTypesViewUndoCommand const &);
};

}

#endif // SET_SHOWN_EVENT_TYPES_VIEW_UNDO_COMMAND_H
