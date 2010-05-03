#ifndef EVENT_TYPES_SELECTION_DIALOG_H
#define EVENT_TYPES_SELECTION_DIALOG_H

#include "../../base/user_types.h"
#include "../../file_handling/event_manager.h"


#include <QList>
#include <QDialog>

class QTreeWidget;
class QPushButton;
class QTreeWidgetItem;

namespace BioSig_
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
                               QWidget* parent = 0);

    void loadSettings();
    void saveSettings();
    void storeColors();
    std::set<EventType> getSelectedTypes () const;

private slots:
    void itemClicked(QTreeWidgetItem* item, int column);
    void itemChanged(QTreeWidgetItem* item ,int column);

private:
    //-------------------------------------------------------------------------
    // not allowed
    EventTypesSelectionDialog();
    EventTypesSelectionDialog(const EventTypesSelectionDialog&);
    const EventTypesSelectionDialog& operator=(const EventTypesSelectionDialog&);

    //-------------------------------------------------------------------------
    void buildTree (std::set<EventType> const& preselected_types);

    static int const CHECKBOX_COLUMN_INDEX_ = 0;
    static int const NAME_COLUMN_INDEX_ = 1;
    static int const COLOR_COLUMN_INDEX_ = 2;
    static int const ALPHA_COLUMN_INDEX_ = 3;
    static int const ID_COLUMN_INDEX_ = 4;

    QSharedPointer<EventManager const> event_manager_;
    QTreeWidget* event_tree_widget_;
    QPushButton* ok_button_;
    QPushButton* cancel_button_;
};

} // namespace BioSig_

#endif // EVENT_TYPES_SELECTION_DIALOG_H

