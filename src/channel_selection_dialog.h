// channel_selection_dialog.h

#ifndef CHANNEL_SELECTION_DIALOG_H
#define CHANNEL_SELECTION_DIALOG_H

#include "base/user_types.h"

#include <qdialog.h>
#include <QPointer>

class QListWidget;
class QPushButton;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;

namespace BioSig_
{

class BasicHeader;

// channel selection dialog
class ChannelSelectionDialog : public QDialog
{
    Q_OBJECT
public:
    ChannelSelectionDialog(QPointer<BasicHeader> header, QWidget* parent = 0);

    void loadSettings();
    void saveSettings();

    bool isSelected(uint32 channel_nr);
    void setSelected(uint32 channel_nr, bool selected);
//    bool isInitRangeSearch() const;
//    uint32 wholeSubsampling() const;
//    float32 rangeMin() const;
//    float32 rangeMax() const;

private:
    // not allowed
    ChannelSelectionDialog();
    ChannelSelectionDialog(const ChannelSelectionDialog&);
    const ChannelSelectionDialog& operator=(const ChannelSelectionDialog&);

    void buildChannelList();

    QPointer<BasicHeader> basic_header_;
    QListWidget* channel_list_widget_;
    QPushButton* unselect_all_button_;
    QPushButton* select_all_button_;
    QPushButton* ok_button_;
    QPushButton* cancel_button_;
    QCheckBox* init_range_search_checkbox_;
    QComboBox* whole_buffer_combobox_;
    QDoubleSpinBox* min_spinbox_;
    QDoubleSpinBox* max_spinbox_;

 private slots:
    void unselectAll();
    void selectAll();
//    void maxChanged(double value);
//    void minChanged(double value);
//    void rangeSearchChanged(bool checked);
};

} // namespace BioSig_

#endif
