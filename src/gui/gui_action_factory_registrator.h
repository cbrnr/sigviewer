#ifndef GUI_ACTION_FACTORY_REGISTRATOR_H
#define GUI_ACTION_FACTORY_REGISTRATOR_H

#include "gui_action_command.h"
#include "gui_action_factory.h"

#include <QSharedPointer>
#include <QString>

namespace SigViewer_
{

class GuiActionFactoryRegistrator
{
public:
    GuiActionFactoryRegistrator (QString const& name, QSharedPointer<GuiActionCommand> command)
    {
        GuiActionFactory::getInstance()->registerCommand (name, command);
    }
};

} // namespace SigViewer_

#endif // GUI_ACTION_FACTORY_REGISTRATOR_H
