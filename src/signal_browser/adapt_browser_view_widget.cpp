#include "adapt_browser_view_widget.h"
#include "../gui/gui_action_factory.h"
#include "../base/exception.h"

namespace BioSig_
{

//-------------------------------------------------------------------------
AdaptBrowserViewWidget::AdaptBrowserViewWidget (SignalVisualisationView const* signal_visualisation_view,
                                                QWidget *parent) :
    QWidget (parent),
    signal_visualisation_view_ (signal_visualisation_view)
{
    ui_.setupUi (this);
    if (!connect (ui_.x_axis_checkbox_, SIGNAL(toggled(bool)), SIGNAL(xAxisVisibilityChanged(bool))))
        throw (Exception ("connect failed: x_axis_checkbox_"));
    if (!connect (ui_.y_axis_checkbox_, SIGNAL(toggled(bool)), SIGNAL(yAxisVisibilityChanged(bool))))
        throw (Exception ("connect failed: y_axis_checkbox_"));
    if (!connect (ui_.labels_checkbox_, SIGNAL(toggled(bool)), SIGNAL(labelsVisibilityChanged(bool))))
        throw (Exception ("connect failed: labels_checkbox_"));

    ui_.zero_centered_->setDefaultAction (GuiActionFactory::getInstance()->getQAction("Zero Line Centered"));
    ui_.zero_fitted_->setDefaultAction (GuiActionFactory::getInstance()->getQAction("Zero Line Fitted"));
}

//-------------------------------------------------------------------------
void AdaptBrowserViewWidget::showEvent (QShowEvent*)
{
    ui_.x_axis_checkbox_->setChecked (signal_visualisation_view_->getXAxisVisibility ());
    ui_.y_axis_checkbox_->setChecked (signal_visualisation_view_->getYAxisVisibility ());
    ui_.labels_checkbox_->setChecked (signal_visualisation_view_->getLabelsVisibility ());
}


}
