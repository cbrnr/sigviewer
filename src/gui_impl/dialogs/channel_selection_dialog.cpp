#include "channel_selection_dialog.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
ChannelSelectionDialog::ChannelSelectionDialog (QSharedPointer<ChannelManager const>
                                                channel_manager,
                                                QString const& file_name,
                                                QWidget* parent)
 : QDialog (parent),
   channel_manager_ (channel_manager)
{
    ui_.setupUi (this);
    QString window_title ("Channel Selection");
    if (file_name.size ())
        window_title.prepend(file_name + " - ");

    setWindowTitle (window_title);

    foreach (ChannelID id, channel_manager->getChannels())
        ui_.list_widget_->addItem(QString::number(id) + " - " + channel_manager_->getChannelLabel (id));

    adjustSize ();
}

//-----------------------------------------------------------------------------
bool ChannelSelectionDialog::isSelected(uint32 channel_nr)
{
    foreach (QListWidgetItem* item, ui_.list_widget_->selectedItems())
        if (item->text().section(" - ", 0, 0) == QString::number(channel_nr))
            return true;

    return false;
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::setSelected(uint32 channel_nr, bool selected)
{
    foreach (QListWidgetItem* item, ui_.list_widget_->findItems(QString::number(channel_nr) + " - ", Qt::MatchStartsWith))
        item->setSelected(selected);
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_unselect_all_button__clicked ()
{
    ui_.list_widget_->clearSelection ();
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_select_all_button__clicked ()
{
    ui_.list_widget_->selectAll ();
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::on_list_widget__itemSelectionChanged()
{
    unsigned num_selected_items = ui_.list_widget_->selectedItems().count();
    bool nothing_selected = (num_selected_items == 0);
    bool all_selected = (num_selected_items == ui_.list_widget_->count());

    ui_.button_box_->button(QDialogButtonBox::Ok)->setDisabled (nothing_selected);
    ui_.select_all_button_->setDisabled (all_selected);
    ui_.unselect_all_button_->setDisabled (nothing_selected);
}

} //namespace BioSig_
