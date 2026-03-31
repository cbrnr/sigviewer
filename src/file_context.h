// © SigViewer developers
//
// License: GPL-3.0


#ifndef FILE_CONTEXT_H
#define FILE_CONTEXT_H

#include "file_handling/event_manager.h"
#include "file_handling/basic_header.h"
#include "file_handling/channel_manager.h"
#include "file_handling/channel_manager_proxy.h"
#include "file_handling/detrend_channel_manager.h"
#include "base/data_block.h"
#include "gui/signal_visualisation_model.h"
#include "base/file_states.h"

#include <QObject>
#include <QString>
#include <QSharedPointer>


namespace sigviewer
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
    /// Enable or disable offset removal (mean subtraction + optional FIR
    /// high-pass filter).  A high-pass cutoff of 0.0 means mean-only.
    /// Returns true if the filter was (re)built, false if the existing cache
    /// was reused (e.g. toggling off then back on with the same parameters).
    bool setDetrendEnabled (bool enabled, double hp_cutoff_hz = 0.0);

    //-------------------------------------------------------------------------
    /// Pre-warms the detrend manager for a single channel during file load.
    /// Creates the DetrendChannelManager if it does not yet exist or if the
    /// cutoff changed.  Does NOT enable detrend — user still toggles it on.
    void precomputeDetrendChannel (double cutoff_hz, ChannelID id);

    //-------------------------------------------------------------------------
    /// Ensures the DetrendChannelManager exists with the given cutoff.
    /// Must be called on the main thread before any parallel
    /// precomputeDetrendChannelFromRaw() calls.
    void ensureDetrendManager (double cutoff_hz);

    //-------------------------------------------------------------------------
    /// Pre-compute a single channel from already-read raw data.
    /// Thread-safe — multiple channels can be processed concurrently once
    /// ensureDetrendManager() has been called on the main thread.
    void precomputeDetrendChannelFromRaw (ChannelID id,
                                          QSharedPointer<DataBlock const> raw);

    //-------------------------------------------------------------------------
    bool isDetrendEnabled () const { return detrend_enabled_; }

    //-------------------------------------------------------------------------
    double getDetrendCutoffHz () const { return detrend_cutoff_hz_; }

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
    ChannelManager* channel_manager_;          ///< raw source (owned)
    DetrendChannelManager* detrend_manager_;   ///< detrend wrapper (owned, may be null)
    ChannelManagerProxy* proxy_manager_;       ///< stable pointer given to views (owned)
    bool detrend_enabled_;
    double detrend_cutoff_hz_;
    QSharedPointer<BasicHeader> basic_header_;
    QSharedPointer<SignalVisualisationModel> main_signal_vis_model_;
};

}

#endif // FILE_CONTEXT_H
