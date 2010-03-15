#include "gui_action_manager.h"
#include "main_window_model.h"

#include <QApplication>
#include <QStyle>

namespace BioSig_
{

//-----------------------------------------------------------------------------
GUIActionManager::GUIActionManager (MainWindowModel* main_window_model)
    : main_window_model_ (main_window_model),
      application_state_ (ApplicationContext::NO_FILE_OPEN),
      file_state_ (FileContext::UNCHANGED),
      tab_state_ (TabContext::NO_EVENTS_POSSIBLE)
{
    action_map_[ACTION_SEPARATOR] = new QAction (this);
    action_map_[ACTION_SEPARATOR]->setSeparator (true);

    initFileActions ();
    initEditActions ();
    initMouseModeActions();
    initShortcuts ();
    initGroups ();
    initDisabledStates ();
    setApplicationState (application_state_);
    setFileState (file_state_);
}

//-----------------------------------------------------------------------------
GUIActionManager::~GUIActionManager ()
{
    // QActions are deleted automatically because they are children of this
    // GUIActionManager object!
}

//-------------------------------------------------------------------------
void GUIActionManager::setApplicationState (ApplicationContext::State
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
void GUIActionManager::setFileState (FileContext::State file_state)
{
    ActionFileStateMap::iterator state_iter =
            file_state_action_map_.find (file_state_);
    if (state_iter != file_state_action_map_.end ())
        setActionsEnabled (state_iter->second, true);

    file_state_ = file_state;

    updateAllActionsDisabling ();
}

//-----------------------------------------------------------------------------
void GUIActionManager::setTabState (TabContext::State tab_state)
{
    ActionTabStateMap::iterator state_iter =
            tab_state_action_map_.find (tab_state_);
    if (state_iter != tab_state_action_map_.end ())
        setActionsEnabled (state_iter->second, true);

    tab_state_ = tab_state;

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

    createAction (ACTION_FILE_OPEN, tr("&Open..."), SLOT(fileOpenAction()),
               tr("Open a signal file"),
               style->standardIcon (QStyle::SP_DialogOpenButton));

    createAction (ACTION_FILE_SAVE, tr("&Save..."), SLOT(fileSaveAction()),
               tr("Save signal file"),
               style->standardIcon (QStyle::SP_DialogSaveButton));

    createAction (ACTION_FILE_SAVE_AS, tr("Save As..."), SLOT(fileSaveAsAction()),
               tr("Save the signal file under a new name"));

    createAction (ACTION_EXPORT_EVENTS, tr("Export Events..."),
                  SLOT(fileExportEventsAction()),
                   tr("Export events to file"));

    createAction (ACTION_IMPORT_EVENTS, tr("Import Events..."),
                  SLOT(fileImportEventsAction()),
                  tr("Import events from file"));

    createAction (ACTION_FILE_INFO, tr("&Info..."), SLOT(fileInfoAction()),
                  tr("Show the basic information of the signal file"),
                  style->standardIcon (QStyle::SP_MessageBoxInformation));

    createAction (ACTION_FILE_CLOSE, tr("&Close"), SLOT(fileCloseAction()),
                  tr("Close the opened signal file"),
                  style->standardIcon (QStyle::SP_DockWidgetCloseButton));

    createAction (ACTION_EXIT, tr("E&xit"), SLOT(fileExitAction()),
                  tr("Exit the application"),
                  style->standardIcon (QStyle::SP_DialogCloseButton));

}

//-----------------------------------------------------------------------------
void GUIActionManager::initEditActions ()
{
    QStyle* style = QApplication::style ();

    createAction (ACTION_UNDO, tr("Undo"), SLOT(undoAction()),
                  tr("Undo last editing command"),
                  style->standardIcon (QStyle::SP_ArrowLeft));

    createAction (ACTION_REDO, tr("Redo"), SLOT(redoAction()),
                  tr("Redo last undone command"),
                  style->standardIcon (QStyle::SP_ArrowRight));

    createAction (ACTION_TO_ALL_CHANNELS, tr("To all channels"), SLOT(editToAllChannelsAction()),
                  tr("Put the selected event to all channels"),
                  QIcon (":/images/to_all_channels_22x22.png"));

    createAction (ACTION_COPY_TO_CHANNELS, tr("Co&py to Channels..."), SLOT(editCopyToChannelsAction()),
                  tr("Copy the selected event to other channels"),
                  QIcon (":/images/copy_to_channels_22x22.png"));

    createAction (ACTION_DELETE, tr("&Delete"), SLOT(editDeleteAction()),
                  tr("Delete the selected event"),
                  QIcon (":/images/delete_22x22.png"));

    createAction (ACTION_CHANGE_CHANNEL, tr("Change Cha&nnel..."), SLOT(editChangeChannelAction()),
                  tr("Change the channel of the selected event"),
                  QIcon (":/images/change_channel_22x22.png"));

    createAction (ACTION_CHANGE_TYPE, tr("Change &Type..."), SLOT(editChangeTypeAction()),
                  tr("Change the type of the selected event"),
                  QIcon (":/images/change_type_22x22.png"));

    createAction (ACTION_SHOW_EVENT_TABLE, tr("&Event Table..."),
                  SLOT(editEventTableAction()), tr("Edit the events in a Table"));
}

//-----------------------------------------------------------------------------
void GUIActionManager::initMouseModeActions ()
{
    QActionGroup* mouse_mode_action_group = new QActionGroup (this);
    mouse_mode_action_group->setExclusive(true);

    QAction* action = createAction (ACTION_MODE_NEW, tr("&New Event"),
                                    SLOT (mouseModeNewAction()),
                                    tr("Create new events"),
                                    QIcon (":/images/new_22x22.png"));
    action->setCheckable (true);
    mouse_mode_action_group->addAction(action);

    action = createAction (ACTION_MODE_POINTER, tr("&Edit Events"),
                           SLOT (mouseModePointerAction()),
                           tr("Edit existing events"),
                           QIcon (":/images/pointer_22x22.png"));
    action->setCheckable (true);
    mouse_mode_action_group->addAction(action);

    action = createAction (ACTION_MODE_HAND, tr("&Scroll"),
                           SLOT (mouseModeHandAction()),
                           tr("Scroll area"),
                           QIcon (":/images/hand_22x22.png"));
    action->setCheckable (true);
    mouse_mode_action_group->addAction(action);

    action = createAction (ACTION_MODE_SHIFT, tr("&Shift Signal"),
                           SLOT (mouseModeShiftSignalAction()),
                           tr("Shift one channel in y-direction"),
                           QIcon (":/images/shift_signal_22x22.png"));
    action->setCheckable (true);
    mouse_mode_action_group->addAction(action);
}



//-----------------------------------------------------------------------------
void GUIActionManager::initShortcuts ()
{
    setShortCut (ACTION_FILE_OPEN, QKeySequence::Open);
    setShortCut (ACTION_FILE_SAVE, QKeySequence::Save);
    setShortCut (ACTION_FILE_SAVE, QKeySequence::SaveAs);
    setShortCut (ACTION_UNDO, QKeySequence::Undo);
    setShortCut (ACTION_REDO, QKeySequence::Redo);
    setShortCut (ACTION_DELETE, QKeySequence::Delete);

    setShortCut (ACTION_MODE_NEW, QString("Ctrl+1"));
    setShortCut (ACTION_MODE_POINTER, QString("Ctrl+2"));
    setShortCut (ACTION_MODE_HAND, QString("Ctrl+3"));
    setShortCut (ACTION_MODE_SHIFT, QString("Ctrl+4"));
}

//-----------------------------------------------------------------------------
void GUIActionManager::initGroups ()
{
    // FILE_MENU_ACTIONS
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_FILE_OPEN);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_FILE_SAVE);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_FILE_SAVE_AS);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_IMPORT_EVENTS);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_EXPORT_EVENTS);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_FILE_INFO);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_FILE_CLOSE);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_SEPARATOR);
    action_group_map_[FILE_MENU_ACTIONS].push_back (ACTION_EXIT);


    // FILE_TOOLBAR_ACTIONS
    action_group_map_[FILE_TOOLBAR_ACTIONS].push_back (ACTION_FILE_OPEN);
    action_group_map_[FILE_TOOLBAR_ACTIONS].push_back (ACTION_FILE_SAVE);
    action_group_map_[FILE_TOOLBAR_ACTIONS].push_back (ACTION_FILE_SAVE_AS);
    action_group_map_[FILE_TOOLBAR_ACTIONS].push_back (ACTION_FILE_INFO);
    action_group_map_[FILE_TOOLBAR_ACTIONS].push_back (ACTION_FILE_CLOSE);

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
    action_group_map_[EDIT_MENU_ACTIONS].push_back (ACTION_SHOW_EVENT_TABLE);

    // EDIT_TOOLBAR_ACTIONS
    action_group_map_[EDIT_TOOLBAR_ACTIONS].push_back (ACTION_UNDO);
    action_group_map_[EDIT_TOOLBAR_ACTIONS].push_back (ACTION_REDO);
    action_group_map_[EDIT_TOOLBAR_ACTIONS].push_back (ACTION_TO_ALL_CHANNELS);
    action_group_map_[EDIT_TOOLBAR_ACTIONS].push_back (ACTION_COPY_TO_CHANNELS);
    action_group_map_[EDIT_TOOLBAR_ACTIONS].push_back (ACTION_DELETE);
    action_group_map_[EDIT_TOOLBAR_ACTIONS].push_back (ACTION_CHANGE_CHANNEL);
    action_group_map_[EDIT_TOOLBAR_ACTIONS].push_back (ACTION_CHANGE_TYPE);

    // EVENT_CONTEXT_ACTIONS


    // MODE ACTIONS
    action_group_map_[MODE_ACTIONS].push_back (ACTION_MODE_NEW);
    action_group_map_[MODE_ACTIONS].push_back (ACTION_MODE_POINTER);
    action_group_map_[MODE_ACTIONS].push_back (ACTION_MODE_HAND);
    action_group_map_[MODE_ACTIONS].push_back (ACTION_MODE_SHIFT);
}

