// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl



#ifndef XDF_READER_H_
#define XDF_READER_H_

#include "file_handling/file_signal_reader.h"
#include "xdf.h"

#include <QFile>
#include <QMutex>
#include <QMap>

namespace sigviewer
{

//the object to store XDF data
extern QSharedPointer<Xdf> XDFdata;

//XDFReader, modeled  on BiosigReader
class XDFReader : public FileSignalReader
{
public:
    XDFReader ();

    virtual ~XDFReader();

    //-------------------------------------------------------------------------
    QPair<FileSignalReader*, QString> createInstance (QString const& file_path);

    //-------------------------------------------------------------------------
    virtual QSharedPointer<DataBlock const> getSignalData (ChannelID channel_id,
                                                           size_t start_sample,
                                                           size_t length) const;

    //-------------------------------------------------------------------------
    virtual QList<QSharedPointer<SignalEvent const> > getEvents () const;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<BasicHeader> getBasicHeader ();

    //-------------------------------------------------------------------------
    virtual QSharedPointer<BasicHeader const> getBasicHeader () const {return basic_header_;}

    //-------------------------------------------------------------------------
    int setStreamColors();  /*!< Set a distinct color for each stream. */

    //-------------------------------------------------------------------------
    enum sampleRateTypes
    {
        No_streams_found,
        Zero_Hz_Only,
        Mono_Sample_Rate,
        Multi_Sample_Rate
    };

    sampleRateTypes selectSampleRateType();

private:
    //-------------------------------------------------------------------------
    QString open (QString const& file_path);

    //-------------------------------------------------------------------------
    void bufferAllChannels () const;

    //-------------------------------------------------------------------------
    void bufferAllEvents () const;

    Q_DISABLE_COPY(XDFReader)

    QString loadFixedHeader(const QString& file_path);

    QSharedPointer<BasicHeader> basic_header_;
    mutable QMutex mutex_;
    mutable QMutex xdf_access_lock_;
    mutable bool buffered_all_channels_;
    mutable bool buffered_all_events_;
    mutable QMap<ChannelID, QSharedPointer<DataBlock const> > channel_map_;
    mutable QList<QSharedPointer<SignalEvent const> > events_;
};

} // namespace sigviewer

#endif /*XDF_READER_H_*/
