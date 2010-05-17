// channel_selection_dialog.h

#ifndef CHANNEL_SELECTION_DIALOG_H
#define CHANNEL_SELECTION_DIALOG_H

#include "../../base/user_types.h"
#include "../../file_handling/channel_manager.h"
#include "../../gui/color_manager.h"

#include "ui_channel_dialog.h"

#include <QDialog>
#include <QSharedPointer>

namespace BioSig_
{

//-----------------------------------------------------------------------------
class ChannelSelectionDialog : public QDialog
{
    Q_OBJECT
public:
    ChannelSelectionDialog (QSharedPointer<ChannelManager const> channel_manager,
                            QString const& file_name,
                            QSharedPointer<ColorManager> color_manager,
                            QWidget* parent = 0);

    bool isSelected (ChannelID channel_id);
    void setSelected (ChannelID channel_id, bool selected);

private slots:
   void on_unselect_all_button__clicked ();
   void on_select_all_button__clicked ();
   void on_reset_colors_button__clicked ();
   void on_channel_table__cellClicked (int row, int column);
   void on_channel_table__cellChanged (int row, int column);

private:
    void updateColor (int row, QColor const& color);

   //-------------------------------------------------------------------------
    /// disabled
    ChannelSelectionDialog();
    ChannelSelectionDialog(const ChannelSelectionDialog&);
    const ChannelSelectionDialog& operator=(const ChannelSelectionDialog&);

    QSharedPointer<ChannelManager const> channel_manager_;
    QSharedPointer<ColorManager> color_manager_;
    Ui::ChannelDialog ui_;

    static int const VISIBLE_INDEX_ = 0;
    static int const LABEL_INDEX_ = 0;
    static int const COLOR_INDEX_ = 1;
    static int const ID_INDEX_ = 2;
};

} // namespace BioSig_

#endif
