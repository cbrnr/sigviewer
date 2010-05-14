#include "color_manager.h"
#include "../file_handling_impl/event_table_file_reader.h"
#include "../application_context.h"

#include <QSettings>
#include <QDebug>

namespace BioSig_
{

// constructor
ColorManager::ColorManager ()
{
    qDebug() << "constructing event color manager";

}

ColorManager::~ColorManager ()
{
    qDebug() << "destructing event color manager";
}


// get event color
const QColor& ColorManager::getEventColor (EventType type) const
{
    EventColorMap::const_iterator it = event_type2color_.find(type);
    if (it == event_type2color_.end())
    {
        it = event_type2color_.find(-1);
    }
    return *it;
}

// set event color
void ColorManager::setEventColor (EventType type, const QColor& color)
{
    event_type2color_[type] = color;
}

// load settings
void ColorManager::loadSettings()
{
    // get event types from event table reader
    QColor alpha_color(0, 0, 0, 50);
    event_type2color_[-1] = alpha_color;
    EventTableFileReader::IntIterator it;
    for (it = ApplicationContext::getInstance()->getEventTableFileReader()->eventTypesBegin();
         it != ApplicationContext::getInstance()->getEventTableFileReader()->eventTypesEnd();
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
void ColorManager::saveSettings()
{
    qDebug() << "color manager save settings";
    QSettings settings("SigViewer");
    settings.beginGroup("EventColorManager");
    settings.beginWriteArray("event");
    int32 i = 0;
    for (EventColorMap::const_iterator it = event_type2color_.begin();
         it != event_type2color_.end();
         it++)
    {
        if (it.key() == UNDEFINED_EVENT_TYPE)
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
