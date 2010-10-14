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
    QSettings settings;
    QString event_codes_file = settings.value ("eventcodes_file", ":/eventcodes.txt").toString();
    load (event_codes_file);
    event_group_ids_.append (UNKNOWN_GROUP_ID);
    group_id2name_[UNKNOWN_GROUP_ID] = UNKNOWN_GROUP_ID;
}

// destructor
EventTableFileReader::~EventTableFileReader()
{
    // nothing
}

// load
bool EventTableFileReader::load(const QString& file_name)
{
    event_file_path_ = file_name;
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    // load event type table
    QTextStream file_stream(&file); 
    QString group_id;
    while (true)
    {
        QString one_line = file_stream.readLine();
        if (one_line.isEmpty())
        {
            break;
        }
        if (one_line.indexOf("### 0x") == 0)
        {
            group_id = one_line.mid(4, 6);
            if (!group_id2name_.contains(group_id))
            {
                QString group_name;
                int32 begin_name = one_line.indexOf(QRegExp("[^ \t]"), 10);
                if (begin_name != -1)
                {
                    group_name = one_line.mid(begin_name);
                }
                group_id2name_[group_id] = group_name;
                event_group_ids_ << group_id;
            }
        }
        else if (one_line.indexOf("0x") == 0)
        {
            uint16 event_type_id = one_line.mid(0, 6).toInt(0, 16);
            if (!event_type2name_.contains(event_type_id))
            {
                EventItem item;
                int32 begin_name = one_line.indexOf(QRegExp("[^ \t]"), 6);
                if (begin_name != -1)
                {
                    item.name = one_line.mid(begin_name);
                }
                item.group_id = group_id;
                event_type2name_[event_type_id] = item;
                event_types_ << event_type_id;
            }
        }
    }
    file.close();
    qSort(event_types_);
    qSort(event_group_ids_);
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
    load (event_file_path_);
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
