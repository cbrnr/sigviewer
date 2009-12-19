#include "event_info_dockwidget.h"

#include "../base/event_table_file_reader.h"

#include <QGridLayout>

#include <cmath>

namespace BioSig_
{

//-------------------------------------------------------------------
EventInfoDockWidget::EventInfoDockWidget(QWidget* parent,
                                         EventTableFileReader const& event_table_file_reader)
                                             : QDockWidget ("Selected Event", parent, Qt::Drawer),
      event_table_file_reader_ (event_table_file_reader)
{
    content_widget_ = new QWidget (this);
    layout_ = new QVBoxLayout(content_widget_);
    layout_->setSpacing(2);
    content_widget_->setLayout(layout_);

    QGridLayout* info_layout = new QGridLayout (content_widget_);
    info_layout->setVerticalSpacing(1);
    layout_->addLayout(info_layout);

    type_label_ = new QLabel ();
    type_label_->setWordWrap(true);
    id_label_ = new QLabel ();
    duration_label_ = new QLabel ();
    start_label_ = new QLabel ();
    info_layout->addWidget(new QLabel(QString("Type")), 1, 1);
    info_layout->addWidget(type_label_, 1, 2);
    info_layout->addWidget(new QLabel(QString("ID")), 2, 1);
    info_layout->addWidget(id_label_, 2, 2);
    info_layout->addWidget(new QLabel(QString("Begin")), 3, 1);
    info_layout->addWidget(start_label_, 3, 2);
    info_layout->addWidget(new QLabel(QString("Duration")), 4, 1);
    info_layout->addWidget(duration_label_, 4, 2);
    setWidget(content_widget_);
}

//-------------------------------------------------------------------
EventInfoDockWidget::~EventInfoDockWidget()
{
    // nothing to do here
}

//-------------------------------------------------------------------
void EventInfoDockWidget::updateEvent (QSharedPointer<SignalEvent const> selected_signal_event)
{
    type_label_->setText(event_table_file_reader_.getEventName(selected_signal_event->getType()));
    id_label_->setText(QString::number(selected_signal_event->getId()));
    duration_label_->setText(QString::number(round(selected_signal_event->getDurationInSec()*100)/100) + QString(" s"));
    start_label_->setText(QString::number(round(selected_signal_event->getPositionInSec()*100)/100) + QString(" s"));
}


}
