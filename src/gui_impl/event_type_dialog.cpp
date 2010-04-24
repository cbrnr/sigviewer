// event_type_dialog.cpp

#include "event_type_dialog.h"
#include "file_handling_impl/event_table_file_reader.h"
#include "event_color_manager.h"
#include "application_context.h"

#include <QPushButton>
#include <QPainter>
#include <QColorDialog>
#include <QInputDialog>
#include <QMap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QHeaderView>
#include <QSettings>

#define max(a,b) ((a) > (b) ? (a) : (b))

namespace BioSig_
{

// is dark
inline bool isDark(const QColor& color)
{
    double alpha_factor = color.alpha() / 255.0;
    double tmp = 255 * (1 - alpha_factor);
    float64 y = 0.299 * (tmp + color.red() * alpha_factor)+
                0.587 * (tmp + color.green() * alpha_factor) +
                0.114 * (tmp + color.blue() * alpha_factor);

    return y < 127;
}

// constructor
EventTypeDialog::EventTypeDialog(const QString& caption,
                                 QWidget* parent)
 : QDialog(parent)
{
    setWindowTitle(caption);

    QVBoxLayout* top_layout = new QVBoxLayout(this);

    top_layout->setMargin(10);
    top_layout->setSpacing(10);
    event_tree_widget_ = new QTreeWidget(this);
    top_layout->addWidget(event_tree_widget_);
    QHBoxLayout* button_layout = new QHBoxLayout;
    button_layout->setMargin(0);
    top_layout->addLayout(button_layout);
    button_layout->addStretch(1);
    ok_button_ = new QPushButton(tr("OK"), this);
    button_layout->addWidget(ok_button_);
    cancel_button_ = new QPushButton(tr("Cancel"), this);
    button_layout->addWidget(cancel_button_);
    button_layout->addStretch(1);
    buildTree();
    resize(550, 600);
    top_layout->activate();

    connect(ok_button_, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancel_button_, SIGNAL(clicked()), this, SLOT(reject()));
    connect(event_tree_widget_, SIGNAL(itemClicked(QTreeWidgetItem* , int)),
            this, SLOT(itemClicked(QTreeWidgetItem*, int)));
    connect(event_tree_widget_, SIGNAL(itemChanged(QTreeWidgetItem* , int)),
            this, SLOT(itemChanged(QTreeWidgetItem*, int)));
}

// load settings
void EventTypeDialog::loadSettings()
{
    QSettings settings("SigViewer");

    settings.beginGroup("EventTypeDialog");
    resize(settings.value("size", QSize(550, 600)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    event_tree_widget_->header()->resizeSection(0, settings.value("col0_width", 100).toInt());
    event_tree_widget_->header()->resizeSection(1, settings.value("col1_width", 150).toInt());
    event_tree_widget_->header()->resizeSection(2, settings.value("col2_width", 100).toInt());
    settings.endGroup();
}

// save settings
void EventTypeDialog::saveSettings()
{
    QSettings settings("SigViewer");

    settings.beginGroup("EventTypeDialog");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("col0_width", event_tree_widget_->header()->sectionSize(0));
    settings.setValue("col1_width", event_tree_widget_->header()->sectionSize(1));
    settings.setValue("col2_width", event_tree_widget_->header()->sectionSize(2));
    settings.endGroup();
}

// build tree
void EventTypeDialog::buildTree()
{
    QSharedPointer<EventTableFileReader> event_table_file_reader = ApplicationContext::getInstance()->getEventTableFileReader();
    event_tree_widget_->setRootIsDecorated(true);
    QStringList header_labels;
    header_labels << tr("Event Type") << tr("Color") << tr("Alpha");
    event_tree_widget_->setHeaderLabels(header_labels);
    event_tree_widget_->header()->setResizeMode(QHeaderView::Interactive);

    // root
    QTreeWidgetItem * root_item = new QTreeWidgetItem(event_tree_widget_);
    root_item->setText(0, tr("All Events"));
    root_item->setFlags(root_item->flags() ^ Qt::ItemIsSelectable);
//                        Qt::ItemIsTristate);
    root_item->setCheckState(0, Qt::Unchecked);
    event_tree_widget_->setItemExpanded(root_item, true);

    // build groups
    QMap<QString, QTreeWidgetItem*> group_id2list_item;
    EventTableFileReader::StringIterator group_it;

    for (group_it = event_table_file_reader->getGroupIdBegin();
         group_it != event_table_file_reader->getGroupIdEnd();
         group_it++)
    {
        QString group_name =
            event_table_file_reader->getEventGroupName(*group_it) +
            QString(" (%1)").arg(*group_it);

        QTreeWidgetItem * group_item = new QTreeWidgetItem(root_item);
        group_item->setFlags(group_item->flags() ^ Qt::ItemIsSelectable);
//                             Qt::ItemIsTristate);
        group_item->setCheckState(0, Qt::Unchecked);
        group_item->setText(0, group_name);
        group_id2list_item[*group_it] = group_item;
    }

    // build events
    EventTableFileReader::IntIterator event_type_it;

    for (event_type_it = event_table_file_reader->eventTypesBegin();
         event_type_it != event_table_file_reader->eventTypesEnd();
         event_type_it++)
    {
        QString group_name
            = event_table_file_reader->getEventGroupId(*event_type_it);

        QString event_name
            = event_table_file_reader->getEventName(*event_type_it);

        QTreeWidgetItem * event_item
            = new QTreeWidgetItem(group_id2list_item[group_name]);

        event_item->setCheckState(0, Qt::Unchecked);
        event_item->setFlags(event_item->flags() ^ Qt::ItemIsSelectable);
        event_item->setText(0, event_name + " " +
                            QString("(0x%1)").arg(*event_type_it, 4, 16)
                                             .replace(' ', '0'));

        QColor color = ApplicationContext::getInstance()->getEventColorManager()->getEventColor(*event_type_it);
        event_item->setBackgroundColor(2, color);
        event_item->setTextColor(2, isDark(color) ? Qt::white : Qt::black);
        event_item->setTextAlignment(2, Qt::AlignHCenter);
        event_item->setText(2, QString("%1").arg(color.alpha()));
        color.setAlpha(255);
        event_item->setBackgroundColor(1, color);
        event_item->setTextColor(1, isDark(color) ? Qt::white : Qt::black);
        event_item->setTextAlignment(1, Qt::AlignHCenter);
        event_item->setText(1, color.name());
    }
}

// item clicked
void EventTypeDialog::itemClicked(QTreeWidgetItem* item, int column)
{
    if ((column != 1 && column != 2) || (item->childCount() > 0))
    {
        return;
    }

    QColor color = item->backgroundColor(2);
    int32 alpha = color.alpha();

    if (column == 1)
    {
        color = QColorDialog::getColor(color, this);
        color.setAlpha(alpha);
    }
    else
    {
        color.setAlpha(QInputDialog::getInteger(this, tr("Alpha"), "Enter new Value",
                                                alpha, 0, 255, 25));
    }

    item->setBackgroundColor(2, color);
    item->setTextColor(2, isDark(color) ? Qt::white : Qt::black);
    item->setText(2, QString("%1").arg(color.alpha()));
    color.setAlpha(255);
    item->setBackgroundColor(1, color);
    item->setTextColor(1, isDark(color) ? Qt::white : Qt::black);
    item->setText(1, color.name());
}

// store colors
void EventTypeDialog::storeColors()
{
    QTreeWidgetItem* root_item = event_tree_widget_->topLevelItem(0);

    for (int32 group_nr = 0; group_nr < root_item->childCount(); group_nr++)
    {
        QTreeWidgetItem* group_item  = root_item->child(group_nr);

        for (int32 nr = 0; nr < group_item->childCount(); nr++)
        {
            QTreeWidgetItem* event_item = group_item->child(nr);
            QString tmp = event_item->text(0);
            int32 type_id = tmp.mid(tmp.lastIndexOf('(') + 1)
                               .remove(')').toInt(0, 16);

            ApplicationContext::getInstance()->getEventColorManager()->setEventColor(type_id,
                                               event_item->backgroundColor(2));
        }
    }
}

// get shown types
void EventTypeDialog::getShownTypes(IntList& shown_type_set)
{
    shown_type_set.clear();
    QTreeWidgetItem* root_item = event_tree_widget_->topLevelItem(0);

    for (int32 group_nr = 0; group_nr < root_item->childCount(); group_nr++)
    {
        QTreeWidgetItem* group_item  = root_item->child(group_nr);

        for (int32 nr = 0; nr < group_item->childCount(); nr++)
        {
            QTreeWidgetItem* event_item = group_item->child(nr);

            if (event_item->checkState(0) == Qt::Checked)
            {
                QString tmp = event_item->text(0);
                int32 type_id = tmp.mid(tmp.lastIndexOf('(') + 1)
                                   .remove(')').toInt(0, 16);
                shown_type_set << type_id;
            }
        }
    }

    qSort(shown_type_set);
}

// set shown types
void EventTypeDialog::setShownTypes(IntList& shown_type_set, const bool all)
{
    IntList::const_iterator iter = shown_type_set.begin();
    QTreeWidgetItem* root_item = event_tree_widget_->topLevelItem(0);

   	if (all)
   		root_item->setCheckState(0, Qt::Checked);

    for (int32 group_nr = 0; group_nr < root_item->childCount(); group_nr++)
    {
        QTreeWidgetItem* group_item  = root_item->child(group_nr);

        for (int32 nr = 0; nr < group_item->childCount();nr++)
        {
            QTreeWidgetItem* event_item = group_item->child(nr);
            QString tmp = event_item->text(0);
            int32 type_id = tmp.mid(tmp.lastIndexOf('(') + 1)
                               .remove(')').toInt(0, 16);

            while(iter != shown_type_set.end() && type_id > *iter)
            {
                iter++;
            }

            if (iter != shown_type_set.end() && type_id == *iter)
            {
                event_item->setCheckState(0, Qt::Checked);
                iter++;
            }
            else
            {
                event_item->setCheckState(0, Qt::Unchecked);
            }
        }
    }
}

// item changed
void EventTypeDialog::itemChanged(QTreeWidgetItem* item ,int)
{
    Qt::CheckState check_state = item->checkState(0);

    if (check_state != Qt::PartiallyChecked)
    {
        for (int32 nr = 0; nr < item->childCount(); nr++)
        {
            QTreeWidgetItem* child = item->child(nr);

            if (child->checkState(0) != check_state)
            {
                child->setCheckState(0, check_state);
            }
        }
    }

    QTreeWidgetItem* parent = item->parent();

    if (parent)
    {
        Qt::CheckState parent_check_state = Qt::PartiallyChecked;

        if (check_state != Qt::PartiallyChecked)
        {
            bool same = true;

            for (int32 nr = 0; same && nr < parent->childCount(); nr++)
            {
                same = parent->child(nr)->checkState(0) == check_state;
            }

            if (same)
            {
                parent_check_state = check_state;
            }
        }

        if (parent->checkState(0) != parent_check_state)
        {
            parent->setCheckState(0, parent_check_state);
        }
    }
}

} //namespace BioSig_

