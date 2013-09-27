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


#include "event_table_file_reader.h"

#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QTextStream>
#include <QSettings>

namespace SigViewer_
{

//-------------------------------------------------------------------------------------------------
QString const EventTableFileReader::UNKNOWN_GROUP_ID = "Unknown Group";


//-------------------------------------------------------------------------------------------------
EventTableFileReader::EventTableFileReader()
{
    load();
    event_group_ids_.append (UNKNOWN_GROUP_ID);
    group_id2name_[UNKNOWN_GROUP_ID] = UNKNOWN_GROUP_ID;
}

// destructor
EventTableFileReader::~EventTableFileReader()
{
    // nothing
}

// load
bool EventTableFileReader::load()
{
    char g[10];
    uint16_t k;
    for (k = 0; EventCodeGroups[k].groupid < 0xffff; k++) {
        sprintf(g, "0x%04x", EventCodeGroups[k].groupid);
        QString group_id = QString(g);
        group_id2name_[group_id] = QString(EventCodeGroups[k].GroupDescription);
        event_group_ids_ << group_id;
    }
    EventItem item;
    for (k = 0; ETD[k].typ ; k++) {
        sprintf(g, "0x%04x", ETD[k].groupid);
        QString group_id = QString(g);
        item.name     =  QString(ETD[k].desc);
        item.group_id = group_id;
        event_type2name_[ETD[k].typ] = item;
        event_types_ << ETD[k].typ;
    }
    return true;
}

// get group id begin
EventTableFileReader::StringIterator EventTableFileReader::getGroupIdBegin() const
{
    return event_group_ids_.begin();
}

// get group id end
EventTableFileReader::StringIterator EventTableFileReader::getGroupIdEnd() const
{
    return event_group_ids_.end();
}

// get event types begin
EventTableFileReader::IntIterator EventTableFileReader::eventTypesBegin()
{
    return event_types_.begin();
}

// get event types end
EventTableFileReader::IntIterator EventTableFileReader::eventTypesEnd()
{
    return event_types_.end();
}

// get event name
QString EventTableFileReader::getEventName(uint16 event_type_id) const
{
    Int2EventItemMap::const_iterator it = event_type2name_.find(event_type_id);
    return it != event_type2name_.end() ? it.value().name
                                        : "NO SUCH EVENT TYPE";
}

//-----------------------------------------------------------------------------
void EventTableFileReader::setEventName (EventType event_type_id,
                                            QString const& name)
{
    event_type2name_[event_type_id].name = name;
}

//-----------------------------------------------------------------------------
void EventTableFileReader::restoreEventNames ()
{
    load();
}

//-----------------------------------------------------------------------------
std::set<EventType> EventTableFileReader::getEventsOfGroup (QString const& group_id) const
{
    std::set<EventType> group_types;
    foreach (EventType event_type, event_types_)
        if (event_type2name_[event_type].group_id == group_id)
            group_types.insert (event_type);

    return group_types;
}


// get event group id
QString EventTableFileReader::getEventGroupId(uint16 event_type_id) const
{
    Int2EventItemMap::const_iterator it = event_type2name_.find(event_type_id);
    return it != event_type2name_.end() ? it.value().group_id
                                        : "";
}

//-------------------------------------------------------------------------------------------------
bool EventTableFileReader::entryExists (EventType type) const
{
    return event_types_.contains (type);
}

//-------------------------------------------------------------------------------------------------
void EventTableFileReader::addEntry (EventType type, QString const& name, QString group_id)
{
    if (event_types_.contains (type))
        return;
    event_types_.append (type);

    if (!event_group_ids_.contains (group_id))
        group_id = UNKNOWN_GROUP_ID;

    EventItem item;
    item.group_id = group_id;

    if (name.size())
        item.name = name;
    else
    {
        item.name = QString::number (type, 16);
        while (item.name.size() < 4)
            item.name.prepend ("0");
        item.name.prepend("0x");
    }

    event_type2name_[type] = item;
}


//-------------------------------------------------------------------
std::set<EventType> EventTableFileReader::getAllEventTypes () const
{
    std::set<EventType> type_set;
    for (IntIterator it = event_types_.begin();
         it != event_types_.end();
         ++it)
    {
        type_set.insert (*it);
    }
    return type_set;
}

// get event group name
QString EventTableFileReader::getEventGroupName(const QString& group_id) const
{
    String2StringMap::const_iterator it = group_id2name_.find(group_id);
    return it != group_id2name_.end() ? it.value()
                                      : "";
}

} // namespace SigViewer_
