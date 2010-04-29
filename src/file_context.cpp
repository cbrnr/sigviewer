#include "file_context.h"

#include <QDir>

namespace BioSig_
{

//-----------------------------------------------------------------------------
FileContext::FileContext (QString const& file_path_and_name,
                          QSharedPointer<EventManager> event_manager,
                          QSharedPointer<ChannelManager> channel_manager,
                          QSharedPointer<FileSignalReader> file_signal_reader)
    : state_ (FILE_STATE_UNCHANGED),
      file_path_and_name_ (file_path_and_name),
      event_manager_ (event_manager),
      channel_manager_ (channel_manager),
      file_signal_reader_ (file_signal_reader)
{
    connect (event_manager_.data(), SIGNAL(changed()), SLOT(setAsChanged()));
}

//-----------------------------------------------------------------------------
FileContext::~FileContext ()
{
    // nothing to do here
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

//-----------------------------------------------------------------------------
QSharedPointer<EventManager> FileContext::getEventManager ()
{
    return event_manager_;
}

//-----------------------------------------------------------------------------
QSharedPointer<ChannelManager> FileContext::getChannelManager ()
{
    return channel_manager_;
}

//-----------------------------------------------------------------------------
QSharedPointer<FileSignalReader> FileContext::getFileSignalReader ()
{
    return file_signal_reader_;
}

//-------------------------------------------------------------------------
FileState FileContext::getState () const
{
    return state_;
}

//-------------------------------------------------------------------------
QSharedPointer<TabContext> FileContext::getMainTabContext ()
{
    return tab_context_;
}

//-------------------------------------------------------------------------
void FileContext::setMainTabContext (QSharedPointer<TabContext> tab_context)
{
    tab_context_ = tab_context;
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


} // namespace BioSig_
