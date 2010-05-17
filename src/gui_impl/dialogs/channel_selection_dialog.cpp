#include "channel_selection_dialog.h"

#include <QDebug>
#include <QColorDialog>

namespace BioSig_
{

//-----------------------------------------------------------------------------
ChannelSelectionDialog::ChannelSelectionDialog (QSharedPointer<ChannelManager const>
                                                channel_manager,
                                                QString const& file_name,
                                                QSharedPointer<ColorManager>
                                                color_manager,
                                                QWidget* parent)
 : QDialog (parent),
   channel_manager_ (channel_manager),
   color_manager_ (color_manager)
{
    ui_.setupUi (this);
    QString window_title (tr("Channel Selection"));
    if (file_name.size ())
        window_title.prepend(file_name + " - ");

    setWindowTitle (window_title);
    ui_.channel_table_->setRowCount (channel_manager_->getNumberChannels());

    int row = 0;
    foreach (ChannelID id, channel_manager_->getChannels())
    {
        QTableWidgetItem* label_item = new QTableWidgetItem (
                channel_manager_->getChannelLabel (id));

        QColor color = color_manager_->getChannelColor (id);
        QTableWidgetItem* color_item = new QTableWidgetItem (
                color.name());
        color_item->setBackground (color);
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

    adjustSize ();
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
void ChannelSelectionDialog::setSelected (ChannelID channel_id, bool selected)
{
    Qt::CheckState state = selected ? Qt::Checked : Qt::Unchecked;
    foreach (QTableWidgetItem* id_item, ui_.channel_table_->findItems (QString::number(channel_id), Qt::MatchExactly))
        if (id_item->column() == ID_INDEX_)
            ui_.channel_table_->item (id_item->row (), VISIBLE_INDEX_)->setCheckState (state);
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_unselect_all_button__clicked ()
{
    for (int row = 0; row < ui_.channel_table_->rowCount(); ++row)
        ui_.channel_table_->item (row, VISIBLE_INDEX_)->setCheckState (Qt::Unchecked);
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_select_all_button__clicked ()
{
    for (int row = 0; row < ui_.channel_table_->rowCount(); ++row)
        ui_.channel_table_->item (row, VISIBLE_INDEX_)->setCheckState(Qt::Checked);
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_reset_colors_button__clicked ()
{
    for (int row = 0; row < ui_.channel_table_->rowCount(); ++row)
        updateColor (row, ColorManager::DEFAULT_CHANNEL_COLOR_);
    color_manager_->saveSettings ();
}


//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_channel_table__cellClicked (int row, int column)
{
    if (column == COLOR_INDEX_)
    {
        QColorDialog color_dialog (ui_.channel_table_->item (row, column)->backgroundColor (), this);
        if (color_dialog.exec () == QDialog::Accepted)
            updateColor (row, color_dialog.selectedColor ());
        color_manager_->saveSettings ();
    }

/*    int num_selected_items = ui_.channel_table_->selectedItems().count();
    bool nothing_selected = (num_selected_items == 0);
    bool all_selected = (num_selected_items == ui_.channel_table_->rowCount());

    ui_.button_box_->button(QDialogButtonBox::Ok)->setDisabled (nothing_selected);
    ui_.select_all_button_->setDisabled (all_selected);
    ui_.unselect_all_button_->setDisabled (nothing_selected);*/
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_channel_table__cellChanged (int, int column)
{
    if (column == VISIBLE_INDEX_)
    {
        bool all_visible = true;
        bool all_hidden = true;
        for (int row = 0; row < ui_.channel_table_->rowCount(); ++row)
        {
            if (!ui_.channel_table_->item (row, VISIBLE_INDEX_))
                return;
            if (ui_.channel_table_->item (row, VISIBLE_INDEX_)->checkState() == Qt::Checked)
                all_hidden = false;
            else
                all_visible = false;
        }
        ui_.select_all_button_->setDisabled (all_visible);
        ui_.unselect_all_button_->setDisabled (all_hidden);
        ui_.button_box_->button (QDialogButtonBox::Ok)->setDisabled (all_hidden);
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

    color_manager_->setChannelColor (ui_.channel_table_->item (row, ID_INDEX_)->text().toUInt(), color);
}


} //namespace BioSig_
