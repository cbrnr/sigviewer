// channel_selection_dialog.h

#ifndef CHANNEL_SELECTION_DIALOG_H
#define CHANNEL_SELECTION_DIALOG_H

#include "../../base/user_types.h"
#include "../../file_handling/channel_manager.h"

#include "ui_channel_dialog.h"

#include <QDialog>
#include <QSharedPointer>

class QListWidget;
class QPushButton;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;

namespace BioSig_
{

// channel selection dialog
class ChannelSelectionDialog : public QDialog
{
    Q_OBJECT
public:
    ChannelSelectionDialog (QSharedPointer<ChannelManager const> channel_manager,
                            QString const& file_name,
                            QWidget* parent = 0);

    bool isSelected(uint32 channel_nr);
    void setSelected(uint32 channel_nr, bool selected);

private slots:
   void on_unselect_all_button__clicked ();
   void on_select_all_button__clicked ();
   void on_list_widget__itemSelectionChanged();

private:
    //-------------------------------------------------------------------------
    /// disabled
    ChannelSelectionDialog();
    ChannelSelectionDialog(const ChannelSelectionDialog&);
    const ChannelSelectionDialog& operator=(const ChannelSelectionDialog&);

    QSharedPointer<ChannelManager const> channel_manager_;
    Ui::ChannelDialog ui_;
};

} // namespace BioSig_

#endif
