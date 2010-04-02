// copy_event_dialog.cpp

#include "copy_event_dialog.h"
#include "file_handling/basic_header.h"

#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSettings>

namespace BioSig_
{

// constructor
CopyEventDialog::CopyEventDialog(QPointer<BasicHeader> header,
                                 QWidget* parent)
 : QDialog(parent),
   basic_header_(header)
{
    setWindowTitle(tr("Copy Event to Channels"));
    QVBoxLayout* top_layout = new QVBoxLayout(this);
    top_layout->setMargin(10);
    top_layout->setSpacing(10);
    channel_list_widget_ = new QListWidget(this);
    channel_list_widget_
                    ->setSelectionMode(QAbstractItemView::ExtendedSelection);
    top_layout->addWidget(channel_list_widget_);
    //QHBoxLayout* button_layout = new QHBoxLayout(top_layout);
    QHBoxLayout* button_layout = new QHBoxLayout(this);
    button_layout->setMargin(10);
    button_layout->addStretch(1);
    ok_button_ = new QPushButton(tr("OK"), this);
    button_layout->addWidget(ok_button_);
    cancel_button_ = new QPushButton(tr("Cancel"), this);
    button_layout->addWidget(cancel_button_);
    button_layout->addStretch(1);
    top_layout->activate();
    connect(ok_button_, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancel_button_, SIGNAL(clicked()), this, SLOT(reject()));
}

// load settings
void CopyEventDialog::loadSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("CopyEventDialog");
    resize(settings.value("size", QSize(800, 600)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
}

// save settings
void CopyEventDialog::saveSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("CopyEventDialog");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

// is selected
bool CopyEventDialog::isSelected(uint32 channel_nr)
{
    QList<QListWidgetItem*> find_list;
    find_list = channel_list_widget_
        ->findItems(QString("(%1)").arg(channel_nr + 1),
                    Qt::MatchStartsWith);
    return (find_list.size() > 0 && 
            channel_list_widget_->isItemSelected(find_list[0]));
}

// add selectable channel
void CopyEventDialog::addSelectableChannel(uint32 channel_nr)
{
    const SignalChannel& channel = basic_header_->getChannel(channel_nr);
    channel_list_widget_->addItem(QString("(%1) %2").arg(channel_nr + 1)
                                            .arg(channel.getLabel()));
}

} //namespace BioSig_
