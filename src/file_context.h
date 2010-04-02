#ifndef FILE_CONTEXT_H
#define FILE_CONTEXT_H

#include <QObject>

namespace BioSig_
{

class EventManager;
class TabContext;

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
    FileContext (EventManager& event_manager,
                 TabContext& tab_context);

    //-------------------------------------------------------------------------
    ~FileContext ();

    //-------------------------------------------------------------------------
    EventManager& getEventManager ();

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
    EventManager& event_manager_;
    TabContext& tab_context_;
};

} // namespace BioSig_

#endif // FILE_CONTEXT_H
