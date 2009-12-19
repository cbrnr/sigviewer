#ifndef EVENT_INFO_DOCKWIDGET_H
#define EVENT_INFO_DOCKWIDGET_H

#include "../base/signal_event.h"

#include <QDockWidget>
#include <QSharedPointer>
#include <QLabel>
#include <QObject>
#include <QVBoxLayout>

namespace BioSig_
{

class EventTableFileReader;

class EventInfoDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    EventInfoDockWidget(QWidget* parent, EventTableFileReader const& event_table_file_reader);
    virtual ~EventInfoDockWidget ();

public slots:
    void updateEvent (QSharedPointer<SignalEvent const> selected_signal_event);

private:
    EventTableFileReader const& event_table_file_reader_;

    QWidget* content_widget_;
    QLabel* type_label_;
    QLabel* id_label_;
    QLabel* duration_label_;
    QLabel* start_label_;
    QVBoxLayout* layout_;
};

}

#endif // EVENT_INFO_DOCKWIDGET_H
