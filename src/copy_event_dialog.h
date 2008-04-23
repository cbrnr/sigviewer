// copy_event_dialog.h



#ifndef COPY_EVENT_DIALOG_H

#define COPY_EVENT_DIALOG_H



#include "base/user_types.h"



#include <QDialog>

#include <QPointer>



class QListWidget;

class QPushButton;



namespace BioSig_

{



class BasicHeader;



// copy event dialog

class CopyEventDialog : public QDialog

{

    Q_OBJECT

public:

    CopyEventDialog(QPointer<BasicHeader> header, QWidget* parent = 0);



    void loadSettings();

    void saveSettings();



    void addSelectableChannel(uint32 channel_nr);

    bool isSelected(uint32 channel_nr);



private:

    // not allowed

    CopyEventDialog();

    CopyEventDialog(const CopyEventDialog&);

    const CopyEventDialog& operator=(const CopyEventDialog&);



    QPointer<BasicHeader> basic_header_;

    QListWidget* channel_list_widget_;

    QPushButton* ok_button_;

    QPushButton* cancel_button_;

};



} // namespace BioSig_



#endif

