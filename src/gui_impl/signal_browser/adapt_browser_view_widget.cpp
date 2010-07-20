#include "adapt_browser_view_widget.h"
#include "gui/gui_action_factory.h"
#include "base/exception.h"
#include "base/math_utils.h"

#include <QSettings>
#include <QDebug>

#include <cmath>

namespace BioSig_
{

//-------------------------------------------------------------------------
AdaptBrowserViewWidget::AdaptBrowserViewWidget (SignalVisualisationView const* signal_visualisation_view,
                                                QSharedPointer<SignalVisualisationModel> sig_vis_model,
                                                QWidget *parent) :
    QWidget (parent),
    signal_visualisation_view_ (signal_visualisation_view),
    sig_vis_model_ (sig_vis_model)
{
    ui_.setupUi (this);
    if (!connect (ui_.x_axis_checkbox_, SIGNAL(toggled(bool)), SIGNAL(xAxisVisibilityChanged(bool))))
        throw (Exception ("connect failed: x_axis_checkbox_"));
    if (!connect (ui_.y_axis_checkbox_, SIGNAL(toggled(bool)), SIGNAL(yAxisVisibilityChanged(bool))))
        throw (Exception ("connect failed: y_axis_checkbox_"));
    if (!connect (ui_.labels_checkbox_, SIGNAL(toggled(bool)), SIGNAL(labelsVisibilityChanged(bool))))
        throw (Exception ("connect failed: labels_checkbox_"));
    if (!connect (ui_.y_grid_slider_, SIGNAL(valueChanged(int)), SIGNAL(yGridChanged(int))))
        throw (Exception ("connect failed: y_grid_spinbox_"));

    ui_.zero_centered_->setDefaultAction (GuiActionFactory::getInstance()->getQAction("Zero Line Centered"));
    ui_.zero_fitted_->setDefaultAction (GuiActionFactory::getInstance()->getQAction("Zero Line Fitted"));
}

//-------------------------------------------------------------------------
void AdaptBrowserViewWidget::showEvent (QShowEvent*)
{
    ui_.x_axis_checkbox_->setChecked (signal_visualisation_view_->getXAxisVisibility ());
    ui_.y_axis_checkbox_->setChecked (signal_visualisation_view_->getYAxisVisibility ());
    ui_.labels_checkbox_->setChecked (signal_visualisation_view_->getLabelsVisibility ());
    ui_.y_grid_slider_->setValue (sig_vis_model_->getYGridFragmentation());
}




}
