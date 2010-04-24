#include "gui_helper_functions.h"
#include "channel_selection_dialog.h"
#include "select_shown_channels_dialog.h"
#include "../application_context.h"

#include <QInputDialog>

namespace BioSig_
{

namespace GuiHelper
{

//-----------------------------------------------------------------------------
QSharedPointer<SignalEvent const> getSelectedEvent ()
{
    EventID id = getSelectedEventID ();
    QSharedPointer<EventManager> event_manager = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager();
    QSharedPointer<SignalEvent const> event = event_manager->getEvent (id);

    return event;
}


//-----------------------------------------------------------------------------
EventID getSelectedEventID ()
{
    QSharedPointer<MainWindowModel> mw_model = ApplicationContext::getInstance()->getMainWindowModel ();
    if (mw_model.isNull())
        return UNDEFINED_EVENT_ID;
    QSharedPointer<SignalVisualisationModel> sv_model = mw_model->getCurrentSignalVisualisationModel ();
    if (sv_model.isNull())
        return UNDEFINED_EVENT_ID;
    return sv_model->getSelectedEvent ();
}

//-----------------------------------------------------------------------------
EventType selectEventType (EventType preselected_type)
{
    EventType new_type = UNDEFINED_EVENT_TYPE;

    QSharedPointer<MainWindowModel> mw_model = ApplicationContext::getInstance()->getMainWindowModel ();
    QSharedPointer<SignalVisualisationModel> sv_model = mw_model->getCurrentSignalVisualisationModel ();
    QSharedPointer<EventManager> event_manager = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager();

    std::set<EventType> types = sv_model->getShownEventTypes ();
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
ChannelID selectChannel (ChannelID preselected_channel_id)
{
    int32 current_item = 0;
    QStringList channel_list;

    channel_list.append (QObject::tr("All Channels"));

    QSharedPointer<MainWindowModel> mw_model = ApplicationContext::getInstance()->getMainWindowModel ();
    QSharedPointer<SignalVisualisationModel> sv_model = mw_model->getCurrentSignalVisualisationModel ();
    QSharedPointer<ChannelManager> channel_manager = ApplicationContext::getInstance()->getCurrentFileContext()->getChannelManager();

    std::set<ChannelID> shown_channels = sv_model->getShownChannels ();

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
std::set<ChannelID> selectShownChannels (ChannelID hide_channel)
{
    QSharedPointer<MainWindowModel> mw_model = ApplicationContext::getInstance()->getMainWindowModel ();
    QSharedPointer<SignalVisualisationModel> sv_model = mw_model->getCurrentSignalVisualisationModel ();
    std::set<ChannelID> channels = sv_model->getShownChannels ();
    channels.erase (hide_channel);
    SelectShownChannelsDialog dialog (QObject::tr("Copy Event to Channels"),
                                      channels);
    dialog.exec ();
    return dialog.getSelectedChannels ();
}


//-----------------------------------------------------------------------------
std::set<ChannelID> selectChannels (QSharedPointer<ChannelManager> channel_manager,
                                    QString const& file_name,
                                    QSharedPointer<SignalVisualisationModel> vis_model)
{
    ChannelSelectionDialog channel_dialog (channel_manager, file_name);
    std::set<ChannelID> pre_selected_channels;
    if (!vis_model.isNull())
        pre_selected_channels = vis_model->getShownChannels ();

    bool empty_selection = pre_selected_channels.size () == 0;

    for (ChannelID channel_id = 0;
         channel_id < channel_manager->getNumberChannels();
         channel_id++)
    {
        bool show_channel = empty_selection ||
                            (pre_selected_channels.count(channel_id) > 0);

        if (show_channel)
            channel_dialog.setSelected (channel_id, show_channel);
    }

    channel_dialog.loadSettings();
    channel_dialog.exec();
    channel_dialog.saveSettings();

    if (channel_dialog.result() == QDialog::Rejected)
        return pre_selected_channels;

    std::set<ChannelID> selected_channels;
    for (ChannelID channel_id = 0;
         channel_id < channel_manager->getNumberChannels();
         channel_id++)
    {
        if (channel_dialog.isSelected (channel_id))
            selected_channels.insert (channel_id);
    }
    return selected_channels;
}

}

}
