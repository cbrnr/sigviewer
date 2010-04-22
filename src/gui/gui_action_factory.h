#ifndef GUI_ACTION_FACTORY_H
#define GUI_ACTION_FACTORY_H

#include "gui_action_command.h"

#include <QSharedPointer>
#include <QString>
#include <QMap>
#include <QMenu>

namespace BioSig_
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
    QList<QAction*> getQActions (QString const& command_name) const;

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

} // namespace BioSig_

#endif // GUI_ACTION_FACTORY_H
