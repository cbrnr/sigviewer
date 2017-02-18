// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "channel_selection_dialog.h"
#include "file_handling_impl/xdf_reader.h"
#include "application_context_impl.h"

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
   header_ (header)
{
    ui_.setupUi (this);

    setWindowTitle (tr("Channels"));

    QStringList headerLabels;
    headerLabels << tr("Channel") << tr("Color");
    ui_.treeWidget->setHeaderLabels(headerLabels);
    ui_.treeWidget->setColumnCount(2);
    ui_.treeWidget->header()->resizeSection(0, width() * 0.6);
    ui_.treeWidget->setAnimated(true);

    if (ApplicationContextImpl::getInstance()->getCurrentFileContext()->getFileName().endsWith("XDF", Qt::CaseInsensitive))
    {
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
                streamItem->setText(0, streamItem->text(0).append(tr(" (text events only)")));
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
    }
    else
    {
        int row = 0;
        foreach (ChannelID id, channel_manager_.getChannels())
        {
            QTreeWidgetItem* channelItem = new QTreeWidgetItem (ui_.treeWidget);
            channelItem->setText(0, tr("Channel ").append(QString::number(id)));
            channelItem->setFlags((Qt::ItemIsEnabled | Qt::ItemIsUserCheckable));

            QColor color = color_manager_->getChannelColor (id);
            channelItem->setText(1, color.name());
            channelItem->setBackgroundColor(1, color);
            if (ColorManager::isDark(color))
                channelItem->setForeground(1, Qt::white);
            row++;
        }
    }

    ui_.select_all_button_->setDisabled (true);
    ui_.unselect_all_button_->setDisabled (false);
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::resizeEvent(QResizeEvent *event) {
    ui_.treeWidget->setColumnWidth(0, this->width() * 0.6);
    QDialog::resizeEvent(event);
}

//-----------------------------------------------------------------------------
bool ChannelSelectionDialog::isSelected (ChannelID channel_id)
{
    QTreeWidgetItemIterator it(ui_.treeWidget);
    while (*it)
    {
        if ((*it)->text(0).compare("Channel "+ QString::number(channel_id), Qt::CaseInsensitive) == 0)
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
    QTreeWidgetItemIterator it(ui_.treeWidget);
    QString temp = "Channel " + QString::number(channel_id);
    while (*it)
    {
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
    QTreeWidgetItemIterator it(ui_.treeWidget);
    while (*it)
    {
        if ((*it)->foreground(0) != NOT_VISIBLE_COLOR_)/*!< These are streams with text only */
        {
            (*it)->setCheckState(0, Qt::Unchecked);
        }
        ++it;
    }
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_select_all_button__clicked ()
{
    QTreeWidgetItemIterator it(ui_.treeWidget);
    while (*it)
    {
        if ((*it)->foreground(0) != NOT_VISIBLE_COLOR_)/*!< These are streams with text only */
        {
            (*it)->setCheckState(0, Qt::Checked);
        }
        ++it;
    }
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_reset_colors_button__clicked ()
{
    QTreeWidgetItemIterator it(ui_.treeWidget);
    while (*it)
    {
        if ((*it)->foreground(0) != NOT_VISIBLE_COLOR_)
        {
            updateColor(*it, color_manager_->getDefaultChannelColor());
        }
        it++;
    }
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_button_box__accepted ()
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
                updateColor(item, color_dialog.selectedColor());
        }
        else if (item->text(0).startsWith("Stream", Qt::CaseInsensitive) && item->childCount())
        {
            QColorDialog color_dialog (item->backgroundColor (1), this);
            if (color_dialog.exec () == QDialog::Accepted)
            {
                updateColor(item, color_dialog.selectedColor());

                for (int i = 0; i < item->childCount(); i++)
                {
                    if (item->child(i)->checkState(0) == Qt::Checked)
                        updateColor(item->child(i), color_dialog.selectedColor());
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    if (column == VISIBLE_INDEX_)
    {
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
    }
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::updateColor (QTreeWidgetItem *item, QColor const& color)
{
    item->setBackgroundColor(1, color);
    item->setText(1, color.name());
    if (ColorManager::isDark(color))
        item->setForeground(1, Qt::white);
    else
        item->setForeground(1, Qt::black);
}

}
