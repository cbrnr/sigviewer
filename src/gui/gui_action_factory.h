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


#ifndef GUI_ACTION_FACTORY_H
#define GUI_ACTION_FACTORY_H

#include "gui_action_command.h"

#include <QSharedPointer>
#include <QString>
#include <QMap>
#include <QMenu>

namespace SigViewer_
{

class GuiActionFactory
{
public:
    static GuiActionFactory* getInstance ();

    //-------------------------------------------------------------------------
    void registerCommand (QString const& name,
                          QSharedPointer<GuiActionCommand> command);

    //-------------------------------------------------------------------------
    /// the caller must not delete
    QList<QAction*> getQActions (QString const& command_name = "") const;

    //-------------------------------------------------------------------------
    /// the caller must not delete
    QAction* getQAction (QString const& action_id) const;

    //-------------------------------------------------------------------------
    void initAllCommands ();
private:
    static GuiActionFactory* instance_;
    typedef QMap<QString, QSharedPointer<GuiActionCommand> > CommandMap;
    CommandMap command_map_;
    CommandMap action_to_command_map_;
};

} // namespace SigViewer_

#endif // GUI_ACTION_FACTORY_H
