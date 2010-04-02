#ifndef FIT_VIEW_TO_EVENT_VIEW_UNDO_COMMAND_H
#define FIT_VIEW_TO_EVENT_VIEW_UNDO_COMMAND_H

#include "base/user_types.h"

#include <QSharedPointer>
#include <QUndoCommand>
#include <QPointF>

namespace BioSig_
{

class SignalBrowserModel;
class SignalEvent;

class FitViewToEventViewUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    FitViewToEventViewUndoCommand(SignalBrowserModel& signal_browser_model);

    //-------------------------------------------------------------------------
    /// destructor
    virtual ~FitViewToEventViewUndoCommand ();

    //-------------------------------------------------------------------------
    virtual void undo ();

    //-------------------------------------------------------------------------
    virtual void redo ();


private:
    SignalBrowserModel& signal_browser_model_;
    QSharedPointer<SignalEvent const> selected_event_;
    float64 previous_pixel_per_sec_;
    QPointF previous_view_position_;
    static float32 const MARGIN_LEFT_RIGHT_;

    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    FitViewToEventViewUndoCommand (FitViewToEventViewUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    FitViewToEventViewUndoCommand& operator= (FitViewToEventViewUndoCommand const &);
};

}

#endif // FIT_VIEW_TO_EVENT_VIEW_UNDO_COMMAND_H
