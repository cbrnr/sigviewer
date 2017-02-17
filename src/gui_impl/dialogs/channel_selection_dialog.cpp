// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "channel_selection_dialog.h"
#include "file_handling_impl/xdf_reader.h"

#include <QDebug>
#include <QColorDialog>
#include <QInputDialog>
#include <QTreeWidget>

namespace sigviewer
{

QColor const ChannelSelectionDialog::NOT_VISIBLE_COLOR_ (Qt::gray);
QColor const ChannelSelectionDialog::VISIBLE_COLOR_ (Qt::black);


ChannelSelectionDialog::ChannelSelectionDialog(ChannelManager const& channel_manager,
                                               QSharedPointer<BasicHeader> header,
                                               QSharedPointer<ColorManager> color_manager,
                                               QWidget* parent)
 : QDialog (parent),
   channel_manager_ (channel_manager),
   color_manager_ (color_manager),
   header_ (header),
   self_setting_ (false)
{
    ui_.setupUi (this);
    QString window_title (tr("Channels"));
    ui_.treeWidget->hide();

    setWindowTitle (window_title);
    ui_.channel_table_->setRowCount (channel_manager_.getNumberChannels());
    ui_.channel_table_->setColumnWidth(0, ui_.channel_table_->width() * 0.67);

    int row = 0;
    foreach (ChannelID id, channel_manager_.getChannels())
    {
        QTableWidgetItem* label_item = new QTableWidgetItem (
                channel_manager_.getChannelLabel (id));

        QColor color = color_manager_->getChannelColor (id);
        QTableWidgetItem* color_item = new QTableWidgetItem (
                color.name());
        color_item->setBackground (color);
        color_item->setFlags (Qt::ItemIsEnabled);
        if (ColorManager::isDark (color))
            color_item->setForeground (Qt::white);

        QTableWidgetItem* id_item = new QTableWidgetItem (
                QString::number (id));

        label_item->setFlags (label_item->flags() | Qt::ItemIsUserCheckable);
        label_item->setCheckState (Qt::Unchecked);

        ui_.channel_table_->setItem (row, LABEL_INDEX_, label_item);
        ui_.channel_table_->setItem (row, COLOR_INDEX_, color_item);
        ui_.channel_table_->setItem (row, ID_INDEX_, id_item);
        row++;
    }
    ui_.channel_table_->hideColumn (ID_INDEX_);
}

ChannelSelectionDialog::ChannelSelectionDialog(QString file_format, const ChannelManager &channel_manager, QSharedPointer<BasicHeader> header,
                                               QSharedPointer<ColorManager> color_manager, QWidget *parent)
    : QDialog (parent), channel_manager_ (channel_manager), color_manager_ (color_manager),
      header_ (header), self_setting_ (false), fileType (file_format)
{
    ui_.setupUi (this);
    this->setWindowTitle (tr("Channels"));

    ui_.channel_table_->hide();
    QStringList headerLabels;
    headerLabels << tr("Stream") << tr("Color");
    ui_.treeWidget->setHeaderLabels(headerLabels);
    ui_.treeWidget->setColumnCount(2);
    ui_.treeWidget->setAnimated(true);

    int channelCount = 0;
    for (size_t i = 0; i < XDFdata.streams.size(); i++)
    {
        QTreeWidgetItem* streamItem = new QTreeWidgetItem(ui_.treeWidget);
        streamItem->setText(0, tr("Stream ").append(QString::number(i)));
        streamItem->setFlags(Qt::ItemIsAutoTristate | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        streamItem->setExpanded(true);
        if (XDFdata.streams[i].info.channel_format.compare("string") == 0)
        {
            streamItem->setForeground(0, NOT_VISIBLE_COLOR_);
        }
        else
        {
            QColor streamColor = color_manager_->getChannelColor (channelCount);
            streamItem->setText(1, streamColor.name());
            streamItem->setBackgroundColor(1, streamColor);
            if (ColorManager::isDark(streamColor))
                streamItem->setForeground(1, Qt::white);

            for (int j = 0; j < XDFdata.streams[i].info.channel_count; j++)
            {
                QTreeWidgetItem* channelItem = new QTreeWidgetItem(streamItem);
                channelItem->setText(0, tr("Channel ").append(QString::number(channelCount)));
                channelItem->setCheckState(0, Qt::Unchecked);
                channelItem->setFlags(Qt::ItemIsAutoTristate | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
                QColor color = color_manager_->getChannelColor (channelCount);
                channelItem->setText(1, color.name());
                channelItem->setBackgroundColor(1, color);
                if (ColorManager::isDark(color))
                    channelItem->setForeground(1, Qt::white);
                channelCount++;
            }
        }
    }
    ui_.select_all_button_->setDisabled (true);
    ui_.unselect_all_button_->setDisabled (false);
}

void ChannelSelectionDialog::resizeEvent(QResizeEvent *event) {
    ui_.channel_table_->setColumnWidth(0, this->width() * 0.6);
    ui_.treeWidget->setColumnWidth(0, this->width() * 0.6);
    QDialog::resizeEvent(event);
}


//-----------------------------------------------------------------------------
bool ChannelSelectionDialog::isSelected (ChannelID channel_id)
{
    foreach (QTableWidgetItem* id_item, ui_.channel_table_->findItems (QString::number(channel_id), Qt::MatchExactly))
        if (id_item->column() == ID_INDEX_)
            return ui_.channel_table_->item (id_item->row (), VISIBLE_INDEX_)->checkState() == Qt::Checked;    
    return false;
}

//-----------------------------------------------------------------------------
bool ChannelSelectionDialog::isSelected (QString file_format, ChannelID channel_id)
{
    QTreeWidgetItemIterator it(ui_.treeWidget);
    while (*it) {
        if ((*it)->text(0).compare("Channel "+QString::number(channel_id), Qt::CaseInsensitive) == 0)
        {
            return (*it)->checkState(0) == Qt::Checked;
            break;
        }
        it++;
    }
    return false;
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::setSelected (ChannelID channel_id, bool selected)
{
    Qt::CheckState state = selected ? Qt::Checked : Qt::Unchecked;
    for (int row = 0; row < ui_.channel_table_->rowCount(); row++)
        if (QString::number(channel_id) == ui_.channel_table_->item(row, ID_INDEX_)->text())
            ui_.channel_table_->item(row, VISIBLE_INDEX_)->setCheckState(state);
}

void ChannelSelectionDialog::setSelected(QString file_format, ChannelID channel_id, bool selected)
{
    Qt::CheckState state = selected ? Qt::Checked : Qt::Unchecked;
    QTreeWidgetItemIterator it(ui_.treeWidget);
    QString temp = "Channel " + QString::number(channel_id);
    while (*it) {
        if ((*it)->text(0) == temp)
        {
            (*it)->setCheckState(0, state);
            break;
        }
        it++;
    }
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_unselect_all_button__clicked ()
{
    self_setting_ = true;
    if (fileType.compare("XDF", Qt::CaseInsensitive) == 0)
    {
        QTreeWidgetItemIterator it(ui_.treeWidget);
        while (*it) {
            if ((*it)->foreground(0) != NOT_VISIBLE_COLOR_)
            {
                (*it)->setCheckState(0, Qt::Unchecked);
            }
            ++it;
        }
    }
    else
    {
        for (int row = 0; row < ui_.channel_table_->rowCount(); ++row)
            ui_.channel_table_->item (row, VISIBLE_INDEX_)->setCheckState (Qt::Unchecked);
    }
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_select_all_button__clicked ()
{
    if (fileType.compare("XDF", Qt::CaseInsensitive)==0)
    {
        QTreeWidgetItemIterator it(ui_.treeWidget);
        while (*it) {
            if ((*it)->foreground(0) != NOT_VISIBLE_COLOR_)
            {
                (*it)->setCheckState(0, Qt::Checked);
            }
            ++it;
        }
    }
    else
    {
        for (int row = 0; row < ui_.channel_table_->rowCount(); ++row)
            ui_.channel_table_->item (row, VISIBLE_INDEX_)->setCheckState(Qt::Checked);
    }
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_reset_colors_button__clicked ()
{
    if (fileType.compare("XDF", Qt::CaseInsensitive)==0)
    {
        QTreeWidgetItemIterator it(ui_.treeWidget);
        while (*it) {
            if ((*it)->foreground(0) != NOT_VISIBLE_COLOR_)
            {
                updateColor("XDF", *it, color_manager_->getDefaultChannelColor());
            }
            it++;
        }
    }
    else
    {
        for (int row = 0; row < ui_.channel_table_->rowCount(); ++row)
            updateColor (row, color_manager_->getDefaultChannelColor());
    }
}


//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_channel_table__cellClicked (int row, int column)
{
    QTableWidgetItem* item = ui_.channel_table_->item (row, column);
    if (column == LABEL_INDEX_)
    {
        self_setting_ = true;
        if (item->checkState() == Qt::Checked)
            item->setCheckState (Qt::Unchecked);
        else
            item->setCheckState (Qt::Checked);
    }
    else if (column == COLOR_INDEX_)
    {
        QColorDialog color_dialog (item->backgroundColor (), this);
        if (color_dialog.exec () == QDialog::Accepted)
            updateColor (row, color_dialog.selectedColor ());
    }
    self_setting_ = false;
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_channel_table__cellChanged (int row, int column)
{
    if (column == VISIBLE_INDEX_)
    {
        if (!self_setting_)
        {
            self_setting_ = true;
            QTableWidgetItem* item = ui_.channel_table_->item (row, column);
            if (item->checkState() == Qt::Checked)
                item->setCheckState (Qt::Unchecked);
            else
                item->setCheckState (Qt::Checked);
        }
        bool all_visible = true;
        bool all_hidden = true;
        for (int row_index = 0; row_index < ui_.channel_table_->rowCount(); ++row_index)
        {
            QTableWidgetItem* item = ui_.channel_table_->item (row_index, VISIBLE_INDEX_);
            if (!item)
                return;
            if (item->checkState() == Qt::Checked)
            {
                all_hidden = false;
                item->setForeground (VISIBLE_COLOR_);
            }
            else
            {
                all_visible = false;
                item->setForeground (NOT_VISIBLE_COLOR_);
            }
        }
        ui_.select_all_button_->setDisabled (all_visible);
        ui_.unselect_all_button_->setDisabled (all_hidden);
        ui_.button_box_->button (QDialogButtonBox::Ok)->setDisabled (all_hidden);
    }
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_button_box__accepted ()
{
    if (fileType.compare("XDF", Qt::CaseInsensitive) == 0)
    {
        QTreeWidgetItemIterator it(ui_.treeWidget);
        while (*it)
        {
            if ((*it)->text(0).startsWith("Channel"))
            {
                unsigned channelNumber = (*it)->text(0).remove("Channel ", Qt::CaseInsensitive).toUInt();
                color_manager_->setChannelColor(channelNumber, (*it)->backgroundColor(1));
            }
            it++;
        }
    }
    else
    {
        for (int row = 0; row < ui_.channel_table_->rowCount(); ++row)
            color_manager_->setChannelColor (ui_.channel_table_->item (row, ID_INDEX_)->text().toUInt(),
                                             ui_.channel_table_->item (row, COLOR_INDEX_)->backgroundColor());
    }
    color_manager_->saveSettings ();
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_set_default_color_button__clicked ()
{
    QColor new_default_color = QColorDialog::getColor (color_manager_->getDefaultChannelColor());
    if (new_default_color.isValid())
        color_manager_->setDefaultChannelColor (new_default_color);
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    if (column == COLOR_INDEX_)
    {
        if (item->text(0).startsWith("Channel", Qt::CaseInsensitive))
        {
            QColorDialog color_dialog (item->backgroundColor (1), this);
            if (color_dialog.exec () == QDialog::Accepted)
                updateColor("XDF", item, color_dialog.selectedColor());
        }
        else if (item->text(0).startsWith("Stream", Qt::CaseInsensitive) && item->childCount())
        {
            QColorDialog color_dialog (item->backgroundColor (1), this);
            if (color_dialog.exec () == QDialog::Accepted)
            {
                updateColor("XDF", item, color_dialog.selectedColor());

                for (int i = 0; i < item->childCount(); i++)
                {
                    if (item->child(i)->checkState(0) == Qt::Checked)
                        updateColor("XDF", item->child(i), color_dialog.selectedColor());
                }
            }
        }
    }
    self_setting_ = false;
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    if (column == VISIBLE_INDEX_)
    {
        self_setting_ = true;

        bool select_all_disabled = true;
        bool unselect_all_disabled = true;
        QTreeWidgetItemIterator it(ui_.treeWidget);
        while (*it) {
            if ((*it)->foreground(0) != NOT_VISIBLE_COLOR_)
            {
                if ((*it)->checkState(0) == Qt::Unchecked)
                {
                    select_all_disabled = false;
                }
                else
                {
                    unselect_all_disabled = false;
                }
            }
            it++;
        }
        ui_.select_all_button_->setDisabled (select_all_disabled);
        ui_.unselect_all_button_->setDisabled (unselect_all_disabled);
//        ui_.button_box_->button (QDialogButtonBox::Ok)->setDisabled (all_hidden);
    }
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::updateColor (int row, QColor const& color)
{
    ui_.channel_table_->item (row, COLOR_INDEX_)->setBackgroundColor (color);
    ui_.channel_table_->item (row, COLOR_INDEX_)->setText (color.name ());
    if (ColorManager::isDark (color))
        ui_.channel_table_->item (row, COLOR_INDEX_)->setForeground (Qt::white);
    else
        ui_.channel_table_->item (row, COLOR_INDEX_)->setForeground (Qt::black);
}

void ChannelSelectionDialog::updateColor(QString file_format, QTreeWidgetItem *item, const QColor &color)
{
    item->setBackgroundColor(1, color);
    item->setText(1, color.name());
    if (ColorManager::isDark(color))
        item->setForeground(1, Qt::white);
    else
        item->setForeground(1, Qt::black);
}

}
