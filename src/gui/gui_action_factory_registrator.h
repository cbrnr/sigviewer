// © SigViewer developers
//
// License: GPL-3.0

#ifndef GUI_ACTION_FACTORY_REGISTRATOR_H
#define GUI_ACTION_FACTORY_REGISTRATOR_H

#include <QMap>
#include <QSharedPointer>
#include <QString>

#include "gui_action_command.h"
#include "gui_action_factory.h"

namespace sigviewer {

class GuiActionCommandFactory {
   public:
    virtual ~GuiActionCommandFactory() = default;

    virtual QSharedPointer<GuiActionCommand> createCommand() = 0;
};

class GuiActionFactoryRegistrator {
   public:
    GuiActionFactoryRegistrator(QString const& name,
        QSharedPointer<GuiActionCommandFactory> command_factory) {
        factories_map()[name] = command_factory;
    }

    static void registerActions() {
        for (auto iter = factories_map().begin(); iter != factories_map().end(); ++iter) {
            GuiActionFactory::getInstance()
                ->registerCommand(iter.key(), iter.value()->createCommand());
        }
    }

   private:
    static QMap<QString, QSharedPointer<GuiActionCommandFactory> >& factories_map() {
        static QMap<QString, QSharedPointer<GuiActionCommandFactory> > value;

        return value;
    }
};

}  // namespace sigviewer

#endif
