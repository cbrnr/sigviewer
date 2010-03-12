#ifndef APPLICATION_CONTEXT_H
#define APPLICATION_CONTEXT_H

#include <QObject>

namespace BioSig_
{

class GUIActionManager;
class MainWindowModel;

//-----------------------------------------------------------------------------
/// ApplicationContext
///
/// exists once in an application
/// holds instances of GUIActionManager
class ApplicationContext : public QObject
{
    Q_OBJECT
public:
    enum State
    {
        NO_FILE_OPEN,
        FILE_OPEN
    };

    //-------------------------------------------------------------------------
    ApplicationContext (GUIActionManager& gui_action_manager,
                        MainWindowModel& main_window_model);

    //-------------------------------------------------------------------------
    GUIActionManager& getGUIActionManager ();

    //-------------------------------------------------------------------------
    MainWindowModel& getMainWindowModel ();

public slots:
    void setState (State state);

signals:
    void stateChanged (State state);

private:
    GUIActionManager& gui_action_manager_;
    MainWindowModel& main_window_model_;
    State state_;
};

} // namespace BioSig_

#endif // APPLICATION_CONTEXT_H
