#include "gui_helper_functions.h"
#include "dialogs/channel_selection_dialog.h"
#include "select_shown_channels_dialog.h"
#include "dialogs/event_types_selection_dialog.h"

#include <QInputDialog>
#include <QFileDialog>
#include <QDebug>
#if QT_VERSION >= 0x040600
#include <QPropertyAnimation>
#endif
#include <QSettings>
#include <QMetaObject>

namespace BioSig_
{

namespace GuiHelper
{

//-----------------------------------------------------------------------------
void animateProperty (QObject* target, QByteArray const& property_name,
                      QVariant const& start_value, QVariant const& end_value,
                      QObject* call_back_object, char const* call_back_slot)
{
    QSettings settings ("SigViewer");
    settings.beginGroup("Animations");
    bool animations_activated = settings.value("activated", false).toBool();
    settings.endGroup();
#if QT_VERSION < 0x040600
    animations_activated = false;
#endif
    if (animations_activated)
    {
#if QT_VERSION >= 0x040600
        QPropertyAnimation* animation = new QPropertyAnimation (target,
                                                                property_name);
        animation->setDuration (400);
        animation->setStartValue (start_value);
        animation->setEndValue (end_value);
        if (call_back_object && call_back_slot)
            call_back_object->connect (animation, SIGNAL(finished()), call_back_slot);
        animation->start (animation->DeleteWhenStopped);
#endif
    }
    else
    {
        target->setProperty (property_name, end_value);
        if (call_back_object && call_back_slot)
        {
           QAction dummy_action (call_back_object);
           call_back_object->connect(&dummy_action, SIGNAL(triggered()), call_back_slot);
           dummy_action.trigger();
        }
    }
}

//-----------------------------------------------------------------------------
QSharedPointer<SignalEvent const> getSelectedEvent (QSharedPointer<SignalVisualisationModel>
                                                    signal_visualisation_model)
{
    EventID id = getSelectedEventID (signal_visualisation_model);
    if (id == UNDEFINED_EVENT_ID)
        return QSharedPointer<SignalEvent const> (0);
    QSharedPointer<EventManager> event_manager = signal_visualisation_model->getEventManager();
    QSharedPointer<SignalEvent const> event = event_manager->getEvent (id);

    return event;
}


//-----------------------------------------------------------------------------
EventID getSelectedEventID (QSharedPointer<SignalVisualisationModel>
                            signal_visualisation_model)
{
    if (signal_visualisation_model.isNull())
        return UNDEFINED_EVENT_ID;
    return signal_visualisation_model->getSelectedEvent ();
}

//-----------------------------------------------------------------------------
EventType selectEventType (EventType preselected_type,
                           QSharedPointer<SignalVisualisationModel> signal_visualisation_model)
{
    EventType new_type = UNDEFINED_EVENT_TYPE;

    QSharedPointer<EventManager> event_manager = signal_visualisation_model->getEventManager();

    std::set<EventType> types = signal_visualisation_model->getShownEventTypes ();
    QStringList event_type_list;
    int32 current_item = 0;

    for (std::set<EventType>::const_iterator it = types.begin();
         it != types.end();
         it++)
    {
        if (preselected_type == *it)
            current_item = event_type_list.size();

        QString event_name = event_manager->getNameOfEventType (*it);
        event_type_list.append (event_name + " " + QString("(%1)")
                                                        .arg(*it,4, 16)
                                                        .replace(' ', '0'));
    }

    bool ok = false;
    QString res = QInputDialog::getItem(0, QObject::tr("Change Type"),
                                        QObject::tr("Select new Type:"),
                                        event_type_list, current_item,
                                        false, &ok);

    if (ok)
        new_type = res.right(5).left(4).toUShort(0, 16);

    return new_type;
}

//-----------------------------------------------------------------------------
std::set<EventType> selectEventTypes (std::set<EventType> const& preselected_type,
                                      QSharedPointer<EventManager const> event_manager,
                                      QSharedPointer<ColorManager> color_manager,
                                      bool enable_color_editing)
{
    std::set<EventType> selected_types;
    if (event_manager.isNull())
        return selected_types;

    EventTypesSelectionDialog dialog (QObject::tr("Select Event Types"),
                                      event_manager, preselected_type,
                                      color_manager,
                                      enable_color_editing, 0);
    dialog.exec();
    selected_types = dialog.getSelectedTypes ();
    if (enable_color_editing)
        dialog.storeColors ();
    return selected_types;
}


//-----------------------------------------------------------------------------
ChannelID selectChannel (ChannelID preselected_channel_id,
                         QSharedPointer<SignalVisualisationModel> signal_visualisation_model)
{
    int32 current_item = 0;
    QStringList channel_list;

    channel_list.append (QObject::tr("All Channels"));

    QSharedPointer<ChannelManager const> channel_manager = signal_visualisation_model->getChannelManager();

    std::set<ChannelID> shown_channels = signal_visualisation_model->getShownChannels ();

    for (std::set<ChannelID>::iterator channel_iter = shown_channels.begin();
         channel_iter != shown_channels.end();
         ++channel_iter)
    {
        if (*channel_iter == preselected_channel_id)
            current_item = channel_list.size();

        channel_list.append(
                QString("(%1) ").arg(*channel_iter + 1) +
                channel_manager->getChannelLabel (*channel_iter));
    }

    bool ok = false;
    QString res = QInputDialog::getItem(0, QObject::tr("Change Channel"),
                                        QObject::tr("Select New Channel:"),
                                        channel_list, current_item,
                                        false, &ok);

    ChannelID new_channel = res.replace(')', '(').section('(', 1, 1).toInt() - 1;

    if (ok)
        return new_channel;
    else
        return preselected_channel_id;
}

//-----------------------------------------------------------------------------
std::set<ChannelID> selectShownChannels (ChannelID hide_channel,
                                         QSharedPointer<SignalVisualisationModel> vis_model)
{
    std::set<ChannelID> channels = vis_model->getShownChannels ();
    channels.erase (hide_channel);
    SelectShownChannelsDialog dialog (QObject::tr("Copy Event to Channels"),
                                      channels,
                                      vis_model->getChannelManager());
    dialog.exec ();
    return dialog.getSelectedChannels ();
}


//-----------------------------------------------------------------------------
std::set<ChannelID> selectChannels (QSharedPointer<ChannelManager const> channel_manager,
                                    QSharedPointer<ColorManager> color_manager,
                                    QString const& file_name,
                                    QSharedPointer<SignalVisualisationModel> vis_model)
{
    ChannelSelectionDialog channel_dialog (channel_manager, file_name, color_manager);
    std::set<ChannelID> pre_selected_channels;
    if (!vis_model.isNull())
        pre_selected_channels = vis_model->getShownChannels ();

    bool empty_selection = pre_selected_channels.size () == 0;

    foreach (ChannelID channel_id, channel_manager->getChannels())
    {
        bool show_channel = empty_selection ||
                            (pre_selected_channels.count(channel_id) > 0);

        if (show_channel)
            channel_dialog.setSelected (channel_id, show_channel);
    }

    channel_dialog.exec();

    if (channel_dialog.result() == QDialog::Rejected)
        return pre_selected_channels;

    std::set<ChannelID> selected_channels;
    foreach (ChannelID channel_id, channel_manager->getChannels())
    {
        if (channel_dialog.isSelected (channel_id))
            selected_channels.insert (channel_id);
    }
    return selected_channels;
}

//-----------------------------------------------------------------------------
QString getFilePathFromSaveAsDialog (QString const& path,
                                     QString const& extensions,
                                     QString const& file_type_description)
{
    QString extension_selection = file_type_description + QObject::tr(" (%1)").arg(extensions);
    QStringList ext_list = extensions.split (" ");
    for (QStringList::iterator it = ext_list.begin();
         it != ext_list.end();
         it++)
    {
        extension_selection += ";; " + *it +" (" + *it + ")";
    }
    extension_selection += ";; *.* (*.*)";
    return QFileDialog::getSaveFileName (0, QObject::tr("Save"),
                                        path, extension_selection);
}


}

}
