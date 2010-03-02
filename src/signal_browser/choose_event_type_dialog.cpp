#include "choose_event_type_dialog.h"

namespace BioSig_
{

ChooseEventTypeDialog::ChooseEventTypeDialog(std::map<uint16, QString> const& shown_event_types)
    : shown_event_types_(shown_event_types)
{
}

}
