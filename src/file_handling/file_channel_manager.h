// © SigViewer developers
//
// License: GPL-3.0

#ifndef FILE_CHANNEL_MANAGER_H
#define FILE_CHANNEL_MANAGER_H

#include "channel_manager.h"
#include "file_signal_reader.h"

namespace sigviewer {

//-----------------------------------------------------------------------------
/// FileChannelManager
///
/// implementation of ChannelManager
///
class FileChannelManager : public ChannelManager {
   public:
    //---------------------------------------------------------------------------------------------
    /// @param file_signal_reader the FileChannelManager takes ownership of the given FileSignalReader
    ///                           and destroys it if the FileChannelManager is destructed
    FileChannelManager(FileSignalReader* file_signal_reader);

    //---------------------------------------------------------------------------------------------
    virtual ~FileChannelManager();

    //-------------------------------------------------------------------------
    virtual std::set<ChannelID> getChannels() const;

    //-------------------------------------------------------------------------
    virtual uint32 getNumberChannels() const;

    //-------------------------------------------------------------------------
    virtual QString getChannelLabel(ChannelID id) const;

    //-------------------------------------------------------------------------
    virtual QString getChannelLabel(ChannelID id, int streamNumber) const;

    //-------------------------------------------------------------------------
    virtual QString getChannelYUnitString(ChannelID id) const;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<DataBlock const> getData(ChannelID id, unsigned start_pos, unsigned length) const;

    //-------------------------------------------------------------------------
    virtual float64 getDurationInSec() const;

    //-------------------------------------------------------------------------
    virtual size_t getNumberSamples() const;

    //-------------------------------------------------------------------------
    virtual float64 getSampleRate() const;

   private:
    FileSignalReader* reader_;
};

}  // namespace sigviewer

#endif
