#include "adapt_event_view_gui_command.h"
#include "../application_context.h"
#include "../gui/signal_visualisation_model.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
QString const AdaptEventViewGuiCommand::FIT_TO_EVENT_ = "Fit View to Selected Event";
QString const AdaptEventViewGuiCommand::GO_TO_NEXT_EVENT_ = "Goto and Select Next Event";
QString const AdaptEventViewGuiCommand::GO_TO_PREVIOUS_EVENT_ = "Goto and Select Previous Event";
QStringList const AdaptEventViewGuiCommand::TEXTS_ = QStringList() <<
                                              AdaptEventViewGuiCommand::FIT_TO_EVENT_ <<
                                              AdaptEventViewGuiCommand::GO_TO_NEXT_EVENT_ <<
                                              AdaptEventViewGuiCommand::GO_TO_PREVIOUS_EVENT_;

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator AdaptEventViewGuiCommand::registrator_ ("Adapt Event View",
    QSharedPointer<AdaptEventViewGuiCommand> (new AdaptEventViewGuiCommand));


//-----------------------------------------------------------------------------
AdaptEventViewGuiCommand::AdaptEventViewGuiCommand ()
    : GuiActionCommand (TEXTS_)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
AdaptEventViewGuiCommand::~AdaptEventViewGuiCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void AdaptEventViewGuiCommand::init ()
{
    getQAction (GO_TO_NEXT_EVENT_)->setShortcut (QKeySequence("Ctrl+Right"));
    getQAction (GO_TO_PREVIOUS_EVENT_)->setShortcut (QKeySequence("Ctrl+Left"));

    getQAction (GO_TO_NEXT_EVENT_)->setIcon (QIcon(":/images/icons/next.png"));
    getQAction (GO_TO_PREVIOUS_EVENT_)->setIcon (QIcon(":/images/icons/previous.png"));
}

//-----------------------------------------------------------------------------
void AdaptEventViewGuiCommand::trigger (QString const& action_name)
{
    if (action_name == FIT_TO_EVENT_)
        fitViewToEvent ();
    else if (action_name == GO_TO_NEXT_EVENT_)
        gotoAndSelectEvent (true);
    else if (action_name == GO_TO_PREVIOUS_EVENT_)
        gotoAndSelectEvent (false);
}

//-----------------------------------------------------------------------------
void AdaptEventViewGuiCommand::applicationStateChanged (ApplicationState state)
{
    app_state_ = state;
    updateEnabledness ();
}

//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::tabSelectionStateChanged (TabSelectionState state)
{
    tab_sec_state_ = state;
    updateEnabledness ();
}

//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::fitViewToEvent ()
{
    QSharedPointer<MainWindowModel> mw_model = ApplicationContext::getInstance()->getMainWindowModel ();
    QSharedPointer<SignalVisualisationModel> sv_model = mw_model->getCurrentSignalVisualisationModel ();
    EventID event_id = sv_model->getSelectedEvent ();

    if (event_id == UNDEFINED_EVENT_ID)
        return;

    QSharedPointer<EventManager> event_manager = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager();
    QSharedPointer<SignalEvent const> event = event_manager->getEvent (event_id);

    float32 width = sv_model->getShownSignalWidth ();
    float32 desired_pixel_per_sample = width / event->getDuration ();
    float32 pixel_per_sample = 1;
    while (pixel_per_sample < desired_pixel_per_sample)
        pixel_per_sample *= 2;
    while (pixel_per_sample > desired_pixel_per_sample)
        pixel_per_sample /= 2;

    if (pixel_per_sample > 8)
        pixel_per_sample = 8;

    sv_model->setPixelPerSample (pixel_per_sample);
    sv_model->updateLayout ();
    sv_model->goToSample (event->getPosition ());
}

//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::gotoAndSelectEvent (bool forward)
{
    QSharedPointer<EventManager> event_manager = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager();
    QSharedPointer<MainWindowModel> mw_model = ApplicationContext::getInstance()->getMainWindowModel ();
    QSharedPointer<SignalVisualisationModel> sv_model = mw_model->getCurrentSignalVisualisationModel ();
    EventID event_id = sv_model->getSelectedEvent ();

    if (event_id == UNDEFINED_EVENT_ID)
        return;

    QSharedPointer<SignalEvent const> event = event_manager->getEvent (event_id);

    QMap<uint32, EventID> events = event_manager->getEventPositions (event->getType());
    QMap<uint32, EventID>::iterator current_event_iter = events.find (event->getPosition());
    if (forward)
        ++current_event_iter;
    else if (current_event_iter != events.begin())
        --current_event_iter;
    else
        return;

    if (current_event_iter != events.end())
    {
        sv_model->goToSample (current_event_iter.key ());
        sv_model->selectEvent (current_event_iter.value ());
    }
}


//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::updateEnabledness ()
{
    if (app_state_ == APP_STATE_NO_FILE_OPEN)
    {
        getQAction (FIT_TO_EVENT_)->setEnabled (false);
        return;
    }

    if (tab_sec_state_ == TAB_STATE_NO_EVENT_SELECTED)
        getQAction (FIT_TO_EVENT_)->setEnabled (false);
    else
        getQAction (FIT_TO_EVENT_)->setEnabled (true);
}


}
