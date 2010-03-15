#ifndef APPLICATION_CONTEXT_H
#define APPLICATION_CONTEXT_H

#include <QObject>

namespace BioSig_
{

class GUIActionManager;

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
    ApplicationContext (GUIActionManager& gui_action_manager);

    //-------------------------------------------------------------------------
    GUIActionManager& getGUIActionManager ();

    //-------------------------------------------------------------------------
    ApplicationState getState () const;

public slots:
    void setState (ApplicationState state);

signals:
    void stateChanged (ApplicationState state);

private:
    GUIActionManager& gui_action_manager_;
    ApplicationState state_;
};

} // namespace BioSig_

#endif // APPLICATION_CONTEXT_H
