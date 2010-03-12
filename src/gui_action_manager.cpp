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
      file_state_ (FileContext::UNCHANGED)
{
    QStyle* style = QApplication::style ();

    action_map_[ACTION_SEPARATOR] = new QAction (this);
    action_map_[ACTION_SEPARATOR]->setSeparator (true);

    // init file actions
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

    state_iter = app_state_action_map_.find (application_state_);
    if (state_iter != app_state_action_map_.end ())
        setActionsEnabled (state_iter->second, false);
}

//-------------------------------------------------------------------------
void GUIActionManager::setFileState (FileContext::State file_state)
{
    ActionFileStateMap::iterator state_iter =
            file_state_action_map_.find (file_state_);
    if (state_iter != file_state_action_map_.end ())
        setActionsEnabled (state_iter->second, true);

    file_state_ = file_state;

    state_iter = file_state_action_map_.find (file_state_);
    if (state_iter != file_state_action_map_.end ())
        setActionsEnabled (state_iter->second, false);
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
void GUIActionManager::initShortcuts ()
{
    setShortCut (ACTION_FILE_OPEN, QKeySequence::Open);
    setShortCut (ACTION_FILE_SAVE, QKeySequence::Save);
    setShortCut (ACTION_FILE_SAVE, QKeySequence::SaveAs);
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
}


//-----------------------------------------------------------------------------
void GUIActionManager::createAction (GUIAction action_id,
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
