// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef CHANNEL_SELECTION_DIALOG_H
#define CHANNEL_SELECTION_DIALOG_H

#include "base/sigviewer_user_types.h"
#include "file_handling/channel_manager.h"
#include "file_handling/basic_header.h"
#include "gui/color_manager.h"

#include "ui_channel_dialog.h"

#include <QDialog>
#include <QSharedPointer>

namespace sigviewer
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
   void on_button_box__accepted ();
   void on_set_default_color_button__clicked ();
   void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
   void on_treeWidget_itemChanged(QTreeWidgetItem *item, int column);

private:
    void updateColor (QTreeWidgetItem *item, QColor const& color);
    void resizeEvent(QResizeEvent *event);

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
};

}

#endif