//-----------------------------------------------------------------------------
void GUIActionManager::initDisabledStates ()
{
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_FILE_SAVE);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_FILE_SAVE_AS);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_IMPORT_EVENTS);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_EXPORT_EVENTS);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_FILE_INFO);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_FILE_CLOSE);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_UNDO);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_REDO);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_TO_ALL_CHANNELS);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_COPY_TO_CHANNELS);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_DELETE);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_CHANGE_CHANNEL);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_CHANGE_TYPE);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_SHOW_EVENT_TABLE);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_MODE_HAND);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_MODE_POINTER);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_MODE_NEW);
    app_state_action_map_[ApplicationContext::NO_FILE_OPEN].push_back (ACTION_MODE_SHIFT);

    file_state_action_map_[FileContext::UNCHANGED].push_back (ACTION_UNDO);

    tab_state_action_map_[TabContext::NO_EVENTS_POSSIBLE].push_back (ACTION_TO_ALL_CHANNELS);
    tab_state_action_map_[TabContext::NO_EVENT_SELECTED].push_back (ACTION_TO_ALL_CHANNELS);
    tab_state_action_map_[TabContext::EVENT_SELECTED_ALL_CHANNELS].push_back (ACTION_TO_ALL_CHANNELS);
    tab_state_action_map_[TabContext::NO_EVENTS_POSSIBLE].push_back (ACTION_COPY_TO_CHANNELS);
    tab_state_action_map_[TabContext::NO_EVENT_SELECTED].push_back (ACTION_COPY_TO_CHANNELS);
    tab_state_action_map_[TabContext::NO_EVENTS_POSSIBLE].push_back (ACTION_DELETE);
    tab_state_action_map_[TabContext::NO_EVENT_SELECTED].push_back (ACTION_DELETE);
    tab_state_action_map_[TabContext::NO_EVENTS_POSSIBLE].push_back (ACTION_CHANGE_CHANNEL);
    tab_state_action_map_[TabContext::NO_EVENT_SELECTED].push_back (ACTION_CHANGE_CHANNEL);
    tab_state_action_map_[TabContext::NO_EVENTS_POSSIBLE].push_back (ACTION_CHANGE_TYPE);
    tab_state_action_map_[TabContext::NO_EVENT_SELECTED].push_back (ACTION_CHANGE_TYPE);
    tab_state_action_map_[TabContext::NO_EVENTS_POSSIBLE].push_back (ACTION_MODE_NEW);
    tab_state_action_map_[TabContext::NO_EVENTS_POSSIBLE].push_back (ACTION_MODE_POINTER);
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
    ActionTabStateMap::iterator tab_state_iter =
            tab_state_action_map_.find (tab_state_);
    if (tab_state_iter != tab_state_action_map_.end ())
        setActionsEnabled (tab_state_iter->second, false);

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
