#ifndef GUI_ACTION_MANAGER_H
#define GUI_ACTION_MANAGER_H

#include "application_context.h"
#include "file_context.h"
#include "tab_context.h"

#include <QObject>
#include <QAction>
#include <QMenu>
#include <QString>

#include <map>
#include <list>

namespace BioSig_
{

class MainWindowModel;

//-----------------------------------------------------------------------------
///
/// GUIActionManager
///
/// manages all QAction-objects needed on different places of the GUI
/// e.g.: Actions that appears in the Menu, in ToolBars and/or in ContextMenus
///
/// Management of QAction-objects includes management of...
///    - short cuts
///    - action names/titles
///    - status tips
///    - icons
///    - enabledness in different states
///    - SLOT: method which should be called when action is triggered
///
class GUIActionManager : public QObject
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    enum ActionGroup
    {
        EDIT_MENU_ACTIONS,
        EDIT_TOOLBAR_ACTIONS,
        EVENT_CONTEXT_ACTIONS,
        FILE_MENU_ACTIONS,
        FILE_TOOLBAR_ACTIONS,
        MODE_ACTIONS,
        VIEW_MENU_ACTIONS,
        VIEW_TOOLBAR_ACTIONS
    };


    //-------------------------------------------------------------------------
    GUIActionManager ();
    virtual ~GUIActionManager ();

    //-------------------------------------------------------------------------
    /// @param main_window_model the given model will act as the receiver of
    ///                          all actions
    void init (MainWindowModel* main_window_model, ApplicationContext*
               app_context);

    //-------------------------------------------------------------------------
    /// creates a QMenu with all actions of the given group
    ///
    /// @param group the action group
    /// @param parent the parent of the menu (which will care for destruction)
    /// @return a QMenu containing all actions of the given group
    QMenu* getGroupAsMenu (ActionGroup group, QString const& title,
                           QWidget* parent);

    //-------------------------------------------------------------------------
    QList<QAction*> getActionsOfGroup (ActionGroup group);

public slots:
    //-------------------------------------------------------------------------
    void setApplicationState (ApplicationState application_state);

    //-------------------------------------------------------------------------
    void setFileState (FileState file_state);

    //-------------------------------------------------------------------------
    void setTabSelectionState (TabSelectionState tab_state);

    //-------------------------------------------------------------------------
    void setTabEditState (TabEditState tab_state);

private:
    //-------------------------------------------------------------------------
    enum GUIAction
    {
        ACTION_FILE_OPEN,
        ACTION_FILE_SAVE,
        ACTION_FILE_SAVE_AS,
        ACTION_EXPORT_EVENTS,
        ACTION_IMPORT_EVENTS,
        ACTION_FILE_INFO,
        ACTION_FILE_CLOSE,
        ACTION_EXIT,
        ACTION_SEPARATOR,
        ACTION_UNDO,
        ACTION_REDO,
        ACTION_TO_ALL_CHANNELS,
        ACTION_COPY_TO_CHANNELS,
        ACTION_DELETE,
        ACTION_INSERT_OVER,
        ACTION_CHANGE_CHANNEL,
        ACTION_CHANGE_TYPE,
        ACTION_SHOW_EVENT_TABLE,
        ACTION_MODE_NEW,
        ACTION_MODE_POINTER,
        ACTION_MODE_HAND,
        ACTION_MODE_SHIFT,
        ACTION_VIEW_ZOOM_IN,
        ACTION_VIEW_ZOOM_OUT,
        ACTION_VIEW_AUTO_SCALE,
        ACTION_VIEW_GOTO,
        ACTION_VIEW_GOTO_NEXT_EVENT,
        ACTION_VIEW_GOTO_PREVIOUS_EVENT,
        ACTION_VIEW_HIDE_EVENTS_OF_OTHER_TYPE,
        ACTION_VIEW_FIT_TO_EVENT,
        ACTION_VIEW_CHANNELS,
        ACTION_VIEW_EVENTS
    };

    //-------------------------------------------------------------------------
    // disabled assignment and copy-ctr
    GUIActionManager (GUIActionManager const&);
    GUIActionManager& operator= (GUIActionManager const&);

    //-------------------------------------------------------------------------
    void initFileActions ();

    //-------------------------------------------------------------------------
    void initEditActions ();

    //-------------------------------------------------------------------------
    void initMouseModeActions ();

    //-------------------------------------------------------------------------
    void initViewActions ();

    //-------------------------------------------------------------------------
    void initShortcuts ();

    //-------------------------------------------------------------------------
    void initGroups ();

    //-------------------------------------------------------------------------
    void initDisabledStates ();

    //-------------------------------------------------------------------------
    QAction* createAction (GUIAction action_id,
                           QString const& text,
                           char const * const slot,
                           QString const& status_tip,
                           QIcon const& icon = QIcon ());

    //-------------------------------------------------------------------------
    QAction* createSeparator ();

    //-------------------------------------------------------------------------
    void setShortCut (GUIAction const& gui_action,
                      QKeySequence const& key_sequence);


    //-------------------------------------------------------------------------
    // some private typedefs
    typedef std::list<GUIAction> ActionList;
    typedef std::map<GUIAction, QAction*> ActionMap;
    typedef std::map<ActionGroup, ActionList > ActionGroupMap;
    typedef std::map<ApplicationState, ActionList > ActionAppStateMap;
    typedef std::map<FileState, ActionList > ActionFileStateMap;
    typedef std::map<TabSelectionState, ActionList > ActionTabSelectionStateMap;
    typedef std::map<TabEditState, ActionList > ActionTabEditStateMap;

    //-------------------------------------------------------------------------
    void updateAllActionsDisabling ();

    //-------------------------------------------------------------------------
    void setActionsEnabled (ActionList& action_list, bool enabled);

    MainWindowModel* main_window_model_;

    ApplicationState application_state_;
    FileState file_state_;
    TabSelectionState tab_selection_state_;
    TabEditState tab_edit_state_;

    ActionMap action_map_;
    ActionGroupMap action_group_map_;
    ActionAppStateMap app_state_action_map_;
    ActionFileStateMap file_state_action_map_;
    ActionTabSelectionStateMap tab_selection_state_action_map_;
    ActionTabEditStateMap tab_edit_state_action_map_;
};

} // namespace BioSig_

#endif // GUI_ACTION_MANAGER_H
