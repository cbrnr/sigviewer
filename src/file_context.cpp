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


#include "file_context.h"

#include <QDir>
#include <QDebug>

namespace sigviewer
{

//-----------------------------------------------------------------------------
FileContext::FileContext (QString const& file_path_and_name,
                          QSharedPointer<EventManager> event_manager,
                          ChannelManager* channel_manager,
                          QSharedPointer<BasicHeader> header)
    : state_ (FILE_STATE_UNCHANGED),
      file_path_and_name_ (file_path_and_name),
      event_manager_ (event_manager),
      channel_manager_ (channel_manager),
      basic_header_ (header)
{
    connect (event_manager_.data(), SIGNAL(changed()), SLOT(setAsChanged()));
}

//-----------------------------------------------------------------------------
FileContext::~FileContext ()
{
    qDebug () << "deleting FileContext";
    delete channel_manager_;
}


//-------------------------------------------------------------------------
void FileContext::resetFilePathAndName (QString const& new_file_path_and_name)
{
    file_path_and_name_ = new_file_path_and_name;
    emit fileNameChanged (file_path_and_name_.section (QDir::separator(), -1));
}

//-------------------------------------------------------------------------
QString FileContext::getFilePath () const
{
    QString file_name = getFileName ();
    return file_path_and_name_.left (file_path_and_name_.length() - file_name.length());
}

//-----------------------------------------------------------------------------
QString const& FileContext::getFilePathAndName () const
{
    return file_path_and_name_;
}

//-----------------------------------------------------------------------------
QString FileContext::getFileName () const
{
    return file_path_and_name_.section (QDir::separator(), -1);
}

//-------------------------------------------------------------------------
QSharedPointer<SignalVisualisationModel> FileContext::getMainVisualisationModel ()
{
    return main_signal_vis_model_;
}

//-------------------------------------------------------------------------
void FileContext::setMainVisualisationModel (QSharedPointer<SignalVisualisationModel> signal_vis_model)
{
    main_signal_vis_model_ = signal_vis_model;
}

//-----------------------------------------------------------------------------
QSharedPointer<EventManager const> FileContext::getEventManager () const
{
    return event_manager_;
}

//-----------------------------------------------------------------------------
QSharedPointer<EventManager> FileContext::getEventManager ()
{
    return event_manager_;
}

//-----------------------------------------------------------------------------
ChannelManager const& FileContext::getChannelManager () const
{
    return *channel_manager_;
}

//-----------------------------------------------------------------------------
ChannelManager& FileContext::getChannelManager ()
{
    return *channel_manager_;
}

//-------------------------------------------------------------------------
FileState FileContext::getState () const
{
    return state_;
}

//-----------------------------------------------------------------------------
void FileContext::setState (FileState state)
{
    state_ = state;
    emit stateChanged (state_);
}

//-----------------------------------------------------------------------------
void FileContext::setAsChanged ()
{
    state_ = FILE_STATE_CHANGED;
    emit stateChanged (state_);
}


} // namespace SigViewer_
