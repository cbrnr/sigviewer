#include "gui_action_factory.h"

namespace SigViewer_
{

GuiActionFactory* GuiActionFactory::instance_ = 0;

//-----------------------------------------------------------------------------
GuiActionFactory* GuiActionFactory::getInstance ()
{
    if (!instance_)
    {
        instance_ = new GuiActionFactory;
    }
    return instance_;
}

//-----------------------------------------------------------------------------
void GuiActionFactory::registerCommand (QString const& name, QSharedPointer<GuiActionCommand> command)
{
    command_map_[name] = command;
    QStringList action_ids = command->getActionIDs();
    for (QStringList::const_iterator id_iter = action_ids.begin ();
         id_iter != action_ids.end ();
         ++id_iter)
        action_to_command_map_[*id_iter] = command;
}


//-----------------------------------------------------------------------------
QList<QAction*> GuiActionFactory::getQActions (QString const& command_name) const
{
    if (command_name.size() == 0)
    {
        QList<QAction*> actions;
        foreach (QSharedPointer<GuiActionCommand> command, command_map_.values())
            actions.append (command->getQActions());
        return actions;
    }
    else if (!command_map_.contains (command_name))
        return QList<QAction*>();
    else
        return command_map_[command_name]->getQActions ();
}

//-----------------------------------------------------------------------------
QAction* GuiActionFactory::getQAction (QString const& action_id) const
{
    if (!action_to_command_map_.contains (action_id))
        return 0;
    else
        return action_to_command_map_[action_id]->getQAction (action_id);
}


//-----------------------------------------------------------------------------
void GuiActionFactory::initAllCommands ()
{
    for (CommandMap::iterator cmd_iter = command_map_.begin ();
         cmd_iter != command_map_.end ();
         ++cmd_iter)
        cmd_iter.value()->init ();
}



} // namespace SigViewer_
