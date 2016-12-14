#ifndef EVENT_MANAGER_IMPL_H
#define EVENT_MANAGER_IMPL_H

#include "file_handling/event_manager.h"
#include "file_handling/file_signal_reader.h"
#include "event_table_file_reader.h"

#include <QSharedPointer>
#include <QMap>
#include <QMutex>

namespace SigViewer_
{

class EventManagerImpl : public EventManager
{
public:
    EventManagerImpl (FileSignalReader const& reader);

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
    virtual std::set<EventID> getEventsAt (unsigned pos, ChannelID channel_id) const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual double getSampleRate () const;

    //-------------------------------------------------------------------------
    virtual unsigned getMaxEventPosition () const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual QString getNameOfEventType (EventType type) const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual QString getNameOfEvent (EventID event) const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual QList<EventID> getAllEvents () const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual unsigned getNumberOfEvents () const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual std::set<EventType> getEventTypes (QString group_id = "") const;

    //-------------------------------------------------------------------------
    virtual std::set<QString> getEventTypeGroupIDs () const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual QList<EventID> getEvents (EventType type) const;

    //-------------------------------------------------------------------------
    virtual EventID getNextEventOfSameType (EventID id) const;

    //-------------------------------------------------------------------------
    virtual EventID getPreviousEventOfSameType (EventID id) const;

private:
    EventTableFileReader event_table_reader_;

    unsigned const max_event_position_;
    double sample_rate_;
    QMutex* caller_mutex_;

    typedef QMap<EventID, QSharedPointer<SignalEvent> > EventMap;
    typedef QMap<EventID, QSharedPointer<QMutex> > MutexMap;
    typedef QMultiMap<uint32, EventID> PositionMap;

    EventMap event_map_;
    MutexMap mutex_map_;
    EventID next_free_id_;
    PositionMap position_event_map_;
    QMap<EventID, uint32> temp_event_position_map_;
};

}

#endif // EVENT_MANAGER_IMPL_H
