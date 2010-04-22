#include "gui_action_manager.h"
#include "application_context.h"
#include "main_window_model_impl.h"
#include "gui/gui_action_factory.h"

#include <QApplication>
#include <QStyle>

namespace BioSig_
{

//-----------------------------------------------------------------------------
GUIActionManager::GUIActionManager ()
    : main_window_model_ (0),
      application_state_ (APP_STATE_NO_FILE_OPEN),
      file_state_ (FILE_STATE_UNCHANGED),
      tab_edit_state_ (TAB_STATE_NO_REDO_NO_UNDO)
{
    action_map_[ACTION_SEPARATOR] = new QAction (this);
    action_map_[ACTION_SEPARATOR]->setSeparator (true);
}

//-----------------------------------------------------------------------------
GUIActionManager::~GUIActionManager ()
{
    // QActions are deleted automatically because they are children of this
    // GUIActionManager object!
}

//-----------------------------------------------------------------------------
void GUIActionManager::init (MainWindowModelImpl* main_window_model)
{
    connect (ApplicationContext::getInstance().data(), SIGNAL(stateChanged(ApplicationState)), SLOT(setApplicationState(ApplicationState)));
    main_window_model_ = main_window_model;
    initFileActions ();
    initEditActions ();
    initViewActions();
    initOptionsActions();
    initToolsActions();
    initShortcuts ();
    initGroups ();
    initDisabledStates ();
    setApplicationState (application_state_);
    setFileState (file_state_);
}

//-------------------------------------------------------------------------
void GUIActionManager::setApplicationState (ApplicationState
                                            application_state)
{
    ActionAppStateMap::iterator state_iter =
            app_state_action_map_.find (application_state_);
    if (state_iter != app_state_action_map_.end ())
        setActionsEnabled (state_iter->second, true);

    application_state_ = application_state;

    updateAllActionsDisabling ();
}

//-------------------------------------------------------------------------
void GUIActionManager::setFileState (FileState file_state)
{
    ActionFileStateMap::iterator state_iter =
            file_state_action_map_.find (file_state_);
    if (state_iter != file_state_action_map_.end ())
        setActionsEnabled (state_iter->second, true);

    file_state_ = file_state;

    updateAllActionsDisabling ();
}

//-----------------------------------------------------------------------------
void GUIActionManager::setTabSelectionState(TabSelectionState tab_state)
{
    ActionTabSelectionStateMap::iterator state_iter =
            tab_selection_state_action_map_.find (tab_selection_state_);
    if (state_iter != tab_selection_state_action_map_.end ())
        setActionsEnabled (state_iter->second, true);

    tab_selection_state_ = tab_state;

    updateAllActionsDisabling ();
}

//-----------------------------------------------------------------------------
void GUIActionManager::setTabEditState(TabEditState tab_state)
{
    ActionTabEditStateMap::iterator state_iter =
            tab_edit_state_action_map_.find (tab_edit_state_);
    if (state_iter != tab_edit_state_action_map_.end ())
        setActionsEnabled (state_iter->second, true);

    tab_edit_state_ = tab_state;

    updateAllActionsDisabling ();
}

//-----------------------------------------------------------------------------
QMenu* GUIActionManager::getGroupAsMenu (ActionGroup group, QString const& title,
                                         QWidget* parent)
{
    QMenu* menu = new QMenu (title, parent);
    ActionGroupMap::iterator group_iter = action_group_map_.find (group);
    if (group_iter != action_group_map_.end())
    {
        for (ActionList::iterator action_iter = group_iter->second.begin();
             action_iter != group_iter->second.end ();
             ++action_iter)
        {
            ActionMap::iterator action = action_map_.find (*action_iter);
            if (action == action_map_.end())
                menu->addAction (tr("ACTION NOT INITIALIZED!!!!"));
            else
            {
                if (action->second->isSeparator ())
                    menu->addSeparator ();
                else
                    menu->addAction (action->second);
            }
        }
    }
    return menu;
}

//-----------------------------------------------------------------------------
QList<QAction*> GUIActionManager::getActionsOfGroup (ActionGroup group)
{
    QList<QAction*> list;
    ActionGroupMap::iterator group_iter = action_group_map_.find (group);
    if (group_iter != action_group_map_.end())
    {
        for (ActionList::iterator action_iter = group_iter->second.begin();
             action_iter != group_iter->second.end ();
             ++action_iter)
        {
            ActionMap::iterator action = action_map_.find (*action_iter);
            if (action == action_map_.end())
                list.append (new QAction (tr("ACTION NOT INITIALIZED!!!!"), this));
            else
            {
                if (action->second->isSeparator ())
                    list.append (createSeparator());
                else
                    list.append (action->second);
            }
        }
    }
    return list;
}


//-----------------------------------------------------------------------------
void GUIActionManager::initFileActions ()
{
    QStyle* style = QApplication::style ();

    createAction (ACTION_FILE_SAVE, tr("&Save..."), SLOT(fileSaveAction()),
                  tr("Save signal file"),
                  QIcon(":/images/icons/filesave.png"));

    createAction (ACTION_FILE_SAVE_AS, tr("Save As..."), SLOT(fileSaveAsAction()),
                  tr("Save the signal file under a new name"),
                  QIcon(":/images/icons/filesaveas.png"));

    createAction (ACTION_EXPORT_EVENTS, tr("Export Events..."),
                  SLOT(fileExportEventsAction()),
                   tr("Export events to file"));

    createAction (ACTION_IMPORT_EVENTS, tr("Import Events..."),
                  SLOT(fileImportEventsAction()),
                  tr("Import events from file"));

    createAction (ACTION_FILE_INFO, tr("&Info..."), SLOT(fileInfoAction()),
                  tr("Show the basic information of the signal file"),
                  style->standardIcon (QStyle::SP_MessageBoxInformation));

    createAction (ACTION_EXIT, tr("E&xit"), SLOT(fileExitAction()),
                  tr("Exit the application"),
                  QIcon(":/images/icons/exit.png"));

}

//-----------------------------------------------------------------------------
void GUIActionManager::initEditActions ()
{
    createAction (ACTION_UNDO, tr("Undo"), SLOT(undoAction()),
                  tr("Undo last editing command"),
                  QIcon(":/images/icons/undo.png"));

    createAction (ACTION_REDO, tr("Redo"), SLOT(redoAction()),
                  tr("Redo last undone command"),
                  QIcon(":/images/icons/redo.png"));

    createAction (ACTION_TO_ALL_CHANNELS, tr("To all channels"), SLOT(editToAllChannelsAction()),
                  tr("Put the selected event to all channels"),
                  QIcon (":/images/to_all_channels_22x22.png"));

    createAction (ACTION_COPY_TO_CHANNELS, tr("Co&py to Channels..."), SLOT(editCopyToChannelsAction()),
                  tr("Copy the selected event to other channels"),
                  QIcon (":/images/copy_to_channels_22x22.png"));

    createAction (ACTION_DELETE, tr("&Delete"), SLOT(editDeleteAction()),
                  tr("Delete the selected event"),
                  QIcon (":/images/icons/editdelete.png"));

    createAction (ACTION_CHANGE_CHANNEL, tr("Change Cha&nnel..."), SLOT(editChangeChannelAction()),
                  tr("Change the channel of the selected event"),
                  QIcon (":/images/change_channel_22x22.png"));

    createAction (ACTION_CHANGE_TYPE, tr("Change &Type..."), SLOT(editChangeTypeAction()),
                  tr("Change the type of the selected event"),
                  QIcon (":/images/change_type_22x22.png"));

    createAction (ACTION_INSERT_OVER, tr("&Insert Over"), SLOT(editInsertOverAction()),
                  tr("Insert an event at the exact same position as the selected event"),
                  QIcon (":/images/icons/add.png"));

    createAction (ACTION_SHOW_EVENT_TABLE, tr("&Event Table..."),
                  SLOT(editEventTableAction()), tr("Edit the events in a Table"));
}

//-----------------------------------------------------------------------------
void GUIActionManager::initViewActions ()
{
    createAction (ACTION_VIEW_EVENTS, tr("&Events..."),
                  SLOT(optionsShowEventsAction()),
                  tr("Select the events that should be shown"),
                  QIcon(":/images/events_22x22.png"));

    createAction (ACTION_VIEW_CHANNELS, tr("Channe&ls..."),
                  SLOT(optionsChannelsAction()),
                  tr("Select the channels that should be shown"),
                  QIcon(":/images/channels_22x22.png"));

    createAction (ACTION_VIEW_ZOOM_IN, tr("Zoom &In"),
                  SLOT(viewZoomInAction()),
                  tr("Zoom in all channels"),
                  QIcon(":/images/zoom_in_22x22.png"));

    createAction (ACTION_VIEW_ZOOM_OUT, tr("Zoom &Out"),
                  SLOT(viewZoomOutAction()),
                  tr("Zoom out all channels"),
                  QIcon(":/images/zoom_out_22x22.png"));

    createAction (ACTION_VIEW_AUTO_SCALE, tr("&Auto Scale"),
                  SLOT(viewAutoScaleAction()),
                  tr("Autoscale all channels"),
                  QIcon(":/images/auto_scale_22x22.png"));

    createAction (ACTION_VIEW_GOTO, tr("&Go To..."),
                  SLOT(viewGoToAction()),
                  tr("Go to a specified point of the signal"),
                  QIcon(":/images/icons/goto.png"));

    createAction (ACTION_VIEW_GOTO_NEXT_EVENT, tr("Select &Next Event"),
                  SLOT(viewShowAndSelectNextEventAction()),
                  tr("Jumps to the next specified event and selects it"),
                  QIcon(":/images/icons/next.png"));

    createAction (ACTION_VIEW_GOTO_PREVIOUS_EVENT, tr("Select &Previous Event"),
                  SLOT(viewShowAndSelectPreviousEventAction()),
                  tr("Jumps to the previous specified event and selects it"),
                  QIcon(":/images/icons/previous.png"));

    createAction (ACTION_VIEW_HIDE_EVENTS_OF_OTHER_TYPE,
                  tr("Hide Events of other Type"),
                  SLOT(viewShowEventsOfSelectedTypeAction()),
                  tr("Only shows events which are of the same type as the selected one"));
}

//-------------------------------------------------------------------------
void GUIActionManager::initOptionsActions ()
{
    createAction (ACTION_OPTIONS_CHANGE_EVENT_CREATION_TYPE,
                  tr("Set Event Creation Type"),
                  SLOT(optionsChangeCreationType()),
                  tr("Change the type of newly created or inserted events"));

    createAction (ACTION_SHOW_SETTINGS,
                  tr("Settings"),
                  SLOT(optionsShowSettingsAction()),
                  tr("Shows the settings dialog/"),
                  QIcon(":/images/icons/configure.png"));
}

//-------------------------------------------------------------------------
void GUIActionManager::initToolsActions ()
{
    createAction (ACTION_CALCULATE_MEANS,
                  tr("Calculate Mean (alpha version)"),
                  SLOT(calculateMeanAction()),
                  tr("Calculates mean of selected event type"));
}

//-----------------------------------------------------------------------------
void GUIActionManager::initShortcuts ()
{
    setShortCut (ACTION_FILE_SAVE, QKeySequence::Save);
    setShortCut (ACTION_FILE_SAVE_AS, QKeySequence::SaveAs);
    setShortCut (ACTION_UNDO, QKeySequence::Undo);
    setShortCut (ACTION_REDO, QKeySequence::Redo);
    setShortCut (ACTION_DELETE, QKeySequence::Delete);
    setShortCut (ACTION_INSERT_OVER, QKeySequence("Ctrl+I"));

    setShortCut (ACTION_VIEW_ZOOM_IN, QKeySequence::ZoomIn);
    setShortCut (ACTION_VIEW_ZOOM_OUT, QKeySequence::ZoomOut);
    setShortCut (ACTION_VIEW_GOTO, QKeySequence("Ctrl+G"));
    setShortCut (ACTION_VIEW_GOTO_NEXT_EVENT, QKeySequence("Ctrl+Right"));
    setShortCut (ACTION_VIEW_GOTO_PREVIOUS_EVENT, QKeySequence("Ctrl+Left"));

}

//-----------------------------------------------------------------------------
void GUIActionManager::initGroups ()
{
    // FILE_MENU_ACTIONS
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_FILE_SAVE);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_FILE_SAVE_AS);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_IMPORT_EVENTS);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_EXPORT_EVENTS);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_FILE_INFO);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_EXIT);


    // FILE_TOOLBAR_ACTIONS
    action_group_map_[FILE_TOOLBAR_ACTIONS].push_back (ACTION_FILE_SAVE);
    action_group_map_[FILE_TOOLBAR_ACTIONS].push_back (ACTION_FILE_SAVE_AS);
    action_group_map_[FILE_TOOLBAR_ACTIONS].push_back (ACTION_FILE_INFO);
    action_group_map_[FILE_TOOLBAR_ACTIONS].push_back (ACTION_UNDO);
    action_group_map_[FILE_TOOLBAR_ACTIONS].push_back (ACTION_REDO);

    // EDIT_MENU_ACTIONS
    action_group_map_[EDIT_MENU_ACTIONS].push_back (ACTION_UNDO);
    action_group_map_[EDIT_MENU_ACTIONS].push_back (ACTION_REDO);
    action_group_map_[EDIT_MENU_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[EDIT_MENU_ACTIONS].push_back (ACTION_TO_ALL_CHANNELS);
    action_group_map_[EDIT_MENU_ACTIONS].push_back (ACTION_COPY_TO_CHANNELS);
    action_group_map_[EDIT_MENU_ACTIONS].push_back (ACTION_DELETE);
    action_group_map_[EDIT_MENU_ACTIONS].push_back (ACTION_CHANGE_CHANNEL);
    action_group_map_[EDIT_MENU_ACTIONS].push_back (ACTION_CHANGE_TYPE);
    action_group_map_[EDIT_MENU_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[EDIT_MENU_ACTIONS].push_back (ACTION_INSERT_OVER);
    action_group_map_[EDIT_MENU_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[EDIT_MENU_ACTIONS].push_back (ACTION_SHOW_EVENT_TABLE);

    // EDIT_TOOLBAR_ACTIONS
    action_group_map_[EDIT_TOOLBAR_ACTIONS].push_back (ACTION_TO_ALL_CHANNELS);
    action_group_map_[EDIT_TOOLBAR_ACTIONS].push_back (ACTION_COPY_TO_CHANNELS);
    action_group_map_[EDIT_TOOLBAR_ACTIONS].push_back (ACTION_DELETE);
    action_group_map_[EDIT_TOOLBAR_ACTIONS].push_back (ACTION_CHANGE_CHANNEL);
    action_group_map_[EDIT_TOOLBAR_ACTIONS].push_back (ACTION_CHANGE_TYPE);

    // EVENT_CONTEXT_ACTIONS
    action_group_map_[EVENT_CONTEXT_ACTIONS].push_back (ACTION_TO_ALL_CHANNELS);
    action_group_map_[EVENT_CONTEXT_ACTIONS].push_back (ACTION_COPY_TO_CHANNELS);
    action_group_map_[EVENT_CONTEXT_ACTIONS].push_back (ACTION_DELETE);
    action_group_map_[EVENT_CONTEXT_ACTIONS].push_back (ACTION_CHANGE_CHANNEL);
    action_group_map_[EVENT_CONTEXT_ACTIONS].push_back (ACTION_CHANGE_TYPE);
    action_group_map_[EVENT_CONTEXT_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[EVENT_CONTEXT_ACTIONS].push_back (ACTION_INSERT_OVER);
    action_group_map_[EVENT_CONTEXT_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[EVENT_CONTEXT_ACTIONS].push_back (ACTION_VIEW_GOTO_NEXT_EVENT);
    action_group_map_[EVENT_CONTEXT_ACTIONS].push_back (ACTION_VIEW_GOTO_PREVIOUS_EVENT);
    action_group_map_[EVENT_CONTEXT_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[EVENT_CONTEXT_ACTIONS].push_back (ACTION_VIEW_HIDE_EVENTS_OF_OTHER_TYPE);

    // VIEW_MENU_ACTIONS
    action_group_map_[VIEW_MENU_ACTIONS].push_back (ACTION_VIEW_CHANNELS);
    action_group_map_[VIEW_MENU_ACTIONS].push_back (ACTION_VIEW_EVENTS);
    action_group_map_[VIEW_MENU_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[VIEW_MENU_ACTIONS].push_back (ACTION_VIEW_ZOOM_IN);
    action_group_map_[VIEW_MENU_ACTIONS].push_back (ACTION_VIEW_ZOOM_OUT);
    action_group_map_[VIEW_MENU_ACTIONS].push_back (ACTION_VIEW_AUTO_SCALE);
    action_group_map_[VIEW_MENU_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[VIEW_MENU_ACTIONS].push_back (ACTION_VIEW_GOTO);
    action_group_map_[VIEW_MENU_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[VIEW_MENU_ACTIONS].push_back (ACTION_VIEW_GOTO_NEXT_EVENT);
    action_group_map_[VIEW_MENU_ACTIONS].push_back (ACTION_VIEW_GOTO_PREVIOUS_EVENT);
    action_group_map_[VIEW_MENU_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[VIEW_MENU_ACTIONS].push_back (ACTION_VIEW_HIDE_EVENTS_OF_OTHER_TYPE);


    // VIEW_TOOLBAR_ACTIONS
    action_group_map_[VIEW_TOOLBAR_ACTIONS].push_back (ACTION_VIEW_CHANNELS);
    action_group_map_[VIEW_TOOLBAR_ACTIONS].push_back (ACTION_VIEW_EVENTS);
    action_group_map_[VIEW_TOOLBAR_ACTIONS].push_back (ACTION_VIEW_ZOOM_IN);
    action_group_map_[VIEW_TOOLBAR_ACTIONS].push_back (ACTION_VIEW_ZOOM_OUT);
    action_group_map_[VIEW_TOOLBAR_ACTIONS].push_back (ACTION_VIEW_AUTO_SCALE);

    // OPTIONS_MENU_ACTIONS
    action_group_map_[OPTIONS_MENU_ACTIONS].push_back (ACTION_SHOW_SETTINGS);
    action_group_map_[OPTIONS_MENU_ACTIONS].push_back (ACTION_OPTIONS_CHANGE_EVENT_CREATION_TYPE);

    // TOOLS_MENU_ACTIONS
    action_group_map_[TOOLS_MENU_ACTIONS].push_back (ACTION_CALCULATE_MEANS);
}

//-----------------------------------------------------------------------------
void GUIActionManager::initDisabledStates ()
{
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_FILE_SAVE);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_FILE_SAVE_AS);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_IMPORT_EVENTS);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_EXPORT_EVENTS);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_FILE_INFO);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_UNDO);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_REDO);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_TO_ALL_CHANNELS);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_COPY_TO_CHANNELS);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_DELETE);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_CHANGE_CHANNEL);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_CHANGE_TYPE);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_SHOW_EVENT_TABLE);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_VIEW_ZOOM_IN);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_VIEW_ZOOM_OUT);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_VIEW_AUTO_SCALE);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_VIEW_GOTO);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_VIEW_GOTO_NEXT_EVENT);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_VIEW_GOTO_PREVIOUS_EVENT);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_VIEW_HIDE_EVENTS_OF_OTHER_TYPE);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_VIEW_CHANNELS);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_VIEW_EVENTS);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_INSERT_OVER);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_OPTIONS_CHANGE_EVENT_CREATION_TYPE);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_SHOW_SETTINGS);
    app_state_action_map_[APP_STATE_NO_FILE_OPEN].push_back (ACTION_CALCULATE_MEANS);

    file_state_action_map_[FILE_STATE_UNCHANGED].push_back (ACTION_FILE_SAVE);

    tab_edit_state_action_map_[TAB_STATE_NO_REDO].push_back (ACTION_REDO);
    tab_edit_state_action_map_[TAB_STATE_NO_UNDO].push_back (ACTION_UNDO);
    tab_edit_state_action_map_[TAB_STATE_NO_REDO_NO_UNDO].push_back (ACTION_REDO);
    tab_edit_state_action_map_[TAB_STATE_NO_REDO_NO_UNDO].push_back (ACTION_UNDO);

    tab_selection_state_action_map_[TAB_STATE_EVENT_SELECTED_ALL_CHANNELS].push_back (ACTION_TO_ALL_CHANNELS);
    tab_selection_state_action_map_[TAB_STATE_NO_EVENT_SELECTED].push_back (ACTION_TO_ALL_CHANNELS);
    tab_selection_state_action_map_[TAB_STATE_NO_EVENT_SELECTED].push_back (ACTION_COPY_TO_CHANNELS);
    tab_selection_state_action_map_[TAB_STATE_NO_EVENT_SELECTED].push_back (ACTION_DELETE);
    tab_selection_state_action_map_[TAB_STATE_NO_EVENT_SELECTED].push_back (ACTION_CHANGE_CHANNEL);
    tab_selection_state_action_map_[TAB_STATE_NO_EVENT_SELECTED].push_back (ACTION_CHANGE_TYPE);
    tab_selection_state_action_map_[TAB_STATE_NO_EVENT_SELECTED].push_back (ACTION_VIEW_GOTO_NEXT_EVENT);
    tab_selection_state_action_map_[TAB_STATE_NO_EVENT_SELECTED].push_back (ACTION_VIEW_GOTO_PREVIOUS_EVENT);
    tab_selection_state_action_map_[TAB_STATE_NO_EVENT_SELECTED].push_back (ACTION_VIEW_HIDE_EVENTS_OF_OTHER_TYPE);
    tab_selection_state_action_map_[TAB_STATE_NO_EVENT_SELECTED].push_back (ACTION_INSERT_OVER);
}


