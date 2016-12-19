// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


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

} // namespace SigViewer_

#endif // TAB_STATES_H
