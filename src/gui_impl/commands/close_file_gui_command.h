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


#ifndef CLOSE_FILE_GUI_COMMAND_H
#define CLOSE_FILE_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"
#include "file_handling/file_signal_reader.h"

namespace sigviewer
{

//-----------------------------------------------------------------------------
/// CloseFileGuiCommand
///
/// command for closing a file
class CloseFileGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    CloseFileGuiCommand ();

    //-------------------------------------------------------------------------
    virtual ~CloseFileGuiCommand () {}

    //-------------------------------------------------------------------------
    virtual void init ();

    //-------------------------------------------------------------------------
    bool closeCurrentFile ();

public slots:
    //-------------------------------------------------------------------------
    void closeFile ();

    //-------------------------------------------------------------------------
    void exitApplication ();


protected:
    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness ();


private:
    static QString const CLOSE_FILE_;
    static QString const EXIT_APPLICATION_;
    static QStringList const ACTIONS_;

    static GuiActionFactoryRegistrator registrator_;



    //-------------------------------------------------------------------------
    QString showCloseDialog (QString const& path, QString const& extensions);

    //-------------------------------------------------------------------------
    QSharedPointer<FileSignalReader> createAndCloseFileSignalReader
            (QString const& file_path) const;


};

} // namespace SigViewer_


#endif // CLOSE_FILE_GUI_COMMAND_H
