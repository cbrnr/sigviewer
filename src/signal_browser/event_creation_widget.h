#ifndef EVENT_CREATION_WIDGET_H
#define EVENT_CREATION_WIDGET_H

#include "ui_event_creation_toolbox.h"

#include <QWidget>

namespace BioSig_
{

class EventCreationWidget : public QWidget
{
Q_OBJECT
public:
    explicit EventCreationWidget (QWidget* parent = 0);

signals:

public slots:

private:
    Ui::EventCreationToolbox ui_;
};

}

#endif // EVENT_CREATION_WIDGET_H
