#ifndef APPLICATION_CONTEXT_H
#define APPLICATION_CONTEXT_H

namespace BioSig_
{

class GUIActionManager;
class MainWindowModel;

//-----------------------------------------------------------------------------
/// ApplicationContext
///
/// exists once in an application
/// holds instances of GUIActionManager
class ApplicationContext
{
public:
    //-------------------------------------------------------------------------
    ApplicationContext (GUIActionManager& gui_action_manager,
                        MainWindowModel& main_window_model);

    //-------------------------------------------------------------------------
    GUIActionManager& getGUIActionManager ();

    //-------------------------------------------------------------------------
    MainWindowModel& getMainWindowModel ();

private:
    GUIActionManager& gui_action_manager_;
    MainWindowModel& main_window_model_;
};

} // namespace BioSig_

#endif // APPLICATION_CONTEXT_H
