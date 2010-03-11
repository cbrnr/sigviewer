#ifndef GUI_ACTION_MANAGER_H
#define GUI_ACTION_MANAGER_H

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
class GUIActionManager : public QObject
{
    Q_OBJECT
public:
    enum Mode
    {
        MODE_ZERO,
        MODE_NO_FILE,
        MODE_FILE_OPEN,
        MODE_EVENT_SELECTED_ONE_CHANNEL,
        MODE_EVENT_SELECTED_ALL_CHANNELS
    };

    enum ActionGroup
    {
        EVENT_EDITING_ACTIONS,
        FILE_ACTIONS
    };

    //-------------------------------------------------------------------------
    /// @param main_window_model the given model will act as the receiver of
    ///                          all actions
    GUIActionManager (MainWindowModel* main_window_model);
    virtual ~GUIActionManager ();


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
    void setMode (Mode mode);

private:
    //-------------------------------------------------------------------------
    // disabled assignment and copy-ctr
    GUIActionManager (GUIActionManager const&);
    GUIActionManager& operator= (GUIActionManager const&);

    //-------------------------------------------------------------------------
    void addAction (ActionGroup group, QString const& text,
                    char const * const slot,
                    QString const& status_tip = tr(""),
                    Mode mode_of_disabledness = MODE_ZERO,
                    QKeySequence const& short_cut = tr(""),
                    QIcon const& icon = QIcon ());

    //-------------------------------------------------------------------------
    void addSeparator (ActionGroup group);

    Mode mode_;
    MainWindowModel* main_window_model_;
    //std::map<QString, QAction*> actions_map_;
    typedef std::list<QAction*> ActionList;
    typedef std::map<ActionGroup, ActionList > ActionGroupMap;
    typedef std::map<Mode, ActionList > ActionModeMap;

    ActionGroupMap action_group_map_;
    ActionModeMap disabled_actions_in_mode_map_;
};

} // namespace BioSig_

#endif // GUI_ACTION_MANAGER_H
