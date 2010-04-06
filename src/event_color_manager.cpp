// event_color_manager.cpp

#include "event_color_manager.h"
#include "file_handling_impl/event_table_file_reader.h"

#include <QSettings>

namespace BioSig_
{

// constructor
EventColorManager::EventColorManager(EventTableFileReader& event_table_reader)
: event_table_file_reader_(event_table_reader)
{
    // nothing
}

// get event color
const QColor& EventColorManager::getEventColor (EventID id) const
{
    Int2ColorMap::const_iterator it = event_type2color_.find(id);
    if (it == event_type2color_.end())
    {
        it = event_type2color_.find(-1);
    }
    return *it;
}

// set event color
void EventColorManager::setEventColor (EventID id, const QColor& color)
{
    event_type2color_[id] = color; 
}

// load settings
void EventColorManager::loadSettings()
{
    // get event types from event table reader
    QColor alpha_color(0, 0, 0, 50);
    event_type2color_[-1] = alpha_color;
    EventTableFileReader::IntIterator it;
    for (it = event_table_file_reader_.eventTypesBegin();
         it != event_table_file_reader_.eventTypesEnd();
         it++)
    {
        event_type2color_[*it] = QColor(0xff, 0, 0, 50);
    }

    QSettings settings("SigViewer");
    settings.beginGroup("EventColorManager");
    int size = settings.beginReadArray("event");
    for (int i = 0; i < size; i++)
    {
        settings.setArrayIndex(i);
        int32 id = settings.value("id").toInt();
        QColor color = settings.value("color").toString();
        color.setAlpha(settings.value("alpha").toInt());
        event_type2color_[id] = color;
    }
    settings.endArray();
    settings.endGroup();
}

// save settings
void EventColorManager::saveSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("EventColorManager");
    settings.beginWriteArray("event");
    int32 i = 0;
    for (Int2ColorMap::const_iterator it = event_type2color_.begin();
         it != event_type2color_.end();
         it++)
    {
        if (it.key() < 0)
        {
            continue;
        }
        settings.setArrayIndex(i++);
        settings.setValue("id", it.key());
        settings.setValue("color", it->name());
        settings.setValue("alpha", it->alpha());
    }
    settings.endArray();
    settings.endGroup();
}

} // namespace BioSig_
