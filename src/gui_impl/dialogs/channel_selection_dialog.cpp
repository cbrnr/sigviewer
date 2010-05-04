#include "channel_selection_dialog.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
ChannelSelectionDialog::ChannelSelectionDialog (QSharedPointer<ChannelManager>
                                                channel_manager,
                                                QString const& file_name,
                                                QWidget* parent)
 : QDialog (parent),
   channel_manager_ (channel_manager)
{
    ui_.setupUi (this);
    setWindowTitle(file_name + tr(" - Channel Selection"));

    for (uint32 channel_nr = 0;
         channel_nr < channel_manager_->getNumberChannels();
         channel_nr++)
    {
        ui_.list_widget_->addItem(QString("(%1) %2").arg(channel_nr + 1)
                                  .arg(channel_manager_->getChannelLabel
                                       (channel_nr)));
    }
    adjustSize ();
}

//-----------------------------------------------------------------------------
bool ChannelSelectionDialog::isSelected(uint32 channel_nr)
{
    return ui_.list_widget_->isItemSelected (ui_.list_widget_->item (channel_nr));
}

//-----------------------------------------------------------------------------
void ChannelSelectionDialog::setSelected(uint32 channel_nr, bool selected)
{
    ui_.list_widget_->setItemSelected (ui_.list_widget_->item(channel_nr),
                                       selected);
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
