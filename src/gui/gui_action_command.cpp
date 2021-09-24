// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "gui_action_command.h"
#include "gui_action_factory.h"
#include "application_context_impl.h"

#include <QDebug>
#include <QObject>

namespace sigviewer
{

class GuiActionCommandException : public Exception
{
public:
    explicit GuiActionCommandException (QString const& action_id,
                                        QString const& error) :
    Exception (QObject::tr("GuiActionCommand: \"%1\" failed: %2").arg(action_id).arg(error).toStdString())
    {}
};

//-----------------------------------------------------------------------------
GuiActionCommand::GuiActionCommand (QStringList const& action_ids)
    : tab_edit_state_ (TAB_STATE_NO_REDO_NO_UNDO)
{
    for (QStringList::const_iterator iter = action_ids.begin();
         iter != action_ids.end();
         ++iter)
    {
        action_map_[*iter] = new QAction (*iter, this);
        connectors_.push_back (new ActionConnector (this, *iter));
    }
}

//-----------------------------------------------------------------------------
void GuiActionCommand::initConnections()
{
	for (ActionConnector* con : connectors_)
	{
		QAction* act = action_map_[con->getName()];
		con->connect (act, SIGNAL(triggered()), SLOT(trigger()));
		connect (con, SIGNAL(triggered(QString const&)), SLOT(trigger(QString const&)));
	}
	QString firstActionId = connectors_.first()->getName();
	qDebug() << "GuiActionCommand::GuiActionCommand connecting to ApplicationContextImpl::getInstance() = " << ApplicationContextImpl::getInstance();
	if (!connect(ApplicationContextImpl::getInstance().data(), SIGNAL(stateChanged(ApplicationState)),
		SLOT(updateEnablednessToApplicationState(ApplicationState))))
		throw (GuiActionCommandException(firstActionId, tr("connect to signal stateChanged(ApplicationState)")));
	else
		qDebug() << "GuiActionCommand::GuiActionCommand connect to signal stateChanged(ApplicationState) = true";
	if (!connect(ApplicationContextImpl::getInstance().data(), SIGNAL(currentTabSelectionStateChanged(TabSelectionState)),
		SLOT(updateEnablednessToTabSelectionState(TabSelectionState))))
		throw (GuiActionCommandException(firstActionId, tr("connect to signal currentTabSelectionStateChanged(TabSelectionState)")));
	if (!connect(ApplicationContextImpl::getInstance().data(), SIGNAL(currentTabEditStateChanged(TabEditState)),
		SLOT(updateEnablednessToTabEditState(TabEditState))))
		throw (GuiActionCommandException(firstActionId, tr("connect to signal currentTabEditStateChanged(TabEditState)")));
	if (!connect(ApplicationContextImpl::getInstance().data(), SIGNAL(currentFileStateChanged(FileState)),
		SLOT(updateEnablednessToFileState(FileState))))
		throw (GuiActionCommandException(firstActionId, tr("connect to signal currentFileStateChanged(FileState)")));
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
        throw GuiActionCommandException (id, tr("not exists"));
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
    if (!action_map_.contains (action_id))
        throw (GuiActionCommandException (action_id, tr("resetActionTriggerSlot, action not exists")));
    QAction* action = action_map_[action_id];
    action->disconnect (SIGNAL(triggered()));
    if (!connect (action, SIGNAL(triggered()), slot))
        throw (GuiActionCommandException (action_id, tr("connect triggered to %1").arg(slot)));
}

//-----------------------------------------------------------------------------
void GuiActionCommand::setShortcut (QString const& action_id,
                                    QKeySequence const& key_sequence)
{
    if (!action_map_.contains (action_id))
        throw (GuiActionCommandException (action_id, tr("setting shortcut, action not exists")));
    action_map_[action_id]->setShortcut (key_sequence);
}

//-----------------------------------------------------------------------------
void GuiActionCommand::setIcon (QString const& action_id, QIcon const& icon)
{
    if (!action_map_.contains (action_id))
        throw (GuiActionCommandException (action_id, tr("setting icon, action not exists")));
    action_map_[action_id]->setIcon (icon);
}

//-------------------------------------------------------------------------
void GuiActionCommand::evaluateEnabledness ()
{

}

//-------------------------------------------------------------------------
QSharedPointer<EventView> GuiActionCommand::currentEventView ()
{
    QSharedPointer<EventView> event_view;

    QSharedPointer<MainWindowModel> main_window_model =
        ApplicationContextImpl::getInstance()->getMainWindowModel ();

    if (!main_window_model.isNull())
        event_view = main_window_model->getCurrentEventView ();

    return event_view;
}

//-------------------------------------------------------------------------
QSharedPointer<SignalVisualisationModel> GuiActionCommand::currentVisModel ()
{
    QSharedPointer<SignalVisualisationModel> model;

    QSharedPointer<MainWindowModel> main_window_model =
        ApplicationContextImpl::getInstance()->getMainWindowModel ();

    if (!main_window_model.isNull())
        model = main_window_model->getCurrentSignalVisualisationModel();

    return model;
}

//-------------------------------------------------------------------------
QSharedPointer<SignalViewSettings> GuiActionCommand::currentSignalViewSettings ()
{
    QSharedPointer<SignalViewSettings> settings;

    QSharedPointer<SignalVisualisationModel> vis_model = currentVisModel();
    if (!vis_model.isNull())
        settings = vis_model->getSignalViewSettings();

    return settings;
}

//-------------------------------------------------------------------------
QSharedPointer<FileContext> GuiActionCommand::currentFileContext ()
{
    return ApplicationContextImpl::getInstance()->getCurrentFileContext();
}

//-------------------------------------------------------------------------
QSharedPointer<ApplicationContext> GuiActionCommand::applicationContext ()
{
    return ApplicationContextImpl::getInstance();
}

//-------------------------------------------------------------------------
bool GuiActionCommand::disableIfNoSignalIsVisualised (QStringList const &actions)
{
    bool no_signal_is_visualised = currentVisModel().isNull();

    if (no_signal_is_visualised)
        foreach (QString action, actions)
            action_map_[action]->setDisabled (no_signal_is_visualised);

    return no_signal_is_visualised;
}

//-------------------------------------------------------------------------
bool GuiActionCommand::disableIfNoEventsPossible (QStringList const &actions)
{
    bool no_events_possible = false;
    if (!currentEventView().isNull())
    {
        if (currentEventView()->getEventManager().isNull())
            no_events_possible = true;
    }
    else
        no_events_possible = true;


    foreach (QString action, actions)
        action_map_[action]->setDisabled (no_events_possible);

    return no_events_possible;
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
bool GuiActionCommand::disableIfNoFileIsOpened (QStringList const &actions)
{
    bool no_file_open = (app_state_ == APP_STATE_NO_FILE_OPEN);
    foreach (QString action, actions)
        action_map_[action]->setDisabled (no_file_open);
    return no_file_open;
}

}
