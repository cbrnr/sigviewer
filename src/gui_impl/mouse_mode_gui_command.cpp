#include "mouse_mode_gui_command.h"
#include "../gui/signal_visualisation_model.h"
#include "../application_context.h"

namespace BioSig_
{


//-----------------------------------------------------------------------------
QString const MouseModeGuiCommand::NEW_MODE_TEXT_ = tr("New Event");
QString const MouseModeGuiCommand::POINTER_MODE_TEXT_ = tr("Edit Event");
QString const MouseModeGuiCommand::HAND_MODE_TEXT_ = tr("Scroll");
QString const MouseModeGuiCommand::SHIFT_MODE_TEXT_ = tr("Shift Signal");
QStringList const MouseModeGuiCommand::MODE_TEXTS_ = QStringList() <<
                                                     MouseModeGuiCommand::NEW_MODE_TEXT_ <<
                                                     MouseModeGuiCommand::POINTER_MODE_TEXT_ <<
                                                     MouseModeGuiCommand::HAND_MODE_TEXT_ <<
                                                     MouseModeGuiCommand::SHIFT_MODE_TEXT_;

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
    action_to_mode_map_[SHIFT_MODE_TEXT_] = MODE_SHIFT_SIGNAL;
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
    getQAction (SHIFT_MODE_TEXT_)->setIcon (QIcon(":/images/shift_signal_22x22.png"));

    getQAction (NEW_MODE_TEXT_)->setShortcut (QString("Ctrl+1"));
    getQAction (POINTER_MODE_TEXT_)->setShortcut (QString("Ctrl+2"));
    getQAction (HAND_MODE_TEXT_)->setShortcut (QString("Ctrl+3"));
    getQAction (SHIFT_MODE_TEXT_)->setShortcut (QString("Ctrl+4"));

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
    QSharedPointer<MainWindowModel> mw_model = ApplicationContext::getInstance()->getMainWindowModel ();
    QSharedPointer<SignalVisualisationModel> sv_model = mw_model->getCurrentSignalVisualisationModel ();
    sv_model->setMode (action_to_mode_map_[action_name]);
}

//-----------------------------------------------------------------------------
void MouseModeGuiCommand::applicationStateChanged (ApplicationState state)
{
    QList<QAction*> actions = getQActions ();

    if (state == APP_STATE_NO_FILE_OPEN)
        foreach (QAction* action, actions)
            action->setEnabled (false);
    else if (state == APP_STATE_FILE_OPEN)
    {
        foreach (QAction* action, actions)
            action->setEnabled (true);
        getQAction (HAND_MODE_TEXT_)->setChecked (true);
    }
}


}
