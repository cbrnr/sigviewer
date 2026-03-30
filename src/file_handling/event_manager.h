// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "base/sigviewer_user_types.h"
#include "base/signal_event.h"
#include "file_signal_reader.h"
#include "event_table_file_reader.h"

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <QMap>
#include <QMutex>

#include <set>

namespace sigviewer
{

class EventManager : public QObject
{
    Q_OBJECT
public:
    EventManager (FileSignalReader const& reader);

    ~EventManager ();

    QSharedPointer<SignalEvent const> getEvent (EventID id) const;

    QSharedPointer<SignalEvent> getAndLockEventForEditing (EventID id);

    void updateAndUnlockEvent (EventID id);

    QSharedPointer<SignalEvent const> createEvent (ChannelID channel_id,
                                                   unsigned pos,
                                                   unsigned length,
                                                   EventType type,
                                                   int stream_id,
                                                   EventID id = UNDEFINED_EVENT_ID);

    void removeEvent (EventID id);

    std::set<EventID> getEventsAt (unsigned pos, ChannelID channel_id) const;

    double getSampleRate () const;

    unsigned getMaxEventPosition () const;

    QString getNameOfEventType (EventType type) const;

    QString getNameOfEvent (EventID event) const;

    QList<EventID> getAllEvents () const;

    unsigned getNumberOfEvents () const;

    std::set<EventType> getEventTypes (QString group_id = "") const;

    std::set<QString> getEventTypeGroupIDs () const;

    QList<EventID> getEvents (EventType type) const;

    EventID getNextEventOfSameType (EventID id) const;

    EventID getPreviousEventOfSameType (EventID id) const;

    QString getFileType () const;

    void setEventName (EventType event_type_id, QString const& name);

signals:
    void eventChanged (EventID id);
    void eventCreated (QSharedPointer<SignalEvent const> event);
    void eventRemoved (EventID id);
    void changed ();

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
    QString file_type_;
};

}

#endif // EVENT_MANAGER_H
