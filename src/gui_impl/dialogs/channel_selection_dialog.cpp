#include "channel_selection_dialog.h"
//#include "signal_processing/notch_filter8.h"


#include <QDebug>
#include <QColorDialog>
#include <QInputDialog>

namespace SigViewer_
{

QColor const ChannelSelectionDialog::NOT_VISIBLE_COLOR_ (Qt::gray);
QColor const ChannelSelectionDialog::VISIBLE_COLOR_ (Qt::black);


//-----------------------------------------------------------------------------
ChannelSelectionDialog::ChannelSelectionDialog (ChannelManager const&
                                                channel_manager,
                                                QSharedPointer<BasicHeader> header,
                                                QSharedPointer<ColorManager>
                                                color_manager,
                                                QWidget* parent)
 : QDialog (parent),
   channel_manager_ (channel_manager),
   color_manager_ (color_manager),
   header_ (header),
   self_setting_ (false)
{
    ui_.setupUi (this);
    QString window_title (tr("Channels"));

//    if (header.isNull())
//    {
        while (ui_.tabs->count() > 1)
            ui_.tabs->removeTab (1);
//    }
//    else
//    {
//        window_title.prepend (header->getFilePath() + " - ");
//        ui_.tabs->removeTab (1);
//        ui_.sr_file_label_->setText (QString::number (header_->getDownSamplingFactor() * header_->getSampleRate()).append(" Hz"));
//        ui_.sr_load_label_->setText (QString::number (header_->getSampleRate()).append(" Hz"));
//        ui_.downsample_factor_spinbox_->setValue (header_->getDownSamplingFactor());
//    }

    setWindowTitle (window_title);
    ui_.channel_table_->setRowCount (channel_manager_.getNumberChannels());

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
    on_show_colors_box__toggled (false);
    ui_.tabs->setCurrentIndex (0);
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
    for (int row = 0; row < ui_.channel_table_->rowCount(); row++)
        if (QString::number(channel_id) == ui_.channel_table_->item(row, ID_INDEX_)->text())
            ui_.channel_table_->item(row, VISIBLE_INDEX_)->setCheckState(state);
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_unselect_all_button__clicked ()
{
    self_setting_ = true;
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
        updateColor (row, color_manager_->getDefaultChannelColor());
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
    for (int row = 0; row < ui_.channel_table_->rowCount(); ++row)
    {
        color_manager_->setChannelColor (ui_.channel_table_->item (row, ID_INDEX_)->text().toUInt(),
                                         ui_.channel_table_->item (row, COLOR_INDEX_)->backgroundColor());
    }
    color_manager_->saveSettings ();
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_show_colors_box__toggled (bool on)
{
    ui_.channel_table_->setColumnHidden (COLOR_INDEX_, !on);
    ui_.reset_colors_button_->setVisible (on);
    ui_.set_default_color_button_->setVisible (on);
    if (on)
        ui_.channel_table_->horizontalHeader()->setDefaultSectionSize ((ui_.channel_table_->width() - 10) / 2);
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_set_default_color_button__clicked ()
{
    QColor new_default_color = QColorDialog::getColor (color_manager_->getDefaultChannelColor());
    if (new_default_color.isValid())
        color_manager_->setDefaultChannelColor (new_default_color);
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_downsample_factor_spinbox__valueChanged (int /*value*/)
{
//    header_->setDownSamplingFactor (value);
//    ui_.sr_load_label_->setText (QString::number (header_->getSampleRate() ).append(" Hz"));
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_add_filter_button__clicked ()
{
//    bool ok = false;

//    double frequency = QInputDialog::getDouble (0, "Notch", "Frequency", 0, 0, header_->getSampleRate(), 1, &ok);
//    if (!ok)
//        return;

//    QSharedPointer<Filter> notch_filter (new NotchFilter8 (header_->getSampleRate() * header_->getDownSamplingFactor(), frequency));
//    int header_filter_index = header_->addFilter (notch_filter);
//    ui_.chosen_filter_list_->addItem (QString ("Notch ").append(QString::number(frequency)).append("Hz"));
//    ui_.chosen_filter_list_->item (ui_.chosen_filter_list_->count() - 1)->setData (Qt::UserRole, header_filter_index);
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_remove_filter_button__clicked ()
{
//    foreach (QListWidgetItem* item, ui_.chosen_filter_list_->selectedItems())
//    {
//        header_->removeFilter (item->data(Qt::UserRole).toInt());
//        ui_.chosen_filter_list_->takeItem (ui_.chosen_filter_list_->row (item));
//        delete item;
//    }
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


} //namespace SigViewer_
