#include "calculate_erd_ers_map_command.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
CalculateERDERSMapCommand::CalculateERDERSMapCommand(QSharedPointer<SignalBrowserModel const>
                                                     signal_browser_model,
                                                     MainWindowModelImpl&
                                                     main_window_model,
                                                     uint16 event_type,
                                                     std::vector<uint32> channels,
                                                     float seconds_before_event,
                                                     float length_in_seconds)
   : signal_browser_model_ (signal_browser_model),
     main_window_model_ (main_window_model),
     event_type_ (event_type),
     channels_ (channels),
     seconds_before_event_ (seconds_before_event),
     length_in_seconds_ (length_in_seconds)
{
   // nothing to do here
}

//-----------------------------------------------------------------------------
void CalculateERDERSMapCommand::execute ()
{
    // FIXXME: NOT IMPLEMENTED YET!
}

}
