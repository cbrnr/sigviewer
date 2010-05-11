#include "gui_action_command.h"
#include "gui_action_factory.h"
#include "../application_context.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
GuiActionCommand::GuiActionCommand (QStringList const& action_ids)
{
    for (QStringList::const_iterator iter = action_ids.begin();
         iter != action_ids.end();
         ++iter)
    {
        bool connect_ok = false;
        if (action_ids.contains("Zoom In Horizontal"))
            connect_ok = true; // dummy statement just to get debugger here
        action_map_[*iter] = new QAction (*iter, this);
        connectors_.push_back (new ActionConnector (this, *iter));
        connectors_.last ()->connect (action_map_[*iter], SIGNAL(triggered()), SLOT(trigger()));
        connect_ok = connect (connectors_.last (), SIGNAL(triggered(QString const&)), SLOT(trigger(QString const&)));
        connect_ok = action_map_[*iter]->connect (this, SIGNAL(qActionEnabledChanged(bool)), SLOT(setEnabled (bool)));
        connect_ok = connect (ApplicationContext::getInstance().data(), SIGNAL(stateChanged(ApplicationState)),
                          SLOT(updateEnablednessToApplicationState(ApplicationState)));
        connect_ok = connect (ApplicationContext::getInstance().data(), SIGNAL(currentTabSelectionStateChanged(TabSelectionState)),
                          SLOT(updateEnablednessToTabSelectionState (TabSelectionState)));
        connect_ok = connect (ApplicationContext::getInstance().data(), SIGNAL(currentTabEditStateChanged(TabEditState)),
                          SLOT(updateEnablednessToTabEditState (TabEditState)));
        connect_ok = connect (ApplicationContext::getInstance().data(), SIGNAL(currentFileStateChanged(FileState)),
                          SLOT(updateEnablednessToFileState (FileState)));
    }
}

//-----------------------------------------------------------------------------
QList<QAction*> GuiActionCommand::getQActions ()
{
    return action_map_.values ();
}

//-----------------------------------------------------------------------------
QList<QString> GuiActionCommand::getActionIDs () const
{
    return action_map_.keys ();
}


//-----------------------------------------------------------------------------
QAction* GuiActionCommand::getQAction (QString const& id)
{
    if (action_map_.contains (id))
        return action_map_[id];
    else
        return 0;
}

//-----------------------------------------------------------------------------
void GuiActionCommand::updateEnablednessToApplicationState (ApplicationState state)
{
    app_state_ = state;
    applicationStateChanged ();
    evaluateEnabledness ();
}

//-----------------------------------------------------------------------------
void GuiActionCommand::updateEnablednessToFileState (FileState state)
{
    file_state_ = state;
    evaluateEnabledness ();
}


//-----------------------------------------------------------------------------
void GuiActionCommand::updateEnablednessToTabSelectionState (TabSelectionState state)
{
    tab_selection_state_ = state;
    evaluateEnabledness ();
}

//-----------------------------------------------------------------------------
void GuiActionCommand::updateEnablednessToTabEditState (TabEditState state)
{
    tab_edit_state_ = state;
    evaluateEnabledness ();
}


//-----------------------------------------------------------------------------
void GuiActionCommand::resetActionTriggerSlot (QString const& action_id,
                                              const char* slot)
{
    assert (action_map_.contains (action_id));
    QAction* action = action_map_[action_id];
    action->disconnect (SIGNAL(triggered()));
    assert (connect (action, SIGNAL(triggered()), slot));
}

//-----------------------------------------------------------------------------
void GuiActionCommand::setShortcut (QString const& action_id,
                                    QKeySequence const& key_sequence)
{
    assert (action_map_.contains (action_id));
    action_map_[action_id]->setShortcut (key_sequence);
}

//-----------------------------------------------------------------------------
void GuiActionCommand::setIcon (QString const& action_id, QIcon const& icon)
{
    assert (action_map_.contains (action_id));
    action_map_[action_id]->setIcon (icon);
}

//-------------------------------------------------------------------------
QSharedPointer<SignalVisualisationModel> GuiActionCommand::currentVisModel ()
{
    QSharedPointer<SignalVisualisationModel> model;

    QSharedPointer<MainWindowModel> main_window_model =
        ApplicationContext::getInstance()->getMainWindowModel ();

    if (!main_window_model.isNull())
        model = main_window_model->getCurrentSignalVisualisationModel();

    return model;
}

//-------------------------------------------------------------------------
void GuiActionCommand::disableIfNoEventSelected (QStringList const &actions)
{
    bool no_event_selected = app_state_ == APP_STATE_NO_FILE_OPEN ||
                             tab_selection_state_ == TAB_STATE_NO_EVENT_SELECTED ||
                             tab_selection_state_ == NO_TAB_SELECTION_STATE;
    foreach (QString action, actions)
        action_map_[action]->setDisabled (no_event_selected);
}

//-------------------------------------------------------------------------
void GuiActionCommand::disableIfNoFileIsOpened (QStringList const &actions)
{
    bool no_file_open = (app_state_ == APP_STATE_NO_FILE_OPEN);
    foreach (QString action, actions)
        action_map_[action]->setDisabled (no_file_open);
}





}
