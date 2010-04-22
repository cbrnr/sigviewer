#include "fit_view_gui_command.h"
#include "../application_context.h"
#include "../gui/signal_visualisation_model.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
QString const FitViewGuiCommand::FIT_TO_EVENT_ = tr("Fit View to Selected Event");
QStringList const FitViewGuiCommand::TEXTS_ = QStringList() <<
                                              FitViewGuiCommand::FIT_TO_EVENT_;

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator FitViewGuiCommand::registrator_ ("Fit View",
    QSharedPointer<FitViewGuiCommand> (new FitViewGuiCommand));


//-----------------------------------------------------------------------------
FitViewGuiCommand::FitViewGuiCommand ()
    : GuiActionCommand (TEXTS_)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
FitViewGuiCommand::~FitViewGuiCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void FitViewGuiCommand::init ()
{

}

//-----------------------------------------------------------------------------
void FitViewGuiCommand::trigger (QString const& action_name)
{
    if (action_name == FIT_TO_EVENT_)
        fitViewToEvent ();
}

//-----------------------------------------------------------------------------
void FitViewGuiCommand::applicationStateChanged (ApplicationState state)
{
    app_state_ = state;
    updateEnabledness ();
}

//-------------------------------------------------------------------------
void FitViewGuiCommand::tabSelectionStateChanged (TabSelectionState state)
{
    tab_sec_state_ = state;
    updateEnabledness ();
}

//-------------------------------------------------------------------------
void FitViewGuiCommand::fitViewToEvent ()
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

    if (pixel_per_sample > 1)
        pixel_per_sample = 1;

    sv_model->setPixelPerSample (pixel_per_sample);
    sv_model->updateLayout ();
    sv_model->goToSample (event->getPosition ());
}

//-------------------------------------------------------------------------
void FitViewGuiCommand::updateEnabledness ()
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
