#include "event_manager_impl.h"
#include "../file_handling/file_signal_reader.h"
#include "../application_context.h"

#include <QMutexLocker>

#include <cassert>

namespace BioSig_
{

//-----------------------------------------------------------------------------
EventManagerImpl::EventManagerImpl (QSharedPointer<FileSignalReader> reader)
    : reader_ (reader),
      event_table_reader_ (ApplicationContext::getInstance()->getEventTableFileReader()),
      caller_mutex_ (new QMutex)
{
    assert (reader_->isOpen());
    FileSignalReader::SignalEventVector signal_events;
    reader_->loadEvents (signal_events);
    next_free_id_ = 0;
    for (int index = 0; index < signal_events.size(); index++)
    {
        event_map_.insert (next_free_id_,
                           QSharedPointer<SignalEvent> (
                                   new SignalEvent(signal_events[index],
                                                   next_free_id_)));
        mutex_map_.insert (next_free_id_,
                           QSharedPointer<QMutex> (
                                   new QMutex));
        next_free_id_++;
    }
    sample_rate_ = reader_->getBasicHeader()->getEventSamplerate();
    QMap<unsigned, QString> event_names = reader_->getBasicHeader()->getNamesOfUserSpecificEvents();
    for (QMap<unsigned, QString>::iterator name_iter = event_names.begin();
         name_iter != event_names.end();
         ++name_iter)
        event_table_reader_->setEventName (name_iter.key(), name_iter.value());
}

//-----------------------------------------------------------------------------
EventManagerImpl::~EventManagerImpl ()
{
    event_table_reader_->restoreEventNames ();
    delete caller_mutex_;
}

//-----------------------------------------------------------------------------
QSharedPointer<SignalEvent const> EventManagerImpl::getEvent (EventID id) const
{
    QMutexLocker locker (caller_mutex_);
    EventMap::ConstIterator event_it = event_map_.find (id);
    if (event_it == event_map_.end())
        return QSharedPointer<SignalEvent const> (0);
    else
        return event_it.value();
}

//-----------------------------------------------------------------------------
QSharedPointer<SignalEvent> EventManagerImpl::getAndLockEventForEditing (EventID id)
{
    QMutexLocker locker (caller_mutex_);
    EventMap::Iterator event_it = event_map_.find (id);
    if (event_it == event_map_.end())
        return QSharedPointer<SignalEvent> (0);
    else
    {
        mutex_map_[id]->lock ();
        return event_it.value();
    }
}

//-----------------------------------------------------------------------------
void EventManagerImpl::updateAndUnlockEvent (EventID id)
{
    QMutexLocker locker (caller_mutex_);
    mutex_map_[id]->unlock ();
    emit eventChanged (id);
    emit changed ();
}

//-----------------------------------------------------------------------------
QSharedPointer<SignalEvent const> EventManagerImpl::createEvent (
        ChannelID channel_id, unsigned pos, unsigned duration, EventType type,
        EventID id)
{
    QMutexLocker locker (caller_mutex_);
    if (id == UNDEFINED_EVENT_ID)
    {
        id = next_free_id_;
        next_free_id_++;
    }
    else
        if (event_map_.contains(id))
            return QSharedPointer<SignalEvent>(0);

    QSharedPointer<SignalEvent> new_event (
            new SignalEvent(pos, type, sample_rate_, channel_id, duration, id));
    event_map_[id] = new_event;
    mutex_map_[id] = QSharedPointer<QMutex> (new QMutex);

    emit eventCreated (new_event);
    emit changed ();
    return new_event;
}

//-----------------------------------------------------------------------------
void EventManagerImpl::removeEvent (EventID id)
{
    QMutexLocker locker (caller_mutex_);
    EventMap::iterator event_iter = event_map_.find (id);
    if (event_iter == event_map_.end())
        return;

    event_map_.remove (id);
    mutex_map_.remove (id);
    emit eventRemoved (id);
    emit changed ();
}

//-----------------------------------------------------------------------------
std::set<EventID> EventManagerImpl::getEventsAt (unsigned pos,
                                             unsigned channel_id) const
{
    QMutexLocker locker (caller_mutex_);
    std::set<EventID> bla;
    pos++;
    channel_id++;
    return bla;
}

//-----------------------------------------------------------------------------
double EventManagerImpl::getSampleRate () const
{
    QMutexLocker locker (caller_mutex_);
    return sample_rate_;
}

//-----------------------------------------------------------------------------
QString EventManagerImpl::getNameOfEventType (EventType type) const
{
    QMutexLocker locker (caller_mutex_);
    return event_table_reader_->getEventName (type);
}

//-----------------------------------------------------------------------------
QList<EventID> EventManagerImpl::getAllEvents () const
{
    QMutexLocker locker (caller_mutex_);
    return event_map_.keys ();
}

//-----------------------------------------------------------------------------
std::set<EventType> EventManagerImpl::getAllPossibleEventTypes () const
{
    QMutexLocker locker (caller_mutex_);
    return event_table_reader_->getAllEventTypes ();
}

//-----------------------------------------------------------------------------
QList<EventID> EventManagerImpl::getEvents (EventType type) const
{
    QMutexLocker locker (caller_mutex_);
    QList<EventID> events;
    for (EventMap::ConstIterator event_iter = event_map_.begin ();
         event_iter != event_map_.end ();
         ++event_iter)
        if (event_iter.value ()->getType () == type)
        {
            events.push_back (event_iter.key ());
        }
    return events;
}

//-----------------------------------------------------------------------------
QMap<uint32, EventID> EventManagerImpl::getEventPositions (EventType type) const
{
    QMutexLocker locker (caller_mutex_);
    QMap<uint32, EventID> events;
    for (EventMap::ConstIterator event_iter = event_map_.begin ();
         event_iter != event_map_.end ();
         ++event_iter)
        if (event_iter.value ()->getType () == type)
        {
            events.insert (event_iter.value()->getPosition (), event_iter.key ());
        }
    return events;
}

//-----------------------------------------------------------------------------
void EventManagerImpl::getAllEvents (FileSignalReader::SignalEventVector&
                                 event_vector) const
{
    QMutexLocker locker (caller_mutex_);
    for (EventMap::ConstIterator event_iter = event_map_.begin ();
         event_iter != event_map_.end ();
         ++event_iter)
    {
        event_vector.push_back (*(event_iter.value ()));
    }
}




}
