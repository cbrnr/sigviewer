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


#ifndef EVENT_TYPES_SELECTION_DIALOG_H
#define EVENT_TYPES_SELECTION_DIALOG_H

#include "base/sigviewer_user_types.h"
#include "file_handling/event_manager.h"
#include "gui/color_manager.h"
#include "ui_event_type_selection_dialog.h"

#include <QList>
#include <QDialog>

class QTreeWidget;
class QPushButton;
class QTreeWidgetItem;

namespace sigviewer
{

//-----------------------------------------------------------------------------
/// EventTypesSelectionDialog
class EventTypesSelectionDialog : public QDialog
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    EventTypesSelectionDialog (QString const& caption,
                               QSharedPointer<EventManager const> event_manager,
                               std::set<EventType> const& preselected_types,
                               QSharedPointer<ColorManager> color_manager,
                               bool show_colors = false,
                               QWidget* parent = 0);

    void storeColors();
    std::set<EventType> getSelectedTypes () const;

private slots:
    void on_tree_widget__itemClicked (QTreeWidgetItem* item, int column);
    void on_tree_widget__itemChanged (QTreeWidgetItem* item ,int column);
    void on_all_events_button__toggled (bool checked);
    void on_existing_events_button__toggled (bool checked);
    void on_select_all_button__clicked ();
    void on_unselect_all_button__clicked ();
    void on_show_colors_box__toggled (bool on);
    void on_reset_colors_button__clicked ();


private:
    //-------------------------------------------------------------------------
    // not allowed
    EventTypesSelectionDialog();
    EventTypesSelectionDialog(const EventTypesSelectionDialog&);
    const EventTypesSelectionDialog& operator=(const EventTypesSelectionDialog&);

    //-------------------------------------------------------------------------
    void buildTree (bool only_existing_events = false);

    //-------------------------------------------------------------------------
    void handleSelected (QTreeWidgetItem* item);

    //-------------------------------------------------------------------------
    void handleColor (QTreeWidgetItem* item);

    //-------------------------------------------------------------------------
    void handleAlpha (QTreeWidgetItem* item);


    static int const CHECKBOX_COLUMN_INDEX_ = 0;
    static int const NAME_COLUMN_INDEX_ = 0;
    static int const COLOR_COLUMN_INDEX_ = 1;
    static int const ALPHA_COLUMN_INDEX_ = 2;
    static int const ID_COLUMN_INDEX_ = 3;

    bool const show_colors_;
    QSharedPointer<EventManager const> event_manager_;
    std::set<EventType> selected_types_;
    QSharedPointer<ColorManager> color_manager_;
    Ui::EventTypeSelectionDialog ui_;
};

} // namespace SigViewer_

#endif // EVENT_TYPES_SELECTION_DIALOG_H

