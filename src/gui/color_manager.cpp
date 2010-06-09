#include "color_manager.h"

#include <QSettings>
#include <QDebug>

namespace BioSig_
{

QColor const ColorManager::DEFAULT_EVENT_COLOR_ = QColor (200, 0, 0, 25);
QColor const ColorManager::DEFAULT_CHANNEL_COLOR_ = Qt::darkBlue;


// constructor
ColorManager::ColorManager ()
{
    qDebug() << "constructing event color manager";
    loadSettings();
}

ColorManager::~ColorManager ()
{
    qDebug() << "destructing event color manager";
}

//-----------------------------------------------------------------------------
QColor ColorManager::getChannelColor (ChannelID channel_id) const
{
    if (channel_color_map_.contains (channel_id))
        return channel_color_map_[channel_id];
    else
        return DEFAULT_CHANNEL_COLOR_;
}

//-----------------------------------------------------------------------------
void ColorManager::setChannelColor (ChannelID channel_id, QColor const& color)
{
    channel_color_map_[channel_id] = color;
}


//-----------------------------------------------------------------------------
QColor ColorManager::getEventColor (EventType type) const
{
    EventColorMap::const_iterator it = event_type2color_.find(type);
    if (it == event_type2color_.end())
    {
        return DEFAULT_EVENT_COLOR_;
    }
    else
        return *it;
}

//-----------------------------------------------------------------------------
void ColorManager::setEventColor (EventType type, const QColor& color)
{
    event_type2color_[type] = color;
}

//-----------------------------------------------------------------------------
bool ColorManager::isDark (QColor const& color)
{
    double alpha_factor = color.alpha() / 255.0;
    double tmp = 255 * (1 - alpha_factor);
    float64 y = 0.299 * (tmp + color.red() * alpha_factor)+
                0.587 * (tmp + color.green() * alpha_factor) +
                0.114 * (tmp + color.blue() * alpha_factor);

    return y < 127;
}


//-----------------------------------------------------------------------------
void ColorManager::loadSettings()
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
    settings.beginGroup("ColorManager");
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

    size = settings.beginReadArray("channel");
    for (int i = 0; i < size; i++)
    {
        settings.setArrayIndex(i);
        ChannelID id = settings.value("id").toFloat ();
        QColor color = settings.value("color").toString();
        channel_color_map_[id] = color;
    }
    settings.endArray();
    settings.endGroup();
}

//-----------------------------------------------------------------------------
void ColorManager::saveSettings()
{
    qDebug() << "color manager save settings";
    QSettings settings("SigViewer");
    settings.beginGroup("ColorManager");
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

    i = 0;
    settings.beginWriteArray("channel");
    foreach (ChannelID id, channel_color_map_.keys ())
    {
        settings.setArrayIndex (i++);
        settings.setValue ("id", id);
        settings.setValue ("color", channel_color_map_[id]);
    }
    settings.endArray();

    settings.endGroup();
}

} // namespace BioSig_
