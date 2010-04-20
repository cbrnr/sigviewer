#include "gui_action_factory.h"

namespace BioSig_
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
void GuiActionFactory::registerCommand (QString const& name,
                                        QSharedPointer<GuiActionCommand> command)
{
    command_map_[name] =  command;
}



//-----------------------------------------------------------------------------
QList<QAction*> GuiActionFactory::getQActions (QString const& command_name) const
{
    if (!command_map_.contains (command_name))
        return QList<QAction*>();
    else
        return command_map_[command_name]->getQActions ();
}

//-----------------------------------------------------------------------------
void GuiActionFactory::initAllCommands ()
{
    for (CommandMap::iterator cmd_iter = command_map_.begin ();
         cmd_iter != command_map_.end ();
         ++cmd_iter)
        cmd_iter.value()->init ();
}



} // namespace BioSig_
