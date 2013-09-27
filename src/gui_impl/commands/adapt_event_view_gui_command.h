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


#ifndef FIT_VIEW_GUI_COMMAND_H
#define FIT_VIEW_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"

#include <QStringList>

namespace SigViewer_
{

//-------------------------------------------------------------------------
class AdaptEventViewGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    AdaptEventViewGuiCommand ();

    //-------------------------------------------------------------------------
    virtual ~AdaptEventViewGuiCommand () {}

    //-------------------------------------------------------------------------
    virtual void init ();

public slots:
    //-------------------------------------------------------------------------
    virtual void trigger (QString const& action_name);

    //-------------------------------------------------------------------------
    void hideEventsOfOtherType ();

    //-------------------------------------------------------------------------
    void showAllEvents ();

    //-------------------------------------------------------------------------
    void setShownEvents ();

protected:
    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness ();

private:
    //-------------------------------------------------------------------------
    void fitViewToEvent ();

    //-------------------------------------------------------------------------
    void gotoAndSelectEvent (bool forward);

    //-------------------------------------------------------------------------
    void setNextAndPreviousEvent ();

    //-------------------------------------------------------------------------
    EventID current_event_;
    EventID next_event_;
    EventID previous_event_;



    //-------------------------------------------------------------------------
    static QString const FIT_TO_EVENT_;
    static QString const HIDE_EVENTS_OF_OTHER_TYPE_;
    static QString const SHOW_ALL_EVENTS_;
    static QString const GO_TO_NEXT_EVENT_;
    static QString const GO_TO_PREVIOUS_EVENT_;
    static QString const SET_SHOWN_EVENTS_;
    static QStringList const ACTIONS_;

    static GuiActionFactoryRegistrator registrator_;
};

} // namespace SigViewer_

#endif // FIT_VIEW_GUI_COMMAND_H
