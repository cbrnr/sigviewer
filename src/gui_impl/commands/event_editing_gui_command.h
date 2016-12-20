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


#ifndef EVENT_EDITING_GUI_COMMAND_H
#define EVENT_EDITING_GUI_COMMAND_H

#include "base/sigviewer_user_types.h"
#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"

#include <QUndoCommand>

namespace sigviewer
{

class EventEditingGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    EventEditingGuiCommand ();

    //-------------------------------------------------------------------------
    virtual ~EventEditingGuiCommand ();

    //-------------------------------------------------------------------------
    virtual void init ();

public slots:
    //-------------------------------------------------------------------------
    void deleteSelectedEvent ();

    //-------------------------------------------------------------------------
    void changeTypeSelectedEvent ();

    //-------------------------------------------------------------------------
    void changeChannelSelectedEvent ();

    //-------------------------------------------------------------------------
    void toAllChannelsSelectedEvent ();

    //-------------------------------------------------------------------------
    void copyToChannelsSelectedEvent ();

    //-------------------------------------------------------------------------
    void insertEventOverSelectedEvent ();

protected:
    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness ();

private:
    void executeCommands (QList<QSharedPointer<QUndoCommand> > commands);

    static QString const DELETE_;
    static QString const CHANGE_TYPE_;
    static QString const CHANGE_CHANNEL_;
    static QString const TO_ALL_CHANNEL_;
    static QString const COPY_TO_CHANNELS_;
    static QString const INSERT_OVER_;
    static QStringList const ACTIONS_;

    static GuiActionFactoryRegistrator registrator_;
};

}

#endif // EVENT_EDITING_GUI_COMMAND_H
