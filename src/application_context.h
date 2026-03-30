// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef APPLICATION_CONTEXT_H
#define APPLICATION_CONTEXT_H

#include "base/application_states.h"
#include "gui/color_manager.h"
#include "tab_context.h"
#include "file_context.h"
#include "command_executer.h"

#include <QObject>
#include <QSharedPointer>

#include <set>

namespace sigviewer
{

enum ApplicationMode
{
    APPLICATION_TEST_MODE,
    APPLICATION_NON_GUI_MODE
};

class MainWindowModel;

//-----------------------------------------------------------------------------
/// ApplicationContext
///
/// exists once in an application
class ApplicationContext : public QObject
{
    Q_OBJECT
public:
    static QSharedPointer<ApplicationContext> getInstance (bool cleanup = false);

    static void init (std::set<ApplicationMode> activated_modes);

    static void cleanup ();

    ApplicationContext () {}
    ~ApplicationContext ();

    bool modeActivated (ApplicationMode mode) const;

    QSharedPointer<FileContext> getCurrentFileContext ();

    void setCurrentTabContext (QSharedPointer<TabContext> tab_context);

    QSharedPointer<CommandExecuter> getCurrentCommandExecuter ();

    /// NO MULTI-FILE SUPPORT IMPLEMENTED YET!!!
    /// THIS CALL WILL REPLACE ACTUAL FILE CONTEXT
    void addFileContext (QSharedPointer<FileContext> file_context);

    void removeCurrentFileContext ();

    QSharedPointer<MainWindowModel> getMainWindowModel ();

    QSharedPointer<ColorManager> getEventColorManager ();

    QSharedPointer<ColorManager> color_manager_;

signals:
    void stateChanged (ApplicationState state);
    void currentFileStateChanged (FileState state);
    void currentTabSelectionStateChanged (TabSelectionState state);
    void currentTabEditStateChanged (TabEditState state);

private:
    void setState (ApplicationState state);
    std::set<ApplicationMode> activated_modes_;
    QSharedPointer<MainWindowModel> main_window_model_;
    QSharedPointer<FileContext> current_file_context_;
    QSharedPointer<TabContext> current_tab_context_;
    ApplicationState state_;
};

}

#endif // APPLICATION_CONTEXT_H
