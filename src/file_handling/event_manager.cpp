// © SigViewer developers
//
// License: GPL-3.0

#include "event_manager.h"

#include <QDebug>

namespace sigviewer {

//-----------------------------------------------------------------------------
EventManager::EventManager(FileSignalReader const& reader)
    : max_event_position_(reader.getBasicHeader()->getNumberOfSamples()),
      caller_mutex_(new QMutex) {
    file_type_ = reader.getBasicHeader()->getFileTypeString();

    QList<QSharedPointer<SignalEvent const> > signal_events = reader.getEvents();
    next_free_id_ = 0;
    for (int index = 0; index < signal_events.size(); index++) {
        event_map_.insert(next_free_id_,
            QSharedPointer<SignalEvent>(new SignalEvent(*(signal_events[index]), next_free_id_)));
        mutex_map_.insert(next_free_id_, QSharedPointer<QMutex>(new QMutex));

        position_event_map_.insert(signal_events[index]->getPosition(), next_free_id_);
        if (!event_table_reader_.entryExists(signal_events[index]->getType()))
            event_table_reader_.addEntry(signal_events[index]->getType());

        next_free_id_++;
    }
    sample_rate_ = reader.getBasicHeader()->getEventSamplerate();
    QMap<unsigned, QString> event_names = reader.getBasicHeader()->getNamesOfUserSpecificEvents();
    for (QMap<unsigned, QString>::iterator name_iter = event_names.begin();
        name_iter != event_names.end();
        ++name_iter)
        event_table_reader_.setEventName(name_iter.key(), name_iter.value());
}

//-----------------------------------------------------------------------------
EventManager::~EventManager() {
    event_table_reader_.restoreEventNames();
    delete caller_mutex_;
}

//-----------------------------------------------------------------------------
QSharedPointer<SignalEvent const> EventManager::getEvent(EventID id) const {
    QMutexLocker locker(caller_mutex_);
    EventMap::ConstIterator event_it = event_map_.find(id);
    if (event_it == event_map_.end())
        return QSharedPointer<SignalEvent const>(0);
    else
        return event_it.value();
}

//-----------------------------------------------------------------------------
QSharedPointer<SignalEvent> EventManager::getAndLockEventForEditing(EventID id) {
    QMutexLocker locker(caller_mutex_);
    EventMap::Iterator event_it = event_map_.find(id);
    if (event_it == event_map_.end())
        return QSharedPointer<SignalEvent>(0);
    else {
        temp_event_position_map_.insert(id, event_it.value()->getPosition());
        mutex_map_[id]->lock();
        return event_it.value();
    }
}

//-----------------------------------------------------------------------------
void EventManager::updateAndUnlockEvent(EventID id) {
    if (!mutex_map_.contains(id)) return;
    position_event_map_.remove(temp_event_position_map_[id], id);
    temp_event_position_map_.remove(id);
    position_event_map_.insert(event_map_[id]->getPosition(), id);
    mutex_map_[id]->unlock();
    emit eventChanged(id);
    emit changed();
}

//-----------------------------------------------------------------------------
QSharedPointer<SignalEvent const> EventManager::createEvent(ChannelID channel_id,
    unsigned pos,
    unsigned duration,
    EventType type,
    int stream_id,
    EventID id) {
    if (id == UNDEFINED_EVENT_ID) {
        id = next_free_id_;
        next_free_id_++;
    } else if (event_map_.contains(id))
        return QSharedPointer<SignalEvent>(0);

    QSharedPointer<SignalEvent> new_event(
        new SignalEvent(pos, type, sample_rate_, stream_id, channel_id, duration, id));
    event_map_[id] = new_event;
    mutex_map_[id] = QSharedPointer<QMutex>(new QMutex);
    position_event_map_.insert(pos, id);

    emit eventCreated(new_event);
    emit changed();
    return new_event;
}

//-----------------------------------------------------------------------------
void EventManager::removeEvent(EventID id) {
    qDebug() << "EventManager::removeEvent " << id;
    EventMap::iterator event_iter = event_map_.find(id);
    if (event_iter == event_map_.end()) return;

    position_event_map_.remove(event_map_[id]->getPosition(), id);
    event_map_.remove(id);
    mutex_map_.remove(id);
    qDebug() << "EventManager::removeEvent " << id << " emitting";
    emit eventRemoved(id);
    emit changed();
    qDebug() << "EventManager::removeEvent " << id << " finished";
}

//-----------------------------------------------------------------------------
std::set<EventID> EventManager::getEventsAt(unsigned pos, ChannelID channel_id) const {
    QMutexLocker locker(caller_mutex_);
    std::set<EventID> events;
    QSharedPointer<SignalEvent const> event_ptr;
    foreach (uint32 position, position_event_map_.keys()) {
        foreach (EventID event, position_event_map_.values(position)) {
            event_ptr = event_map_[event];
            if (event_ptr->getPosition() <= pos
                && event_ptr->getPosition() + event_ptr->getDuration() >= pos
                && (event_ptr->getChannel() == channel_id || event_ptr->getChannel() == UNDEFINED_CHANNEL))
                events.insert(event);
        }
    }
    return events;
}

//-----------------------------------------------------------------------------
double EventManager::getSampleRate() const {
    QMutexLocker locker(caller_mutex_);
    return sample_rate_;
}

//-------------------------------------------------------------------------
unsigned EventManager::getMaxEventPosition() const { return max_event_position_; }

//-----------------------------------------------------------------------------
QString EventManager::getNameOfEventType(EventType type) const {
    QMutexLocker locker(caller_mutex_);
    return event_table_reader_.getEventName(type);
}

//-------------------------------------------------------------------------
QString EventManager::getNameOfEvent(EventID event) const {
    if (event_map_.contains(event))
        return event_table_reader_.getEventName(event_map_[event]->getType());
    else
        return "";
}

//-----------------------------------------------------------------------------
QList<EventID> EventManager::getAllEvents() const {
    QMutexLocker locker(caller_mutex_);
    return event_map_.keys();
}

//-----------------------------------------------------------------------------
unsigned EventManager::getNumberOfEvents() const {
    QMutexLocker locker(caller_mutex_);
    return event_map_.size();
}

//-----------------------------------------------------------------------------
std::set<EventType> EventManager::getEventTypes(QString group_id) const {
    QMutexLocker locker(caller_mutex_);
    if (group_id.size())
        return event_table_reader_.getEventsOfGroup(group_id);
    else
        return event_table_reader_.getAllEventTypes();
}

//-------------------------------------------------------------------------
std::set<QString> EventManager::getEventTypeGroupIDs() const {
    std::set<QString> groups;
    for (EventTableFileReader::StringIterator group = event_table_reader_.getGroupIdBegin();
        group != event_table_reader_.getGroupIdEnd();
        ++group)
        groups.insert(*group);
    return groups;
}

//-----------------------------------------------------------------------------
QList<EventID> EventManager::getEvents(EventType type) const {
    QMutexLocker locker(caller_mutex_);
    QList<EventID> events;
    for (EventMap::ConstIterator event_iter = event_map_.begin();
        event_iter != event_map_.end();
        ++event_iter)
        if (event_iter.value()->getType() == type) {
            events.push_back(event_iter.key());
        }
    return events;
}

//-------------------------------------------------------------------------
EventID EventManager::getNextEventOfSameType(EventID id) const {
    QMutexLocker locker(caller_mutex_);
    if (!event_map_.contains(id)) return UNDEFINED_EVENT_ID;

    EventType type = event_map_[id]->getType();
    PositionMap::const_iterator iter = position_event_map_.find(event_map_[id]->getPosition());
    ++iter;
    while (iter != position_event_map_.end()) {
        foreach (EventID next_event_id, position_event_map_.values(iter.key()))
            if (event_map_[next_event_id]->getType() == type && next_event_id != id)
                return next_event_id;
        ++iter;
    }
    return UNDEFINED_EVENT_ID;
}

//-------------------------------------------------------------------------
EventID EventManager::getPreviousEventOfSameType(EventID id) const {
    QMutexLocker locker(caller_mutex_);
    if (!event_map_.contains(id)) return UNDEFINED_EVENT_ID;

    EventType type = event_map_[id]->getType();
    PositionMap::const_iterator iter = position_event_map_.find(event_map_[id]->getPosition());
    if (iter == position_event_map_.begin()) return UNDEFINED_EVENT_ID;
    --iter;
    while (iter != position_event_map_.begin()) {
        foreach (EventID previous_event_id, position_event_map_.values(iter.key()))
            if (event_map_[previous_event_id]->getType() == type && previous_event_id != id)
                return previous_event_id;
        --iter;
    }
    foreach (EventID previous_event_id, position_event_map_.values(iter.key()))
        if (event_map_[previous_event_id]->getType() == type && previous_event_id != id)
            return previous_event_id;

    return UNDEFINED_EVENT_ID;
}

QString EventManager::getFileType() const { return file_type_; }

void EventManager::setEventName(EventType event_type_id, const QString& name) {
    event_table_reader_.setEventName(event_type_id, name);
}

}  // namespace sigviewer
