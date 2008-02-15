// basic_header_info_dialog.h

#ifndef BASIC_HEADER_INFO_DIALOG_H
#define BASIC_HEADER_INFO_DIALOG_H

#include <QDialog>
#include <QPointer>

class QTreeWidget;
class QPushButton;

namespace BioSig_
{

class BasicHeader;

// basic header info dialog
class BasicHeaderInfoDialog : public QDialog
{
    Q_OBJECT
public:
    BasicHeaderInfoDialog(QPointer<BasicHeader> header, QWidget* parent = 0);

    void loadSettings();
    void saveSettings();

private:
    // not allowed
    BasicHeaderInfoDialog();
    BasicHeaderInfoDialog(const BasicHeaderInfoDialog&);
    const BasicHeaderInfoDialog& operator=(const BasicHeaderInfoDialog&);

    void buildTree();

    QPointer<BasicHeader> basic_header_;
    QTreeWidget* info_tree_widget_;
    QPushButton* close_button_;
};

} // namespace BioSig_

#endif
