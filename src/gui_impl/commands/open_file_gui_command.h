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


#ifndef OPEN_FILE_GUI_COMMAND_H
#define OPEN_FILE_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"
#include "file_handling/file_signal_reader.h"

namespace SigViewer_
{

//-----------------------------------------------------------------------------
/// OpenFileGuiCommand
///
/// command for opening a file
class OpenFileGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    OpenFileGuiCommand ();

    //-------------------------------------------------------------------------
    virtual ~OpenFileGuiCommand ();

    //-------------------------------------------------------------------------
    virtual void init ();

    //-------------------------------------------------------------------------
    static void openFile (QString file_path, bool instantly = true);

protected:
    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness ();


private slots:
    //-------------------------------------------------------------------------
    void open ();

    //-------------------------------------------------------------------------
    void importEvents ();

    //-------------------------------------------------------------------------
    void showFileInfo ();

private:

    static QString const IMPORT_EVENTS_;
    static QString const OPEN_;
    static QString const SHOW_FILE_INFO_;
    static QStringList const ACTIONS_;


    static QSharedPointer<OpenFileGuiCommand> instance_;

    static GuiActionFactoryRegistrator registrator_;

    //-------------------------------------------------------------------------
    void openFileImpl (QString file_path, bool instantly = true);

    //-------------------------------------------------------------------------
    QString showOpenDialog (QString const& path, QString const& extensions);
};

} // namespace SigViewer_

#endif // OPEN_FILE_GUI_COMMAND_H
