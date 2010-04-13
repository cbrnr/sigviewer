#ifndef FILE_CONTEXT_H
#define FILE_CONTEXT_H

#include <QObject>
#include <QString>

namespace BioSig_
{

class EventManager;
class TabContext;
class ChannelManager;

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
    FileContext (QString const & file_name,
                 EventManager& event_manager,
                 ChannelManager& channel_manager,
                 TabContext& tab_context);

    //-------------------------------------------------------------------------
    ~FileContext ();

    //-------------------------------------------------------------------------
    QString const& getFileName () const;

    //-------------------------------------------------------------------------
    EventManager& getEventManager ();

    //-------------------------------------------------------------------------
    ChannelManager& getChannelManager ();

    //-------------------------------------------------------------------------
    FileState getState () const;

    //-------------------------------------------------------------------------
    TabContext& getMainTabContext ();

signals:
    //-------------------------------------------------------------------------
    void stateChanged (FileState state);

public slots:
    //-------------------------------------------------------------------------
    void setState (FileState state);

private:
    //-------------------------------------------------------------------------
    // disabled
    FileContext (FileContext const&);
    FileContext& operator= (FileContext const&);

    FileState state_;
    QString file_name_;
    EventManager& event_manager_;
    ChannelManager& channel_manager_;
    TabContext& tab_context_;
};

} // namespace BioSig_

#endif // FILE_CONTEXT_H
