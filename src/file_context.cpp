// © SigViewer developers
//
// License: GPL-3.0


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
      detrend_manager_ (nullptr),
      proxy_manager_ (new ChannelManagerProxy (channel_manager)),
      detrend_enabled_ (false),
      detrend_cutoff_hz_ (0.0),
      basic_header_ (header)
{
    connect (event_manager_.data(), SIGNAL(changed()), SLOT(setAsChanged()));
}

//-----------------------------------------------------------------------------
FileContext::~FileContext ()
{
    qDebug () << "deleting FileContext";
    delete proxy_manager_;
    delete detrend_manager_;
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
    return *proxy_manager_;
}

//-----------------------------------------------------------------------------
ChannelManager& FileContext::getChannelManager ()
{
    return *proxy_manager_;
}

//-------------------------------------------------------------------------
void FileContext::precomputeDetrendChannel (double cutoff_hz, ChannelID id)
{
    if (!detrend_manager_ || detrend_manager_->hpCutoff () != cutoff_hz)
    {
        delete detrend_manager_;
        detrend_manager_  = new DetrendChannelManager (channel_manager_, cutoff_hz);
        detrend_cutoff_hz_ = cutoff_hz;
    }
    // A getData(id, 0, 1) call is enough to trigger and cache processedChannel()
    // which also populates the per-channel min/max cache.
    detrend_manager_->getData (id, 0, 1);
}

//-------------------------------------------------------------------------
bool FileContext::setDetrendEnabled (bool enabled, double hp_cutoff_hz)
{
    detrend_enabled_   = enabled;
    detrend_cutoff_hz_ = hp_cutoff_hz;

    bool rebuilt = false;
    if (enabled)
    {
        // Only rebuild if parameters changed or no manager exists yet.
        if (!detrend_manager_ || detrend_manager_->hpCutoff () != hp_cutoff_hz)
        {
            delete detrend_manager_;
            detrend_manager_ = new DetrendChannelManager (channel_manager_, hp_cutoff_hz);
            rebuilt = true;
        }
        proxy_manager_->setTarget (detrend_manager_);
    }
    else
    {
        proxy_manager_->setTarget (channel_manager_);
        // Keep detrend_manager_ alive so toggling back on reuses the cache.
    }
    return rebuilt;
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

}
