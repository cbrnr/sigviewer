// event_type_dialog.h

#ifndef EVENT_TYPE_DIALOG_H
#define EVENT_TYPE_DIALOG_H

#include "base/user_types.h"

#include <QList>

#include <qdialog.h>

class QTreeWidget;
class QPushButton;
class QTreeWidgetItem;

namespace BioSig_
{

class EventColorManager;
class EventTableFileReader;

// basic header info dialog
class EventTypeDialog : public QDialog
{
    Q_OBJECT
public:
    typedef QList<uint16> IntList;

    EventTypeDialog(const QString& caption,
                    EventColorManager& event_color_manager,
                    EventTableFileReader& event_table_file_reader,
                    QWidget* parent = 0);

    void loadSettings();
    void saveSettings();

    void storeColors();

    void setShownTypes(IntList& shown_type_set);
    void getShownTypes(IntList& shown_type_set);

private slots:
    void itemClicked(QTreeWidgetItem* item, int column);
    void itemChanged(QTreeWidgetItem* item ,int column);

private:
    // not allowed
    EventTypeDialog();
    EventTypeDialog(const EventTypeDialog&);
    const EventTypeDialog& operator=(const EventTypeDialog&);

    void buildTree();

    QTreeWidget* event_tree_widget_;
    QPushButton* ok_button_;
    QPushButton* cancel_button_;
    EventColorManager& event_color_manager_;
    EventTableFileReader& event_table_file_reader_;
};

} // namespace BioSig_

#endif
