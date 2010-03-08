#ifndef EVENT_MANAGER_INTERFACE_H
#define EVENT_MANAGER_INTERFACE_H

#include "../base/user_types.h"

#include <set>
#include <string>

namespace BioSig_
{

//-----------------------------------------------------------------------------
/// TODO: IN DEVELOPMENT
/// EventManagerInterface
///
/// interface (abstract base class) for the the central
/// access point for any event manipulation
class EventManagerInterface
{
public:
    //-------------------------------------------------------------------------
    /// virtual destructor
    virtual ~EventManagerInterface () {}

    //-------------------------------------------------------------------------
    ///
    /// @return set of event_id of events at the given position
    std::set<uint16> getEventsAt (unsigned pos, unsigned channel_id) const = 0;
};

} // namespace BioSig_

#endif // EVENT_MANAGER_INTERFACE_H