//-----------------------------------------------------------------------------
QAction* GUIActionManager::createAction (GUIAction action_id,
                                         QString const& text,
                                         char const * const slot,
                                         QString const& status_tip,
                                         QIcon const& icon)
{
    QAction* action = new QAction (text, this);
    if (!icon.isNull())
        action->setIcon (icon);
    if (!status_tip.isEmpty())
        action->setStatusTip (status_tip);
    main_window_model_->connect (action, SIGNAL(triggered()), slot);
    action_map_[action_id] = action;
    return action;
}

//-------------------------------------------------------------------------
QAction* GUIActionManager::createSeparator ()
{
    QAction* separator = new QAction (this);
    separator->setSeparator (true);
    return separator;
}


//-------------------------------------------------------------------------
void GUIActionManager::setShortCut (GUIAction const&  gui_action,
                                    QKeySequence const& key_sequence)
{
    ActionMap::iterator action_iter = action_map_.find (gui_action);
    if (action_iter != action_map_.end())
        action_map_[gui_action]->setShortcut (key_sequence);
}

//-------------------------------------------------------------------------
void GUIActionManager::updateAllActionsDisabling ()
{
    ActionTabSelectionStateMap::iterator tab_state_iter =
            tab_selection_state_action_map_.find (tab_selection_state_);
    if (tab_state_iter != tab_selection_state_action_map_.end ())
        setActionsEnabled (tab_state_iter->second, false);

    ActionTabEditStateMap::iterator tab_edit_state_iter =
            tab_edit_state_action_map_.find (tab_edit_state_);
    if (tab_edit_state_iter != tab_edit_state_action_map_.end ())
        setActionsEnabled (tab_edit_state_iter->second, false);

    ActionAppStateMap::iterator app_state_iter =
            app_state_action_map_.find (application_state_);
    if (app_state_iter != app_state_action_map_.end ())
        setActionsEnabled (app_state_iter->second, false);

    ActionFileStateMap::iterator file_state_iter =
            file_state_action_map_.find (file_state_);
    if (file_state_iter != file_state_action_map_.end ())
        setActionsEnabled (file_state_iter->second, false);

}


//-------------------------------------------------------------------------
void GUIActionManager::setActionsEnabled (ActionList& action_list, bool enabled)
{
    for (ActionList::iterator action_iter = action_list.begin();
         action_iter != action_list.end ();
         ++action_iter)
    {
        ActionMap::iterator action = action_map_.find (*action_iter);
        if (action != action_map_.end())
            action->second->setEnabled (enabled);
    }
}



} // namespace BioSig_
