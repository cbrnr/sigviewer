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


#ifndef FILE_CONTEXT_H
#define FILE_CONTEXT_H

#include "file_handling/event_manager.h"
#include "file_handling/basic_header.h"
#include "file_handling/channel_manager.h"
#include "gui/signal_visualisation_model.h"
#include "base/file_states.h"

#include <QObject>
#include <QString>
#include <QSharedPointer>

namespace SigViewer_
{


//-----------------------------------------------------------------------------
///
/// FileContext
///
/// exists once per opened file...
///
class FileContext : public QObject
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    ///
    /// @param channel_manager the FileContext takes ownership of the given
    ///                        ChannelManager and destroys it if the FileContext
    ///                        is deleted
    FileContext (QString const & file_path_and_name,
                 QSharedPointer<EventManager> event_manager,
                 ChannelManager* channel_manager,
                 QSharedPointer<BasicHeader> header);

    //-------------------------------------------------------------------------
    ~FileContext ();

    //-------------------------------------------------------------------------
    void resetFilePathAndName (QString const& new_file_path_and_name);

    //-------------------------------------------------------------------------
    QString getFilePath () const;

    //-------------------------------------------------------------------------
    QString const& getFilePathAndName () const;

    //-------------------------------------------------------------------------
    QString getFileName () const;

    //-------------------------------------------------------------------------
    QSharedPointer<SignalVisualisationModel> getMainVisualisationModel ();

    //-------------------------------------------------------------------------
    void setMainVisualisationModel (QSharedPointer<SignalVisualisationModel> signal_vis_model);

    //-------------------------------------------------------------------------
    QSharedPointer<EventManager const> getEventManager () const;

    //-------------------------------------------------------------------------
    QSharedPointer<EventManager> getEventManager ();

    //-------------------------------------------------------------------------
    ChannelManager const & getChannelManager () const;

    //-------------------------------------------------------------------------
    ChannelManager& getChannelManager ();

    //-------------------------------------------------------------------------
    QSharedPointer<BasicHeader> getHeader () {return basic_header_;}

    //-------------------------------------------------------------------------
    FileState getState () const;

signals:
    //-------------------------------------------------------------------------
    void stateChanged (FileState state);

    //-------------------------------------------------------------------------
    void fileNameChanged (QString const& file_path_and_name);

public slots:
    //-------------------------------------------------------------------------
    void setState (FileState state);

    //-------------------------------------------------------------------------
    void setAsChanged ();

private:
    //-------------------------------------------------------------------------
    // disabled
    FileContext (FileContext const&);
    FileContext& operator= (FileContext const&);

    FileState state_;
    QString file_path_and_name_;
    QSharedPointer<EventManager> event_manager_;
    ChannelManager* channel_manager_;
    QSharedPointer<BasicHeader> basic_header_;
    QSharedPointer<SignalVisualisationModel> main_signal_vis_model_;
};

} // namespace SigViewer_

#endif // FILE_CONTEXT_H
