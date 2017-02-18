// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


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

    QSharedPointer<EventManager const> event_manager_;
    std::set<EventType> selected_types_;
    QSharedPointer<ColorManager> color_manager_;
    Ui::EventTypeSelectionDialog ui_;
};

}

#endif

