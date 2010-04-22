#include "zoom_gui_command.h"
#include "../application_context.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
QString const ZoomGuiCommand::ZOOM_IN_VERTICAL_TEXT_ = tr("Zoom In Vertical");
QString const ZoomGuiCommand::ZOOM_OUT_VERTICAL_TEXT_ = tr("Zoom Out Vertical");
QString const ZoomGuiCommand::ZOOM_IN_HORIZONTAL_TEXT_ = tr("Zoom In Horizontal");
QString const ZoomGuiCommand::ZOOM_OUT_HORIZONTAL_TEXT_ = tr("Zoom Out Horizontal");


QStringList const ZoomGuiCommand::TEXTS_ = QStringList() << ZoomGuiCommand::ZOOM_IN_VERTICAL_TEXT_
                                           << ZoomGuiCommand::ZOOM_OUT_VERTICAL_TEXT_
                                           << ZoomGuiCommand::ZOOM_IN_HORIZONTAL_TEXT_
                                           << ZoomGuiCommand::ZOOM_OUT_HORIZONTAL_TEXT_;


//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator ZoomGuiCommand::registrator_ ("Zooming",
                                                          QSharedPointer<ZoomGuiCommand> (new ZoomGuiCommand));



//-----------------------------------------------------------------------------
ZoomGuiCommand::ZoomGuiCommand ()
    : GuiActionCommand (TEXTS_)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void ZoomGuiCommand::init ()
{
    getQAction (ZOOM_IN_VERTICAL_TEXT_)->setIcon (QIcon(":/images/icons/zoom_in_vertical.png"));
    getQAction (ZOOM_OUT_VERTICAL_TEXT_)->setIcon (QIcon(":/images/icons/zoom_out_vertical.png"));
    getQAction (ZOOM_IN_HORIZONTAL_TEXT_)->setIcon (QIcon(":/images/icons/zoom_in_horizontal.png"));
    getQAction (ZOOM_OUT_HORIZONTAL_TEXT_)->setIcon (QIcon(":/images/icons/zoom_out_horizontal.png"));
}

//-----------------------------------------------------------------------------
void ZoomGuiCommand::trigger (QString const& action_name)
{
    QSharedPointer<MainWindowModel> main_window_model =
            ApplicationContext::getInstance()->getMainWindowModel ();
    QSharedPointer<SignalVisualisationModel> vis_model =
            main_window_model->getCurrentSignalVisualisationModel();

    float32 pixel_per_sample = vis_model->getPixelPerSample ();
    if (action_name == ZOOM_IN_VERTICAL_TEXT_)
        vis_model->zoom (ZOOM_VERTICAL, ZOOM_FACTOR_);
    else if (action_name == ZOOM_OUT_VERTICAL_TEXT_)
        vis_model->zoom (ZOOM_VERTICAL, ZOOM_FACTOR_ * -1);
    else if (action_name == ZOOM_IN_HORIZONTAL_TEXT_)
    {
        pixel_per_sample *= ZOOM_FACTOR_;
        if (pixel_per_sample > 1)
            pixel_per_sample = 1;
        vis_model->setPixelPerSample (pixel_per_sample);
    }
    else if (action_name == ZOOM_OUT_HORIZONTAL_TEXT_)
        vis_model->setPixelPerSample (pixel_per_sample / ZOOM_FACTOR_);
    vis_model->updateLayout ();
}

//-----------------------------------------------------------------------------
void ZoomGuiCommand::applicationStateChanged (ApplicationState state)
{
    if (state == APP_STATE_NO_FILE_OPEN)
        emit qActionEnabledChanged (false);
    else
        emit qActionEnabledChanged (true);
}



}
