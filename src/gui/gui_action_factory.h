#ifndef GUI_ACTION_FACTORY_H
#define GUI_ACTION_FACTORY_H

#include "gui_action_command.h"

#include <QSharedPointer>
#include <QString>
#include <QMap>


namespace BioSig_
{

class GuiActionFactory
{
public:
    static GuiActionFactory* getInstance ();

    void registerCommand (QString const& name, QSharedPointer<GuiActionCommand> command);

    //-------------------------------------------------------------------------
    /// the caller must not delete
    QAction* getQAction (QString const& name) const;

    //-------------------------------------------------------------------------
    void initAllCommands ();
private:
    static GuiActionFactory* instance_;
    typedef QMap<QString, QSharedPointer<GuiActionCommand> > CommandMap;
    CommandMap command_map_;
};

} // namespace BioSig_

#endif // GUI_ACTION_FACTORY_H
