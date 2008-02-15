// go_to_dialog.h

#ifndef GO_TO_DIALOG_H
#define GO_TO_DIALOG_H

#include "base/user_types.h"

#include <QDialog>
#include <QPointer>

class QPushButton;
class QSpinBox;
class QComboBox;

namespace BioSig_
{

class BasicHeader;

// go to dialog
class GoToDialog : public QDialog
{
    Q_OBJECT
public:
    enum
    {
        NO_CHANNEL = -1
    };

    GoToDialog(QPointer<BasicHeader> header, QWidget* parent = 0);

    void loadSettings();
    void saveSettings();

    int32 getSecond();
    int32 getChannelIndex();
    void setChannelShown(uint32 channel_nr);

private:
    // not allowed
    GoToDialog();
    GoToDialog(const GoToDialog&);
    const GoToDialog& operator=(const GoToDialog&);

    QPointer<BasicHeader> basic_header_;
    QPushButton* ok_button_;
    QPushButton* cancel_button_;
    QSpinBox* sec_spinbox_;
    QComboBox* channel_combobox_;
};

} // namespace BioSig_

#endif
