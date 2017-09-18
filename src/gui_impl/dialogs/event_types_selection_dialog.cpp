// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "event_types_selection_dialog.h"

#include <QColorDialog>
#include <QInputDialog>
#include <QDesktopWidget>

#define max(a,b) ((a) > (b) ? (a) : (b))

namespace sigviewer
{

//-----------------------------------------------------------------------------
EventTypesSelectionDialog::EventTypesSelectionDialog (QString const& caption,
                                                      QSharedPointer<EventManager const> event_manager,
                                                      std::set<EventType> const& preselected_types,
                                                      QSharedPointer<ColorManager> color_manager,
                                                      QWidget* parent)
    : QDialog(parent),
      event_manager_ (event_manager),
      selected_types_ (preselected_types),
      color_manager_ (color_manager)

{
    ui_.setupUi (this);
    setWindowTitle (caption);
    buildTree (true);
    color_manager_->loadSettings();
}

//-----------------------------------------------------------------------------
void EventTypesSelectionDialog::buildTree (bool only_existing_events)
{
    QStringList header_labels;
    header_labels << tr("Event Type") << tr("Color") << tr("Alpha") << tr("Type Id");
    ui_.tree_widget_->setHeaderLabels (header_labels);
    ui_.tree_widget_->setColumnWidth(ID_COLUMN_INDEX_, 0);

    QDesktopWidget dw;
    int x=dw.width()*0.35;
    int y=dw.height()*0.7;
    resize(x,y);

    ui_.tree_widget_->header()->setSectionResizeMode (QHeaderView::Interactive);
    ui_.tree_widget_->header()->resizeSection (NAME_COLUMN_INDEX_, width() * 0.6);
    ui_.tree_widget_->header()->resizeSection (COLOR_COLUMN_INDEX_, width() * 0.18);
    ui_.tree_widget_->header()->resizeSection (ID_COLUMN_INDEX_, 0);

    ui_.tree_widget_->setColumnHidden (ID_COLUMN_INDEX_, true);

    std::set<EventType> existing_types;
    foreach (EventID event_id, event_manager_->getAllEvents ())
        existing_types.insert (event_manager_->getEvent (event_id)->getType ());

    if (event_manager_->getFileType().startsWith("xdf", Qt::CaseInsensitive))
    {
        foreach (EventType event_type, event_manager_->getEventTypes ())
        {
            if (only_existing_events && !existing_types.count(event_type))
                continue;

            QString event_name
                    = event_manager_->getNameOfEventType (event_type);

            QTreeWidgetItem* event_item = new QTreeWidgetItem (ui_.tree_widget_);

            event_item->setFlags (Qt::ItemIsUserCheckable |
                                  Qt::ItemIsEnabled);

            QColor color = color_manager_->getEventColor(event_type);

            if (selected_types_.count(event_type))
                event_item->setCheckState (CHECKBOX_COLUMN_INDEX_, Qt::Checked);
            else
                event_item->setCheckState (CHECKBOX_COLUMN_INDEX_, Qt::Unchecked);

            event_item->setText (NAME_COLUMN_INDEX_, event_name);

            color.setAlpha (255);

            event_item->setBackgroundColor (COLOR_COLUMN_INDEX_, color);
            event_item->setTextColor (COLOR_COLUMN_INDEX_, ColorManager::isDark(color) ? Qt::white : Qt::black);
            event_item->setText (COLOR_COLUMN_INDEX_, color.name());

            color = color_manager_->getEventColor(event_type);  //reselect the same color but less Alpha value

            event_item->setBackgroundColor (ALPHA_COLUMN_INDEX_, color);
            event_item->setTextColor(ALPHA_COLUMN_INDEX_, ColorManager::isDark(color) ? Qt::white : Qt::black);
            event_item->setText(ALPHA_COLUMN_INDEX_, QString("%1").arg(color.alpha()));

            event_item->setText (ID_COLUMN_INDEX_, QString::number(event_type));
        }
    }
    else
    {
        QMap<QString, QTreeWidgetItem*> group_id2list_item;

        foreach (QString group_id, event_manager_->getEventTypeGroupIDs())
        {
            QString group_name = group_id;
            // TODO: event_table_file_reader.getEventGroupName(*group_it);

            bool show_group = !only_existing_events;
            foreach (EventType group_type, event_manager_->getEventTypes (group_id))
                if (existing_types.count(group_type) && only_existing_events)
                    show_group = true;

            if (show_group)
            {
                QTreeWidgetItem * group_item = new QTreeWidgetItem(ui_.tree_widget_);
                group_item->setFlags (Qt::ItemIsUserCheckable |
                                      Qt::ItemIsTristate |
                                      Qt::ItemIsEnabled);
                group_item->setText (NAME_COLUMN_INDEX_, group_name);
                group_item->setExpanded (true);
                group_id2list_item[group_id] = group_item;

                foreach (EventType event_type, event_manager_->getEventTypes (group_id))
                {
                    if (only_existing_events && !existing_types.count(event_type))
                        continue;

                    QString event_name
                            = event_manager_->getNameOfEventType (event_type);

                    QTreeWidgetItem* event_item
                            = new QTreeWidgetItem (group_item);

                    event_item->setFlags (Qt::ItemIsUserCheckable |
                                          Qt::ItemIsEnabled);

                    QColor color = color_manager_->getEventColor(event_type);

                    if (selected_types_.count(event_type))
                        event_item->setCheckState (CHECKBOX_COLUMN_INDEX_, Qt::Checked);
                    else
                        event_item->setCheckState (CHECKBOX_COLUMN_INDEX_, Qt::Unchecked);

                    event_item->setText (NAME_COLUMN_INDEX_, event_name);

                    color.setAlpha (255);

                    event_item->setBackgroundColor (COLOR_COLUMN_INDEX_, color);
                    event_item->setTextColor (COLOR_COLUMN_INDEX_, ColorManager::isDark(color) ? Qt::white : Qt::black);
                    event_item->setText (COLOR_COLUMN_INDEX_, color.name());

                    color = color_manager_->getEventColor(event_type);  //reselect the same color but less Alpha value

                    event_item->setBackgroundColor (ALPHA_COLUMN_INDEX_, color);
                    event_item->setTextColor(ALPHA_COLUMN_INDEX_, ColorManager::isDark(color) ? Qt::white : Qt::black);
                    event_item->setText(ALPHA_COLUMN_INDEX_, QString("%1").arg(color.alpha()));

                    event_item->setText (ID_COLUMN_INDEX_, QString::number(event_type));
                }
            }
        }
    }

    ui_.tree_widget_->setAnimated(true);
}

//-----------------------------------------------------------------------------
void EventTypesSelectionDialog::on_tree_widget__itemClicked(QTreeWidgetItem* item, int column)
{
    switch (column)
    {
    case COLOR_COLUMN_INDEX_:
        handleColor (item);
        break;
    case ALPHA_COLUMN_INDEX_:
        handleAlpha (item);
        break;
    }
}

//-----------------------------------------------------------------------------
void EventTypesSelectionDialog::storeColors()
{
    if (event_manager_->getFileType().startsWith("xdf", Qt::CaseInsensitive))
    {
        // in XDF files we don't display libbiosig event groups, so top level items are events
        for (int nr = 0; nr < ui_.tree_widget_->topLevelItemCount(); nr++)
        {
            QTreeWidgetItem* event_item = ui_.tree_widget_->topLevelItem(nr);
            EventType type = event_item->text(ID_COLUMN_INDEX_).toUInt();
            color_manager_->setEventColor (type, event_item->backgroundColor(ALPHA_COLUMN_INDEX_));
        }
    }
    else
    {
        // in non-XDF files, top level items are libbiosig event groups
        for (int group_nr = 0; group_nr < ui_.tree_widget_->topLevelItemCount(); group_nr++)
        {
            QTreeWidgetItem* group_item  = ui_.tree_widget_->topLevelItem (group_nr);

            for (int nr = 0; nr < group_item->childCount(); nr++)
            {
                QTreeWidgetItem* event_item = group_item->child(nr);
                EventType type = event_item->text(ID_COLUMN_INDEX_).toUInt();
                color_manager_->setEventColor (type,
                                               event_item->backgroundColor(ALPHA_COLUMN_INDEX_));
            }
        }
    }
    color_manager_->saveSettings();
}

//-----------------------------------------------------------------------------
std::set<EventType> EventTypesSelectionDialog::getSelectedTypes () const
{
    return selected_types_;
}

//-----------------------------------------------------------------------------
void EventTypesSelectionDialog::on_tree_widget__itemChanged (QTreeWidgetItem* item, int column)
{
    if (item->childCount())
        return;

    if (column == CHECKBOX_COLUMN_INDEX_)
        handleSelected (item);
}

//-----------------------------------------------------------------------------
void EventTypesSelectionDialog::on_all_events_button__toggled (bool checked)
{
    if (!checked)
        return;

    ui_.tree_widget_->clear();
    buildTree ();
}

//-----------------------------------------------------------------------------
void EventTypesSelectionDialog::on_existing_events_button__toggled (bool checked)
{
    if (!checked)
        return;

    ui_.tree_widget_->clear();
    buildTree (true);
}

//-----------------------------------------------------------------------------
void EventTypesSelectionDialog::on_select_all_button__clicked ()
{
    for (int index = 0; index < ui_.tree_widget_->topLevelItemCount(); index++)
        ui_.tree_widget_->topLevelItem(index)->setCheckState(CHECKBOX_COLUMN_INDEX_, Qt::Checked);
    selected_types_ = event_manager_->getEventTypes ();
}

//-----------------------------------------------------------------------------
void EventTypesSelectionDialog::on_unselect_all_button__clicked ()
{
    for (int index = 0; index < ui_.tree_widget_->topLevelItemCount(); index++)
        ui_.tree_widget_->topLevelItem(index)->setCheckState(CHECKBOX_COLUMN_INDEX_, Qt::Unchecked);
    selected_types_.clear ();
}

//-----------------------------------------------------------------------------
void EventTypesSelectionDialog::on_reset_colors_button__clicked ()
{
    QTreeWidgetItemIterator color_iterator (ui_.tree_widget_, QTreeWidgetItemIterator::NoChildren);
    while (*color_iterator)
    {
        QColor color = color_manager_->getDefaultEventColor((*color_iterator)->text (ID_COLUMN_INDEX_).toUInt ());
        color.setAlpha (255);
        (*color_iterator)->setBackgroundColor (COLOR_COLUMN_INDEX_, color);
        (*color_iterator)->setTextColor (COLOR_COLUMN_INDEX_, ColorManager::isDark(color) ? Qt::white : Qt::black);
        (*color_iterator)->setText (COLOR_COLUMN_INDEX_, color.name());
        ++color_iterator;
    }

    QTreeWidgetItemIterator alpha_iterator (ui_.tree_widget_, QTreeWidgetItemIterator::NoChildren);
    while (*alpha_iterator)
    {
        QColor color = color_manager_->getDefaultEventColor((*alpha_iterator)->text (ID_COLUMN_INDEX_).toUInt ());
        (*alpha_iterator)->setBackgroundColor (ALPHA_COLUMN_INDEX_, color);
        (*alpha_iterator)->setTextColor(ALPHA_COLUMN_INDEX_, ColorManager::isDark(color) ? Qt::white : Qt::black);
        (*alpha_iterator)->setText(ALPHA_COLUMN_INDEX_, QString("%1").arg(color.alpha()));
        ++alpha_iterator;
    }
}

//-------------------------------------------------------------------------
void EventTypesSelectionDialog::handleSelected (QTreeWidgetItem* item)
{
    if (item->checkState (CHECKBOX_COLUMN_INDEX_) == Qt::Checked)
        selected_types_.insert (item->text (ID_COLUMN_INDEX_).toUInt());
    else
        selected_types_.erase (item->text (ID_COLUMN_INDEX_).toUInt());
}

//-------------------------------------------------------------------------
void EventTypesSelectionDialog::handleColor (QTreeWidgetItem* item)
{
    if (item->text(ID_COLUMN_INDEX_).size() == 0)
        return;

    QColor color = item->backgroundColor (COLOR_COLUMN_INDEX_);

    color = QColorDialog::getColor (color, this, tr("Event Color"), QColorDialog::ShowAlphaChannel);
    if (!color.isValid())
        return;

    item->setBackgroundColor (COLOR_COLUMN_INDEX_, color);
    item->setTextColor (COLOR_COLUMN_INDEX_, ColorManager::isDark(color) ? Qt::white : Qt::black);
    item->setText (COLOR_COLUMN_INDEX_, QString("%1").arg(color.name()));

    color.setAlpha (item->text(ALPHA_COLUMN_INDEX_).toInt());
    item->setBackgroundColor (ALPHA_COLUMN_INDEX_, color);
    item->setTextColor (ALPHA_COLUMN_INDEX_, ColorManager::isDark(color) ? Qt::white : Qt::black);
    item->setText (ALPHA_COLUMN_INDEX_, QString::number (color.alpha()));
}

//-------------------------------------------------------------------------
void EventTypesSelectionDialog::handleAlpha (QTreeWidgetItem* item)
{
    if (item->text(ID_COLUMN_INDEX_).size() == 0)
        return;

    QColor color = item->backgroundColor (ALPHA_COLUMN_INDEX_);

    color.setAlpha (QInputDialog::getInt(this, tr("Alpha"), tr("Enter new Value"),
                                         color.alpha(), 0, 255, 25));


    item->setBackgroundColor (ALPHA_COLUMN_INDEX_, color);
    item->setTextColor (ALPHA_COLUMN_INDEX_, ColorManager::isDark(color) ? Qt::white : Qt::black);
    item->setText (ALPHA_COLUMN_INDEX_, QString::number (color.alpha()));
}

}

void sigviewer::EventTypesSelectionDialog::on_toggle_collapse_expand_button_clicked()
{
    if (ui_.toggle_collapse_expand_button->text().compare("Collapse All") == 0)
    {
        ui_.tree_widget_->collapseAll();
        ui_.toggle_collapse_expand_button->setText("Expand All");
    }
    else if (ui_.toggle_collapse_expand_button->text().compare("Expand All") == 0)
    {
        ui_.tree_widget_->expandAll();
        ui_.toggle_collapse_expand_button->setText("Collapse All");
    }

}
