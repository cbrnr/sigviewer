#ifndef FILE_CONTEXT_H
#define FILE_CONTEXT_H

#include "tab_context.h"
#include "file_handling/event_manager.h"
#include "file_handling/file_signal_reader.h"
#include "file_handling/channel_manager.h"

#include <QObject>
#include <QString>
#include <QSharedPointer>

namespace BioSig_
{

enum FileState
{
    FILE_STATE_UNCHANGED,
    FILE_STATE_CHANGED
};

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
    FileContext (QString const & file_path_and_name,
                 QSharedPointer<EventManager> event_manager,
                 QSharedPointer<ChannelManager> channel_manager,
                 QSharedPointer<FileSignalReader> file_signal_reader);

    //-------------------------------------------------------------------------
    ~FileContext ();

    //-------------------------------------------------------------------------
    QString getFilePath () const;

    //-------------------------------------------------------------------------
    QString const& getFilePathAndName () const;

    //-------------------------------------------------------------------------
    QString getFileName () const;

    //-------------------------------------------------------------------------
    QSharedPointer<EventManager> getEventManager ();

    //-------------------------------------------------------------------------
    QSharedPointer<ChannelManager> getChannelManager ();

    //-------------------------------------------------------------------------
    QSharedPointer<FileSignalReader> getFileSignalReader ();

    //-------------------------------------------------------------------------
    FileState getState () const;

    //-------------------------------------------------------------------------
    QSharedPointer<TabContext> getMainTabContext ();

    //-------------------------------------------------------------------------
    void setMainTabContext (QSharedPointer<TabContext> tab_context);

signals:
    //-------------------------------------------------------------------------
    void stateChanged (FileState state);

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
    QSharedPointer<ChannelManager> channel_manager_;
    QSharedPointer<TabContext> tab_context_;
    QSharedPointer<FileSignalReader> file_signal_reader_;
};

} // namespace BioSig_

#endif // FILE_CONTEXT_H
