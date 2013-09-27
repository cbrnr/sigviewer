// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "base/sigviewer_user_types.h"
#include "base/signal_event.h"

#include <QString>
#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <QMap>

#include <set>

namespace SigViewer_
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
    virtual std::set<EventID> getEventsAt (unsigned pos, ChannelID channel_id) const = 0;

    //-------------------------------------------------------------------------
    /// @return sample rate of events
    virtual double getSampleRate () const = 0;

    //-------------------------------------------------------------------------
    virtual unsigned getMaxEventPosition () const = 0;

    //-------------------------------------------------------------------------
    /// @return the name of the given event type
    virtual QString getNameOfEventType (EventType type) const = 0;

    //-------------------------------------------------------------------------
    /// @return the name of the given event
    virtual QString getNameOfEvent (EventID event) const = 0;

    //-------------------------------------------------------------------------
    /// @return list of event_id of all events
    virtual QList<EventID> getAllEvents () const = 0;

    //-------------------------------------------------------------------------
    /// @return amount of events
    virtual unsigned getNumberOfEvents () const = 0;

    //-------------------------------------------------------------------------
    virtual std::set<EventType> getEventTypes () const {return getEventTypes ("");}

    //-------------------------------------------------------------------------
    virtual std::set<EventType> getEventTypes (QString group_id) const = 0;

    //-------------------------------------------------------------------------
    virtual std::set<QString> getEventTypeGroupIDs () const = 0;

    //-------------------------------------------------------------------------
    /// @return set of event_ids of events of the given type
    virtual QList<EventID> getEvents (EventType type) const = 0;

    //-------------------------------------------------------------------------
    virtual EventID getNextEventOfSameType (EventID id) const = 0;

    //-------------------------------------------------------------------------
    virtual EventID getPreviousEventOfSameType (EventID id) const = 0;

signals:
    void eventChanged (EventID id);
    void eventCreated (QSharedPointer<SignalEvent const> event);
    void eventRemoved (EventID id);
    void changed ();
};

} // namespace SigViewer_

#endif // EVENT_MANAGER_INTERFACE_H
