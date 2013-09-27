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


#ifndef APPLICATION_CONTEXT_H
#define APPLICATION_CONTEXT_H

#include "color_manager.h"
#include "main_window_model.h"
#include "command_executer.h"
#include "tab_context.h"

#include <QSharedPointer>

namespace SigViewer_
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
