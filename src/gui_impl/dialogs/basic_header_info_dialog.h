// basic_header_info_dialog.h

#ifndef BASIC_HEADER_INFO_DIALOG_H
#define BASIC_HEADER_INFO_DIALOG_H

#include "file_handling/basic_header.h"

#include <QDialog>
#include <QSharedPointer>

class QTreeWidget;
class QPushButton;

namespace SigViewer_
{

// basic header info dialog
class BasicHeaderInfoDialog : public QDialog
{
    Q_OBJECT
public:
    BasicHeaderInfoDialog(QSharedPointer<BasicHeader> header, QWidget* parent = 0);

    void loadSettings();
    void saveSettings();

private:
    // not allowed
    BasicHeaderInfoDialog();
    BasicHeaderInfoDialog(const BasicHeaderInfoDialog&);
    const BasicHeaderInfoDialog& operator=(const BasicHeaderInfoDialog&);

    void buildTree();

    QSharedPointer<BasicHeader> basic_header_;
    QTreeWidget* info_tree_widget_;
    QPushButton* close_button_;
};

} // namespace SigViewer_

#endif
