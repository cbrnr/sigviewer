#ifndef EVENT_TIME_SELECTION_DIALOG_H
#define EVENT_TIME_SELECTION_DIALOG_H

#include "base/sigviewer_user_types.h"
#include "file_handling/event_manager.h"
#include "file_handling/channel_manager.h"
#include "ui_event_time_selection_dialog.h"

#include <QDialog>
#include <QString>
#include <QSharedPointer>

#include <set>

namespace SigViewer_
{

class EventTimeSelectionDialog : public QDialog
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    EventTimeSelectionDialog (std::set<EventType> const& shown_event_types,
                              std::set<ChannelID> const& shown_channels,
                              ChannelManager const& channel_manager,
                              QSharedPointer<EventManager const> event_manager);

    //-------------------------------------------------------------------------
    std::set<ChannelID> getSelectedChannels () const;

    //-------------------------------------------------------------------------
    EventType getSelectedEventType () const;

    //-------------------------------------------------------------------------
    float getSecondsBeforeEvent () const;

    //-------------------------------------------------------------------------
    float getLengthInSeconds () const;

private slots:
    //-------------------------------------------------------------------------
    void on_unselect_all_button__clicked ();

    //-------------------------------------------------------------------------
    void on_select_all_button__clicked ();

    //-------------------------------------------------------------------------
    void on_event_combo_box__currentIndexChanged (int combo_box_index);

    //-------------------------------------------------------------------------
    void on_add_before_spinbox__valueChanged (double value);

    //-------------------------------------------------------------------------
    void on_list_widget__itemSelectionChanged ();

    //-------------------------------------------------------------------------
    void on_length_spinbox__valueChanged (double value);

private:
    //-------------------------------------------------------------------------
    void updateOkEnabled ();

    std::set<EventType> shown_event_types_;
    std::set<ChannelID> shown_channels_;
    QSharedPointer<EventManager const> event_manager_;

    double start_before_value_;
    Ui::EventTimeSelectionDialog ui_;
};

}

#endif // EVENT_TIME_SELECTION_DIALOG_H
