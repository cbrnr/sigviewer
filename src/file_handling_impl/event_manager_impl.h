#ifndef EVENT_MANAGER_IMPL_H
#define EVENT_MANAGER_IMPL_H

#include "../file_handling/event_manager.h"
#include "../file_handling/file_signal_reader.h"
#include "event_table_file_reader.h"

#include <QSharedPointer>
#include <QMap>
#include <QMutex>

namespace BioSig_
{

class EventManagerImpl : public EventManager
{
public:
    EventManagerImpl (QSharedPointer<FileSignalReader> reader);

    virtual ~EventManagerImpl ();

    //-------------------------------------------------------------------------
    /// see base class
    virtual QSharedPointer<SignalEvent const> getEvent (EventID id) const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual QSharedPointer<SignalEvent> getAndLockEventForEditing (EventID id);

    //-------------------------------------------------------------------------
    /// see base class
    virtual void updateAndUnlockEvent (EventID id);

    //-------------------------------------------------------------------------
    /// see base class
    virtual QSharedPointer<SignalEvent const> createEvent (ChannelID channel_id,
                                                           unsigned pos,
                                                           unsigned length,
                                                           EventType type,
                                                           EventID id = UNDEFINED_EVENT_ID);

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
    virtual std::set<EventType> getAllPossibleEventTypes () const;

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
    QSharedPointer<FileSignalReader> reader_;
    QSharedPointer<EventTableFileReader> event_table_reader_;

    double sample_rate_;
    QMutex* caller_mutex_;

    typedef QMap<EventID, QSharedPointer<SignalEvent> > EventMap;
    typedef QMap<EventID, QSharedPointer<QMutex> > MutexMap;

    EventMap event_map_;
    MutexMap mutex_map_;
    EventID next_free_id_;
};

}

#endif // EVENT_MANAGER_IMPL_H
