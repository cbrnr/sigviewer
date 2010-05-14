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

    QAction* zero_centered = GuiActionFactory::getInstance()->getQAction("Zero Line Centered");
    if (!zero_centered->connect (ui_.zero_centered_, SIGNAL(clicked()), SLOT(trigger())))
        throw (Exception ("connect failed: zero_centered_"));

    QAction* zero_fitted = GuiActionFactory::getInstance()->getQAction("Zero Line Fitted");
    if (!zero_fitted->connect (ui_.zero_fitted_, SIGNAL(clicked()), SLOT(trigger())))
        throw (Exception ("connect failed: zero_fitted_"));
}

//-------------------------------------------------------------------------
void AdaptBrowserViewWidget::showEvent (QShowEvent* event)
{
}


}
