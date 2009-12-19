// event_table_file_reader.h

#ifndef EVENT_TABLE_FILE_READER
#define EVENT_TABLE_FILE_READER

#include "user_types.h"

#include <QStringList>
#include <QMap>
#include <QList>

class QTextStream;

namespace BioSig_
{

// event table file reader
class EventTableFileReader
{
public:
    typedef QList<uint16>::const_iterator IntIterator;
    typedef QStringList::const_iterator StringIterator;

    EventTableFileReader();
    ~EventTableFileReader();

    void setLogStream(QTextStream* log_stream);

    bool load(const QString& file_name);

    StringIterator getGroupIdBegin();
    StringIterator getGroupIdEnd();
    QString getEventGroupName(const QString& group_id);

    IntIterator eventTypesBegin();
    IntIterator eventTypesEnd();
    QString getEventName(uint16 event_type_id) const;
    QString getEventGroupId(uint16 event_type_id);

private:
    struct EventItem
    {
        QString name;
        QString group_id;
    };

    typedef QList<uint16> IntList;
    typedef QMap<uint16, EventItem> Int2EventItemMap;
    typedef QMap<QString, QString> String2StringMap;

    // not allowed
    EventTableFileReader(const EventTableFileReader& src);
    const EventTableFileReader& operator=(const EventTableFileReader& src);

    IntList event_types_;
    QStringList event_group_ids_;
    Int2EventItemMap event_type2name_;
    String2StringMap group_id2name_;

    QTextStream* log_stream_;
};

} // namespace BioSig_

#endif
