#include "color_manager.h"

#include <QSettings>
#include <QDebug>
#include <QDomDocument>
#include <QFile>

namespace SigViewer_
{

const char* ColorManager::DEFAULT_CHANNEL_COLOR_SETTING_ = "Default Channel Color";

//-----------------------------------------------------------------------------
ColorManager::ColorManager ()
{
    qDebug() << "constructing event color manager";
    loadDefaultEventColors ();
    loadSettings();
}

//-----------------------------------------------------------------------------
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
        return default_channel_color_;
}

//-----------------------------------------------------------------------------
QColor ColorManager::getDefaultChannelColor () const
{
    return default_channel_color_;
}

//-----------------------------------------------------------------------------
void ColorManager::setDefaultChannelColor (QColor const& color)
{
    default_channel_color_ = color;
    QSettings settings ("SigViewer");
    settings.beginGroup ("ColorManager");
    settings.setValue (DEFAULT_CHANNEL_COLOR_SETTING_, color);
    settings.endGroup ();

    default_channel_color_ = color;
}

//-----------------------------------------------------------------------------
void ColorManager::setChannelColor (ChannelID channel_id, QColor const& color)
{
    channel_color_map_[channel_id] = color;
}


//-----------------------------------------------------------------------------
QColor ColorManager::getEventColor (EventType type) const
{
    if (type == 800)
        qDebug () << "ups";
    EventColorMap::const_iterator it = event_type2color_.find(type);
    if (it == event_type2color_.end())
    {
        if (default_event_colors_.contains (type))
            return default_event_colors_[type];
        else
            return default_event_color_;
    }
    else
        return *it;
}

//-----------------------------------------------------------------------------
QColor ColorManager::getDefaultEventColor (EventType type) const
{
    if (default_event_colors_.contains (type))
        return default_event_colors_[type];
    else
        return default_event_color_;
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
        event_type2color_[*it] = default_event_colors_[*it];
    }

    QSettings settings("SigViewer");
    settings.beginGroup("ColorManager");

    default_channel_color_ = Qt::darkBlue;
    if (settings.contains (DEFAULT_CHANNEL_COLOR_SETTING_))
        default_channel_color_ = settings.value (DEFAULT_CHANNEL_COLOR_SETTING_).value<QColor>();
    else
        settings.setValue (DEFAULT_CHANNEL_COLOR_SETTING_, default_channel_color_);

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
        ChannelID id = settings.value("id").toInt ();
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

//-----------------------------------------------------------------------------
void ColorManager::loadDefaultEventColors ()
{
    QFile color_settings_file (":/color_settings.xml");
    QDomDocument color_doc;
    QString error_msg;
    int error_line;
    color_doc.setContent (&color_settings_file, true, &error_msg, &error_line);
    QDomElement root = color_doc.documentElement();

    QDomNodeList default_elements = root.elementsByTagName("default");
    if (default_elements.size() != 1)
        return;
    QDomElement default_node = default_elements.at(0).toElement();
    QColor default_color;
    default_color.setAlpha (default_node.attribute("alpha").toUInt());
    default_color.setRed (default_node.attribute("red").toUInt());
    default_color.setGreen (default_node.attribute("green").toUInt());
    default_color.setBlue (default_node.attribute("blue").toUInt());

    default_event_colors_[UNDEFINED_EVENT_TYPE] = default_color;
    default_event_color_ = default_color;

    QDomNodeList event_elements = root.elementsByTagName("event");
    for (int index = 0; index < event_elements.size(); index++)
    {
        QDomElement element = event_elements.at(index).toElement();
        QColor event_color = default_color;
        if (element.hasAttribute("red"))
            event_color.setRed(element.attribute("red").toUInt());
        if (element.hasAttribute("green"))
            event_color.setGreen(element.attribute("green").toUInt());
        if (element.hasAttribute("blue"))
            event_color.setBlue(element.attribute("blue").toUInt());
        if (element.hasAttribute("alpha"))
            event_color.setAlpha(element.attribute("alpha").toUInt());
        default_event_colors_[element.attribute("type").toInt(0, 16)] = event_color;
    }

    EventTableFileReader::IntIterator it;
    for (it = event_table_file_reader_.eventTypesBegin();
         it != event_table_file_reader_.eventTypesEnd();
         it++)
    {
        if (!default_event_colors_.contains(*it))
            default_event_colors_[*it] = default_color;
    }
}

} // namespace SigViewer_
