// channel_selection_dialog.h

#ifndef CHANNEL_SELECTION_DIALOG_H
#define CHANNEL_SELECTION_DIALOG_H

#include "base/user_types.h"

#include <qdialog.h>

class QListWidget;
class QPushButton;

namespace BioSig_
{

class BasicHeader;

// channel selection dialog
class ChannelSelectionDialog : public QDialog
{
    Q_OBJECT
public:
    ChannelSelectionDialog(BasicHeader& header, QWidget* parent = 0);

    void loadSettings();
    void saveSettings();

    bool isSelected(uint32 channel_nr);
    void setSelected(uint32 channel_nr, bool selected);

private:
    // not allowed
    ChannelSelectionDialog();
    ChannelSelectionDialog(const ChannelSelectionDialog&);
    const ChannelSelectionDialog& operator=(const ChannelSelectionDialog&);

    void buildChannelList();

    
    BasicHeader& basic_header_;
    QListWidget* channel_list_widget_;
    QPushButton* unselect_all_button_;
    QPushButton* select_all_button_;
    QPushButton* ok_button_;
    QPushButton* cancel_button_;

 private slots:
    void unselectAll ();
    void selectAll ();
};

} // namespace BioSig_

#endif
