#ifndef CHOOSE_EVENT_TYPE_DIALOG_H
#define CHOOSE_EVENT_TYPE_DIALOG_H

#include "../base/user_types.h"
#include <QDialog>
#include <map>

namespace BioSig_
{

class ChooseEventTypeDialog : public QDialog
{
public:
    //-------------------------------------------------------------------------
    ChooseEventTypeDialog (std::map<uint16, QString> const& shown_event_types);

    //-------------------------------------------------------------------------
    uint16 getSelectedEventType () const;

private:
    std::map<uint16, QString> shown_event_types_;
};

}

#endif // CHOOSE_EVENT_TYPE_DIALOG_H
