#include "adapt_browser_view_widget.h"
#include "../gui/gui_action_factory.h"
#include "../base/exception.h"

namespace BioSig_
{

//-------------------------------------------------------------------------
AdaptBrowserViewWidget::AdaptBrowserViewWidget(QWidget *parent) :
    QWidget (parent)
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
}


}
