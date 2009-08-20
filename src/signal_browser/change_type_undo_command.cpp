#include "change_type_undo_command.h"

namespace BioSig_
{

namespace PortingToQT4_
{

//-----------------------------------------------------------------------------
ChangeTypeUndoCommand::ChangeTypeUndoCommand (SignalBrowserModel& signal_browser_model,
                                              QSharedPointer<SignalEvent> signal_event,
                                              uint16 new_type)
 : signal_browser_model_ (signal_browser_model),
   signal_event_ (signal_event),
   new_type_ (new_type)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
ChangeTypeUndoCommand::~ChangeTypeUndoCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void ChangeTypeUndoCommand::undo ()
{
    signal_event_->setType(old_type_);
    signal_browser_model_.setEventChanged(signal_event_->getId());
}

//-----------------------------------------------------------------------------
void ChangeTypeUndoCommand::redo ()
{
    old_type_ = signal_event_->getType();
    signal_event_->setType(new_type_);
    signal_browser_model_.setEventChanged(signal_event_->getId());
}
}
}
