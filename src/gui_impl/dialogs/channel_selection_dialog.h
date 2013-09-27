// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


#ifndef CHANNEL_SELECTION_DIALOG_H
#define CHANNEL_SELECTION_DIALOG_H

#include "base/sigviewer_user_types.h"
#include "file_handling/channel_manager.h"
#include "file_handling/basic_header.h"
#include "gui/color_manager.h"

#include "ui_channel_dialog.h"

#include <QDialog>
#include <QSharedPointer>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
class ChannelSelectionDialog : public QDialog
{
    Q_OBJECT
friend class FileOpenTest;
public:
    ChannelSelectionDialog (ChannelManager const& channel_manager,
                            QSharedPointer<BasicHeader> header,
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
   void on_button_box__accepted ();
   void on_show_colors_box__toggled (bool on);
   void on_set_default_color_button__clicked ();
   void on_downsample_factor_spinbox__valueChanged (int value);
   void on_add_filter_button__clicked ();
   void on_remove_filter_button__clicked ();

private:
    void updateColor (int row, QColor const& color);

   //-------------------------------------------------------------------------
    /// disabled
    ChannelSelectionDialog();
    ChannelSelectionDialog(const ChannelSelectionDialog&);
    const ChannelSelectionDialog& operator=(const ChannelSelectionDialog&);

    ChannelManager const& channel_manager_;
    QSharedPointer<ColorManager> color_manager_;
    QSharedPointer<BasicHeader> header_;
public:
    Ui::ChannelDialog ui_;
private:
    static int const VISIBLE_INDEX_ = 0;
    static int const LABEL_INDEX_ = 0;
    static int const COLOR_INDEX_ = 1;
    static int const ID_INDEX_ = 2;
    static QColor const NOT_VISIBLE_COLOR_;
    static QColor const VISIBLE_COLOR_;
    bool self_setting_;
};

} // namespace SigViewer_

#endif
