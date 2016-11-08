#include "mouse_mode_gui_command.h"
#include "gui/signal_visualisation_model.h"

namespace SigViewer_
{


//-----------------------------------------------------------------------------
QString const MouseModeGuiCommand::NEW_MODE_TEXT_ = "New Event";
QString const MouseModeGuiCommand::POINTER_MODE_TEXT_ = "Edit Event";
QString const MouseModeGuiCommand::HAND_MODE_TEXT_ = "Scroll";
QString const MouseModeGuiCommand::VIEW_OPTIONS_TEXT_ = "View Options";
QString const MouseModeGuiCommand::INFO_MODE_TEXT_ = "Info";
QStringList const MouseModeGuiCommand::MODE_TEXTS_ = QStringList() <<
                                                     MouseModeGuiCommand::NEW_MODE_TEXT_ <<
                                                     MouseModeGuiCommand::POINTER_MODE_TEXT_ <<
                                                     MouseModeGuiCommand::HAND_MODE_TEXT_ <<
                                                     MouseModeGuiCommand::VIEW_OPTIONS_TEXT_;
                                                     //<< MouseModeGuiCommand::INFO_MODE_TEXT_;

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator MouseModeGuiCommand::registrator_ ("Mouse Modes",
                                                                  QSharedPointer<MouseModeGuiCommand> (new MouseModeGuiCommand));


//-----------------------------------------------------------------------------
MouseModeGuiCommand::MouseModeGuiCommand ()
    : GuiActionCommand (MODE_TEXTS_)
{
    action_to_mode_map_[NEW_MODE_TEXT_] = MODE_NEW;
    action_to_mode_map_[POINTER_MODE_TEXT_] = MODE_POINTER;
    action_to_mode_map_[HAND_MODE_TEXT_] = MODE_HAND;
    action_to_mode_map_[VIEW_OPTIONS_TEXT_] = MODE_VIEW_OPTIONS;
    //action_to_mode_map_[INFO_MODE_TEXT_] = MODE_INFO;
}

//-----------------------------------------------------------------------------
MouseModeGuiCommand::~MouseModeGuiCommand ()
{
    // nothing to do here
}


//-----------------------------------------------------------------------------
void MouseModeGuiCommand::init ()
{
    QActionGroup* mouse_mode_action_group = new QActionGroup (this);
    mouse_mode_action_group->setExclusive(true);

    getQAction (NEW_MODE_TEXT_)->setIcon (QIcon(":/images/new_22x22.png"));
    getQAction (POINTER_MODE_TEXT_)->setIcon (QIcon(":/images/pointer_22x22.png"));
    getQAction (HAND_MODE_TEXT_)->setIcon (QIcon(":/images/hand_22x22.png"));
    getQAction (VIEW_OPTIONS_TEXT_)->setIcon (QIcon(":/images/icons/configure.png"));

    getQAction (NEW_MODE_TEXT_)->setShortcut (QString("Ctrl+1"));
    getQAction (POINTER_MODE_TEXT_)->setShortcut (QString("Ctrl+2"));
    getQAction (HAND_MODE_TEXT_)->setShortcut (QString("Ctrl+3"));
    getQAction (VIEW_OPTIONS_TEXT_)->setShortcut (QString("Ctrl+4"));
    //getQAction (INFO_MODE_TEXT_)->setShortcut (QString("Ctrl+5"));

    QList<QAction*> actions = getQActions ();
    foreach (QAction* action, actions)
    {
        mouse_mode_action_group->addAction (action);
        action->setCheckable (true);
    }

    getQAction (HAND_MODE_TEXT_)->setChecked (true);
}

//-----------------------------------------------------------------------------
void MouseModeGuiCommand::trigger (QString const& action_name)
{
    currentVisModel()->setMode (action_to_mode_map_[action_name]);
}

//-------------------------------------------------------------------------
void MouseModeGuiCommand::evaluateEnabledness ()
{
    if (disableIfNoFileIsOpened (MODE_TEXTS_))
        return;
    if (disableIfNoSignalIsVisualised(MODE_TEXTS_))
        return;

    if (!currentVisModel().isNull())
    {
        getQAction(NEW_MODE_TEXT_)->setDisabled(currentVisModel()->getEventManager().isNull());
        getQAction(POINTER_MODE_TEXT_)->setDisabled(currentVisModel()->getEventManager().isNull());
    }
}


//-----------------------------------------------------------------------------
void MouseModeGuiCommand::applicationStateChanged ()
{
    if (getApplicationState () == APP_STATE_FILE_OPEN)
        getQAction (HAND_MODE_TEXT_)->setChecked (true);
}


}
