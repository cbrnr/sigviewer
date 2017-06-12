// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef APPLICATION_CONTEXT_H
#define APPLICATION_CONTEXT_H

#include "color_manager.h"
#include "main_window_model.h"
#include "tab_context.h"

#include <QSharedPointer>

namespace sigviewer
{

enum ApplicationMode
{
    APPLICATION_TEST_MODE,
    APPLICATION_NON_GUI_MODE
};

///----------------------------------------------------------------------------
/// ApplicationContext
class ApplicationContext
{
public:
    //-------------------------------------------------------------------------
    virtual ~ApplicationContext () {}

    //-------------------------------------------------------------------------
    virtual bool modeActivated (ApplicationMode mode) const = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<FileContext> getCurrentFileContext () = 0;

    //-------------------------------------------------------------------------
    virtual void removeCurrentFileContext () = 0;

    //-------------------------------------------------------------------------
    virtual void addFileContext (QSharedPointer<FileContext>file_context) = 0;

    //-------------------------------------------------------------------------
    virtual void setCurrentTabContext (QSharedPointer<TabContext> tab_context) = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<CommandExecuter> getCurrentCommandExecuter () = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<MainWindowModel> getMainWindowModel () = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<ColorManager> getEventColorManager () = 0;
};

}

#endif // APPLICATION_CONTEXT_H
