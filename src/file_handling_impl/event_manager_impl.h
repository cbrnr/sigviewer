#ifndef EVENT_MANAGER_IMPL_H
#define EVENT_MANAGER_IMPL_H

#include "../file_handling/event_manager.h"
#include "../file_handling/file_signal_reader.h"

#include <QMap>

namespace BioSig_
{

class EventTableFileReader;

class EventManagerImpl : public EventManager
{
public:
    EventManagerImpl (FileSignalReader& reader,
                  EventTableFileReader const& event_table_reader);
    virtual ~EventManagerImpl ();

    //-------------------------------------------------------------------------
    /// see base class
    virtual QSharedPointer<SignalEvent const> getEvent (EventID id) const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual QSharedPointer<SignalEvent> getEventForEditing (EventID id);

    //-------------------------------------------------------------------------
    /// see base class
    virtual void updateEvent (EventID id);

    //-------------------------------------------------------------------------
    /// see base class
    virtual QSharedPointer<SignalEvent const> createEvent (ChannelID channel_id,
                                                           unsigned pos,
                                                           unsigned length,
                                                           EventType type,
                                                           EventID id = SignalEvent::UNDEFINED_ID);

    //-------------------------------------------------------------------------
    /// see base class
    virtual void removeEvent (EventID id);

    //-------------------------------------------------------------------------
    /// see base class
    virtual std::set<EventID> getEventsAt (unsigned pos, unsigned channel_id) const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual double getSampleRate () const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual QString getNameOfEventType (EventType type) const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual QList<EventID> getAllEvents () const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual QList<EventID> getEvents (EventType type) const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual QMap<uint32, EventID> getEventPositions (EventType type) const;

    //-------------------------------------------------------------------------
    /// fills the given eventvector with all events
    void getAllEvents (FileSignalReader::SignalEventVector& event_vector) const;

private:
    FileSignalReader& reader_;
    EventTableFileReader const& event_table_reader_;

    double sample_rate_;
    typedef QMap<EventID, QSharedPointer<SignalEvent> > EventMap;
    EventMap event_map_;
    EventID next_free_id_;
};

}

#endif // EVENT_MANAGER_IMPL_H
