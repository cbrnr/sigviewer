#ifndef EVENT_TABLE_WIDGET_H
#define EVENT_TABLE_WIDGET_H


#include "ui_event_table_widget.h"
#include "file_handling/event_manager.h"
#include "gui/event_view.h"
#include "file_handling/channel_manager.h"
#include "event_table_view_model.h"
#include "tab_context.h"

#include <QWidget>

namespace SigViewer_
{

namespace Tests_ { class EventTableWidgetTests;}

//-----------------------------------------------------------------------------
class EventTableWidget : public QWidget
{
    friend class SigViewer_::Tests_::EventTableWidgetTests;

    Q_OBJECT

public:
    //-------------------------------------------------------------------------
    EventTableWidget (QSharedPointer<TabContext> tab_context,
                      QSharedPointer<EventManager> event_manager,
                      ChannelManager const& channel_manager,
                      QWidget *parent = 0);

    //-------------------------------------------------------------------------
    virtual ~EventTableWidget ();

    //-------------------------------------------------------------------------
    virtual QList<EventID> getSelectedEvents () const;

    //-------------------------------------------------------------------------
    QSharedPointer<EventManager const> getEventManager () const {return event_manager_;}

    //-------------------------------------------------------------------------
    QSharedPointer<EventManager> getEventManager () {return event_manager_;}

    //-------------------------------------------------------------------------
    QSharedPointer<EventView> getEventView ();

private slots:
    void addToTable (QSharedPointer<SignalEvent const> event);
    void removeFromTable (EventID event);
    void updateEventEntry (EventID event_id);

    void on_event_table__itemSelectionChanged ();

private:
    void showEvent (QShowEvent* event);
    void buildTable ();

    static int const ID_INDEX_ = 0;
    static int const POSITION_INDEX_ = 1;
    static int const DURATION_INDEX_ = 2;
    static int const CHANNEL_INDEX_ = 3;
    static int const TYPE_INDEX_ = 4;

    int precision_;

    Ui::EventTableWidget ui_;
    QSharedPointer<TabContext> tab_context_;
    QSharedPointer<EventManager> event_manager_;
    QSharedPointer<EventTableViewModel> event_table_view_model_;
    ChannelManager const& channel_manager_;
};

}

#endif // EVENT_TABLE_WIDGET_H
