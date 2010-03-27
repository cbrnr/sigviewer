#ifndef EVENT_MANAGER_INTERFACE_H
#define EVENT_MANAGER_INTERFACE_H

#include "../base/user_types.h"

#include <QString>

#include <set>

namespace BioSig_
{

typedef uint32 EventID;
typedef uint16 EventType;

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
    virtual void createEvent (unsigned pos, unsigned channel_id) = 0;

    //-------------------------------------------------------------------------
    ///
    /// @return set of event_id of events at the given position
    virtual std::set<EventID> getEventsAt (unsigned pos, unsigned channel_id) const = 0;

    //-------------------------------------------------------------------------
    /// @return sample rate of events
    virtual unsigned getSampleRate () const = 0;

    //-------------------------------------------------------------------------
    /// @return the name of the given event type
    virtual QString getNameOfEventType (EventType type) const = 0;

    //-------------------------------------------------------------------------
    /// @return set of event_id of events of the given type
    virtual std::set<EventID> getEventsOfType (EventType type) const = 0;
};

} // namespace BioSig_

#endif // EVENT_MANAGER_INTERFACE_H
