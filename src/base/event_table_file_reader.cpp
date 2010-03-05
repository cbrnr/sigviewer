// event_table_file_reader.cpp

#include "event_table_file_reader.h"

#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QTextStream>

namespace BioSig_
{

// constructor
EventTableFileReader::EventTableFileReader()
: log_stream_(0)
{
    // nothing
}

// destructor
EventTableFileReader::~EventTableFileReader()
{
    // nothing
}

// set log stream
void EventTableFileReader::setLogStream(QTextStream* log_stream)
{
    log_stream_ = log_stream;
}

// load
bool EventTableFileReader::load(const QString& file_name)
{
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (log_stream_)
        {
            *log_stream_ << "EventTableFileReader::load '" << file_name
                         << "' Error: open file\n";
        }
        return false;
    }

    // load event type table
    if (log_stream_)
    {
        *log_stream_ << "EventTableFileReader::load '" << file_name << "'\n";
    }
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
EventTableFileReader::StringIterator EventTableFileReader::getGroupIdBegin()
{
    return event_group_ids_.begin();
}

// get group id end
EventTableFileReader::StringIterator EventTableFileReader::getGroupIdEnd()
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

// get event group id
QString EventTableFileReader::getEventGroupId(uint16 event_type_id)
{
    Int2EventItemMap::const_iterator it = event_type2name_.find(event_type_id);
    return it != event_type2name_.end() ? it.value().group_id
                                        : "";
}

//-------------------------------------------------------------------
std::set<uint16> EventTableFileReader::getAllEventTypes () const
{
    std::set<uint16> type_set;
    for (IntIterator it = event_types_.begin();
         it != event_types_.end();
         ++it)
    {
        type_set.insert (*it);
    }
    return type_set;
}


// get event group name
QString EventTableFileReader::getEventGroupName(const QString& group_id)
{
    String2StringMap::const_iterator it = group_id2name_.find(group_id);
    return it != group_id2name_.end() ? it.value()
                                      : "";
}

} // namespace BioSig_
