#ifndef APPLICATION_CONTEXT_H
#define APPLICATION_CONTEXT_H

#include <QObject>
#include <QSharedPointer>

namespace BioSig_
{

class GUIActionManager;
class MainWindowModel;
class ApplicationContextImpl;
class EventTableFileReader;
class FileContext;

enum ApplicationState
{
    APP_STATE_NO_FILE_OPEN,
    APP_STATE_FILE_OPEN
};

//-----------------------------------------------------------------------------
/// ApplicationContext
///
/// exists once in an application
/// holds instances of GUIActionManager
class ApplicationContext : public QObject
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    static ApplicationContext* getInstance ();

    //-------------------------------------------------------------------------
    void setImpl (QSharedPointer<ApplicationContextImpl> impl);

    //-------------------------------------------------------------------------
    ApplicationContext ();

    //-------------------------------------------------------------------------
    QSharedPointer<FileContext> getCurrentFileContext () const;

    //-------------------------------------------------------------------------
    /// NO MULTI-FILE SUPPORT IMPLEMENTED YET!!!
    /// THIS CALL WILL REPLACE ACTUAL FILE CONTEXT
    void addFileContext (QSharedPointer<FileContext> file_context);

    //-------------------------------------------------------------------------
    QSharedPointer<GUIActionManager> getGUIActionManager ();

    //-------------------------------------------------------------------------
    ApplicationState getState () const;

    //-------------------------------------------------------------------------
    QSharedPointer<MainWindowModel> getMainWindowModel () const;

    //-------------------------------------------------------------------------
    QSharedPointer<EventTableFileReader> getEventTableFileReader () const;

public slots:
    void setState (ApplicationState state);

signals:
    void stateChanged (ApplicationState state);

private:
    static ApplicationContext* instance_;
    QSharedPointer<ApplicationContextImpl> impl_;
    QSharedPointer<FileContext> current_file_context_;
    ApplicationState state_;
};

} // namespace BioSig_

#endif // APPLICATION_CONTEXT_H
