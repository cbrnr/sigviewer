#ifndef EVENT_TABLE_WIDGET_H
#define EVENT_TABLE_WIDGET_H


#include "ui_event_table_widget.h"

#include <QWidget>

namespace BioSig_
{

class EventTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EventTableWidget(QWidget *parent = 0);
    ~EventTableWidget();

private:
    Ui::EventTableWidget ui_;
};

}

#endif // EVENT_TABLE_WIDGET_H
