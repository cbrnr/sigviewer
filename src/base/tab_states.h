// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef TAB_STATES_H
#define TAB_STATES_H

namespace sigviewer
{

enum TabSelectionState
{
    NO_TAB_SELECTION_STATE,
    TAB_STATE_NO_EVENT_SELECTED,
    TAB_STATE_EVENT_SELECTED_ONE_CHANNEL,
    TAB_STATE_EVENT_SELECTED_ALL_CHANNELS
};

enum TabEditState
{
    NO_TAB_EDIT_STATE,
    TAB_STATE_NO_REDO,
    TAB_STATE_NO_UNDO,
    TAB_STATE_NO_REDO_NO_UNDO,
    TAB_STATE_CAN_REDO_UNDO
};

}

#endif // TAB_STATES_H
