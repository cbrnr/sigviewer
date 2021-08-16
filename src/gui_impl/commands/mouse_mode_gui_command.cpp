// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "mouse_mode_gui_command.h"
#include "gui/signal_visualisation_model.h"

namespace sigviewer
{


//-----------------------------------------------------------------------------
QString const MouseModeGuiCommand::NEW_MODE_TEXT_()
{
    static QString value = tr("New Event");

    return value;
}

QString const MouseModeGuiCommand::POINTER_MODE_TEXT_()
{
    static QString value = tr("Edit Event");

    return value;
}

QString const MouseModeGuiCommand::HAND_MODE_TEXT_()
{
    static QString value = tr("Scroll");

    return value;
}

QString const MouseModeGuiCommand::VIEW_OPTIONS_TEXT_()
{
    static QString value = tr("View Options");

    return value;
}

QString const MouseModeGuiCommand::INFO_MODE_TEXT_()
{
    static QString value = tr("Info");

    return value;
} 

QStringList const MouseModeGuiCommand::MODE_TEXTS_()
{
    static QStringList result = {
        MouseModeGuiCommand::NEW_MODE_TEXT_(),
        MouseModeGuiCommand::POINTER_MODE_TEXT_(),
        MouseModeGuiCommand::HAND_MODE_TEXT_(),
        MouseModeGuiCommand::VIEW_OPTIONS_TEXT_(),
        //<< MouseModeGuiCommand::INFO_MODE_TEXT_(),
    };

    return result;
}

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator MouseModeGuiCommand::registrator_ ("Mouse Modes",
                                                                  QSharedPointer<MouseModeGuiCommand> (new MouseModeGuiCommand));


//-----------------------------------------------------------------------------
MouseModeGuiCommand::MouseModeGuiCommand ()
    : GuiActionCommand (MODE_TEXTS_())
{
    action_to_mode_map_[NEW_MODE_TEXT_()] = MODE_NEW;
    action_to_mode_map_[POINTER_MODE_TEXT_()] = MODE_POINTER;
    action_to_mode_map_[HAND_MODE_TEXT_()] = MODE_HAND;
    action_to_mode_map_[VIEW_OPTIONS_TEXT_()] = MODE_VIEW_OPTIONS;
    //action_to_mode_map_[INFO_MODE_TEXT_()] = MODE_INFO;
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

    getQAction (NEW_MODE_TEXT_())->setIcon (QIcon(":/images/ic_add_box_black_24dp.png"));
    getQAction (POINTER_MODE_TEXT_())->setIcon (QIcon(":/images/ic_create_black_24dp.png"));
    getQAction (HAND_MODE_TEXT_())->setIcon (QIcon(":/images/ic_pan_tool_black_24dp.png"));
    getQAction (VIEW_OPTIONS_TEXT_())->setIcon (QIcon(":/images/ic_build_black_24dp.png"));

    getQAction (NEW_MODE_TEXT_())->setShortcut (QString("Ctrl+1"));
    getQAction (POINTER_MODE_TEXT_())->setShortcut (QString("Ctrl+2"));
    getQAction (HAND_MODE_TEXT_())->setShortcut (QString("Ctrl+3"));
    getQAction (VIEW_OPTIONS_TEXT_())->setShortcut (QString("Ctrl+4"));
    //getQAction (INFO_MODE_TEXT_())->setShortcut (QString("Ctrl+5"));

//    getQAction (NEW_MODE_TEXT_())->setToolTip(getQAction (NEW_MODE_TEXT_())->toolTip().append(tr(" (Ctrl+1)")));
//    getQAction (POINTER_MODE_TEXT_())->setToolTip(getQAction (POINTER_MODE_TEXT_())->toolTip().append(tr(" (Ctrl+2)")));
//    getQAction (HAND_MODE_TEXT_())->setToolTip(getQAction (HAND_MODE_TEXT_())->toolTip().append(tr(" (Ctrl+3)")));
//    getQAction (VIEW_OPTIONS_TEXT_())->setToolTip(getQAction (VIEW_OPTIONS_TEXT_())->toolTip().append(tr(" (Ctrl+4)")));

    QList<QAction*> actions = getQActions ();
    foreach (QAction* action, actions)
    {
        mouse_mode_action_group->addAction (action);
        action->setCheckable (true);
    }

    getQAction (HAND_MODE_TEXT_())->setChecked (true);
}

//-----------------------------------------------------------------------------
void MouseModeGuiCommand::trigger (QString const& action_name)
{
    currentVisModel()->setMode (action_to_mode_map_[action_name]);
}

//-------------------------------------------------------------------------
void MouseModeGuiCommand::evaluateEnabledness ()
{
    if (disableIfNoFileIsOpened (MODE_TEXTS_()))
        return;
    if (disableIfNoSignalIsVisualised(MODE_TEXTS_()))
        return;

    if (!currentVisModel().isNull())
    {
        getQAction(NEW_MODE_TEXT_())->setDisabled(currentVisModel()->getEventManager().isNull());
        getQAction(POINTER_MODE_TEXT_())->setDisabled(currentVisModel()->getEventManager().isNull());
    }
}


//-----------------------------------------------------------------------------
void MouseModeGuiCommand::applicationStateChanged ()
{
    if (getApplicationState () == APP_STATE_FILE_OPEN)
        getQAction (HAND_MODE_TEXT_())->setChecked (true);
}

}
