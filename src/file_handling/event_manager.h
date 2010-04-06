#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "../base/user_types.h"
#include "../base/signal_event.h"

#include <QString>
#include <QObject>
#include <QSharedPointer>
#include <QList>

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
    /// returns the event for editing
    ///
    /// TODO: convert to lockEventForEditing (id)
    virtual QSharedPointer<SignalEvent> getEventForEditing (EventID id) = 0;

    //-------------------------------------------------------------------------
    /// changes the type of the event of the given type
    /// and emits the signal eventChanged (id)
    ///
    /// TODO: convert to releaseEventForEditing (id)
    virtual void updateEvent (EventID id) = 0;

    //-------------------------------------------------------------------------
    ///
    /// @return const pointer to the newly created event
    virtual QSharedPointer<SignalEvent const> createEvent
            (ChannelID channel_id, unsigned pos, unsigned length,
             EventType type, EventID id = SignalEvent::UNDEFINED_ID) = 0;

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
    /// @return set of event_id of events of the given type
    virtual QList<EventID> getEventsOfType (EventType type) const = 0;

signals:
    void eventChanged (EventID id);
    void eventCreated (QSharedPointer<SignalEvent const> event);
    void eventRemoved (EventID id);
};

} // namespace BioSig_

#endif // EVENT_MANAGER_INTERFACE_H
