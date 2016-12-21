// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef GUI_ACTION_FACTORY_REGISTRATOR_H
#define GUI_ACTION_FACTORY_REGISTRATOR_H

#include "gui_action_command.h"
#include "gui_action_factory.h"

#include <QSharedPointer>
#include <QString>

namespace sigviewer
{

class GuiActionFactoryRegistrator
{
public:
    GuiActionFactoryRegistrator (QString const& name, QSharedPointer<GuiActionCommand> command)
    {
        GuiActionFactory::getInstance()->registerCommand (name, command);
    }
};

}

#endif
