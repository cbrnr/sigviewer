#include "gui_action_manager.h"
#include "main_window_model.h"

#include <QApplication>
#include <QStyle>

namespace BioSig_
{

//-----------------------------------------------------------------------------
GUIActionManager::GUIActionManager (MainWindowModel* main_window_model)
    : mode_ (MODE_ZERO),
      main_window_model_ (main_window_model)
{
    QStyle* style = QApplication::style ();

    // init file actions
    addAction (FILE_ACTIONS, tr("&Open..."), SLOT(fileOpenAction()),
               tr("Open a signal file"), MODE_ZERO, QKeySequence::Open,
               style->standardIcon (QStyle::SP_DialogOpenButton));

    addAction (FILE_ACTIONS, tr("&Save..."), SLOT(fileSaveAction()),
               tr("Save signal file"), MODE_NO_FILE, QKeySequence::Save,
               style->standardIcon (QStyle::SP_DialogSaveButton));

    addAction (FILE_ACTIONS, tr("Save As..."), SLOT(fileSaveAsAction()),
               tr("Save the signal file under a new name"), MODE_NO_FILE,
               QKeySequence::SaveAs);

    addSeparator (FILE_ACTIONS);

    addAction (FILE_ACTIONS, tr("Export Events..."),
               SLOT(fileExportEventsAction()),
               tr("Export events to file"), MODE_NO_FILE);

    addAction (FILE_ACTIONS, tr("Import Events..."),
               SLOT(fileImportEventsAction()),
               tr("Import events from file"), MODE_NO_FILE);

    addSeparator (FILE_ACTIONS);

    addAction (FILE_ACTIONS, tr("&Info..."), SLOT(fileInfoAction()),
               tr("Show the basic information of the signal file"),
               MODE_NO_FILE, tr(""),
               style->standardIcon (QStyle::SP_MessageBoxInformation));

    addSeparator (FILE_ACTIONS);

    addAction (FILE_ACTIONS, tr("&Close"), SLOT(fileCloseAction()),
               tr("Close the opened signal file"), MODE_NO_FILE, tr(""),
               style->standardIcon (QStyle::SP_DockWidgetCloseButton));

    addSeparator (FILE_ACTIONS);

    addAction (FILE_ACTIONS, tr("E&xit"), SLOT(fileExitAction()),
               tr("Exit the application"), MODE_ZERO, QKeySequence::Quit,
               style->standardIcon (QStyle::SP_DialogCloseButton));

}

//-----------------------------------------------------------------------------
GUIActionManager::~GUIActionManager ()
{
    // QActions are deleted automatically because they are children of this
    // GUIActionManager object!
}

//-----------------------------------------------------------------------------
void GUIActionManager::setMode (Mode mode)
{
    ActionModeMap::iterator mode_iter =
            disabled_actions_in_mode_map_.find (mode_);

    if (mode_iter != disabled_actions_in_mode_map_.end())
    {
        for (ActionList::iterator action_iter = mode_iter->second.begin();
             action_iter != mode_iter->second.end ();
             ++action_iter)
        {
            (*action_iter)->setEnabled (true);
        }
    }

    mode_ = mode;

    mode_iter = disabled_actions_in_mode_map_.find (mode_);
    if (mode_iter != disabled_actions_in_mode_map_.end())
    {
        for (ActionList::iterator action_iter = mode_iter->second.begin();
             action_iter != mode_iter->second.end ();
             ++action_iter)
        {
            (*action_iter)->setEnabled (false);
        }
    }

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
            menu->addAction (*action_iter);
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
            list.append (*action_iter);
        }
    }
    return list;
}


//-----------------------------------------------------------------------------
void GUIActionManager::addAction (ActionGroup group, QString const& text,
                                  char const * const slot,
                                  QString const& status_tip,
                                  Mode mode_of_disabledness,
                                  QKeySequence const& short_cut,
                                  QIcon const& icon)
{
    QAction* action = new QAction (text, this);
    if (!icon.isNull())
        action->setIcon (icon);
    if (!short_cut.isEmpty())
        action->setShortcut (short_cut);
    if (!status_tip.isEmpty())
        action->setStatusTip (status_tip);
    main_window_model_->connect (action, SIGNAL(triggered()), slot);
    action_group_map_[group].push_back (action);
    if (mode_of_disabledness != MODE_ZERO)
        disabled_actions_in_mode_map_[mode_of_disabledness].push_back (action);
}

//-----------------------------------------------------------------------------
void GUIActionManager::addSeparator (ActionGroup group)
{
    QAction* action = new QAction (this);
    action->setSeparator (true);
    action_group_map_[group].push_back (action);
}


} // namespace BioSig_
