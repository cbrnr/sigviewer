#include "fit_view_to_event_view_undo_command.h"
#include "signal_browser/event_graphics_item.h"
#include "signal_browser/signal_browser_model_4.h"

#include <iostream>

namespace BioSig_
{

float32 const FitViewToEventViewUndoCommand::MARGIN_LEFT_RIGHT_ = 0;

//-----------------------------------------------------------------------------
FitViewToEventViewUndoCommand::FitViewToEventViewUndoCommand(SignalBrowserModel& signal_browser_model)
    : signal_browser_model_ (signal_browser_model),
      selected_event_ (signal_browser_model.getSelectedSignalEvent())
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
FitViewToEventViewUndoCommand::~FitViewToEventViewUndoCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void FitViewToEventViewUndoCommand::undo ()
{
    signal_browser_model_.setPixelPerXUnit(previous_pixel_per_sec_);
    signal_browser_model_.setViewingPosition(previous_view_position_);
}

//-----------------------------------------------------------------------------
void FitViewToEventViewUndoCommand::redo ()
{
    previous_pixel_per_sec_ = signal_browser_model_.getPixelPerXUnit();
    previous_view_position_ = signal_browser_model_.getViewingPosition();
    float64 pixel_per_sec = static_cast<float64>(signal_browser_model_.getVisibleWidth()) / (selected_event_->getDurationInSec() + (2 * MARGIN_LEFT_RIGHT_));
    signal_browser_model_.setPixelPerXUnit(pixel_per_sec);
    signal_browser_model_.updateLayout ();
    signal_browser_model_.goTo(selected_event_->getPositionInSec() - MARGIN_LEFT_RIGHT_, selected_event_->getChannel());
}


}
