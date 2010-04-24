#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "../base/user_types.h"
#include "../base/signal_event.h"

#include <QString>
#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <QMap>

#include <set>

namespace BioSig_
{

//-----------------------------------------------------------------------------
/// EventManager
///
/// interface (abstract base class) for the the central
/// access point for any event manipulation
class EventManager : public QObject
{
    Q_OBJECT

public:
    //-------------------------------------------------------------------------
    /// virtual destructor
    virtual ~EventManager () {}

    //-------------------------------------------------------------------------
    virtual QSharedPointer<SignalEvent const> getEvent (EventID id) const = 0;

    //-------------------------------------------------------------------------
    /// returns the event for editing and locks the event
    virtual QSharedPointer<SignalEvent> getAndLockEventForEditing (EventID id) = 0;

    //-------------------------------------------------------------------------
    /// changes the type of the event of the given type
    /// and emits the signal eventChanged (id)
    virtual void updateAndUnlockEvent (EventID id) = 0;

    //-------------------------------------------------------------------------
    ///
    /// @return const pointer to the newly created event
    virtual QSharedPointer<SignalEvent const> createEvent
            (ChannelID channel_id, unsigned pos, unsigned length,
             EventType type, EventID id = UNDEFINED_EVENT_ID) = 0;

    //-------------------------------------------------------------------------
    /// removes the event with the given id
    virtual void removeEvent (EventID id) = 0;

    //-------------------------------------------------------------------------
    ///
    /// @return set of event_id of events at the given position
    virtual std::set<EventID> getEventsAt (unsigned pos, unsigned channel_id) const = 0;

    //-------------------------------------------------------------------------
    /// @return sample rate of events
    virtual double getSampleRate () const = 0;

    //-------------------------------------------------------------------------
    /// @return the name of the given event type
    virtual QString getNameOfEventType (EventType type) const = 0;

    //-------------------------------------------------------------------------
    /// @return list of event_id of all events
    virtual QList<EventID> getAllEvents () const = 0;

    //-------------------------------------------------------------------------
    /// @return set of event_id of all possible event types (even those of
    ///         which no event exists at the moment
    virtual std::set<EventType> getAllPossibleEventTypes () const = 0;

    //-------------------------------------------------------------------------
    /// @return set of event_ids of events of the given type
    virtual QList<EventID> getEvents (EventType type) const = 0;

    //-------------------------------------------------------------------------
    /// @return map of positions and event_ids of events of the given type
    virtual QMap<uint32, EventID> getEventPositions (EventType type) const = 0;

signals:
    void eventChanged (EventID id);
    void eventCreated (QSharedPointer<SignalEvent const> event);
    void eventRemoved (EventID id);
};

} // namespace BioSig_

#endif // EVENT_MANAGER_INTERFACE_H
