#ifndef TAB_STATES_H
#define TAB_STATES_H

namespace BioSig_
{

enum TabSelectionState
{
    TAB_STATE_NO_EVENT_SELECTED,
    TAB_STATE_EVENT_SELECTED_ONE_CHANNEL,
    TAB_STATE_EVENT_SELECTED_ALL_CHANNELS
};

enum TabEditState
{
    TAB_STATE_NO_REDO,
    TAB_STATE_NO_UNDO,
    TAB_STATE_NO_REDO_NO_UNDO,
    TAB_STATE_CAN_REDO_UNDO
};

} // namespace BioSig_

#endif // TAB_STATES_H
