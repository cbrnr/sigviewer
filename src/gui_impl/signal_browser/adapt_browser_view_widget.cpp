// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "adapt_browser_view_widget.h"
#include "gui/gui_action_factory.h"
#include "base/exception.h"
#include "base/math_utils.h"
#include "gui_impl/gui_helper_functions.h"

#include <QSettings>
#include <QDebug>
#include <QMutexLocker>

#include <cmath>

namespace sigviewer
{

//-------------------------------------------------------------------------
AdaptBrowserViewWidget::AdaptBrowserViewWidget (SignalVisualisationView const* signal_visualisation_view,
                                                QSharedPointer<SignalViewSettings> settings,
                                                QWidget *parent) :
    QWidget (parent),
    signal_visualisation_view_ (signal_visualisation_view),
    settings_ (settings),
    self_updating_ (false),
    updating_values_ (false)
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
    ui_.channelsPerPageSpinbox->setMaximum (settings->getChannelManager().getNumberChannels());
    ui_.secsPerPageSpinbox->setMaximum (settings_->getChannelManager().getDurationInSec());
//    ui_.xUnitsPerPageLabel->setText (settings_->getChannelManager().getXAxisUnitLabel() + ui_.xUnitsPerPageLabel->text());
    ui_.xGridSlider->hide();
    ui_.xGridFragmentationLabel->hide();
    ui_.channelOverlappingSlider->hide();
    ui_.label->hide();

    connect (settings_.data(), SIGNAL(channelHeightChanged()), SLOT(updateValues()));
    connect (settings_.data(), SIGNAL(gridFragmentationChanged()), SLOT(updateValues()));
    connect (settings_.data(), SIGNAL(pixelsPerSampleChanged()), SLOT(updateValues()));
}

//-------------------------------------------------------------------------
void AdaptBrowserViewWidget::showEvent (QShowEvent*)
{
    updateValues ();
}

//-------------------------------------------------------------------------
void AdaptBrowserViewWidget::on_channelOverlappingSlider_valueChanged (int value)
{
    if (updating_values_)
        return;

    settings_->setChannelOverlapping (static_cast<float>(value) / 100.0);
}

//-------------------------------------------------------------------------
void AdaptBrowserViewWidget::on_yGridSlider_valueChanged (int value)
{
    if (updating_values_)
        return;

    settings_->setGridFragmentation (Qt::Vertical, value);
}

//-------------------------------------------------------------------------
void AdaptBrowserViewWidget::on_xGridSlider_valueChanged(int value)
{
    if (updating_values_)
        return;

    settings_->setGridFragmentation (Qt::Horizontal, value);
}

//-------------------------------------------------------------------------
void AdaptBrowserViewWidget::on_channelsPerPageSpinbox_valueChanged (int value)
{
    if (updating_values_)
        return;

    self_updating_ = true;
    int new_channel_height = signal_visualisation_view_->getViewportHeight() /
                             value;
    GuiHelper::animateProperty (settings_.data(), "channelHeight", settings_->getChannelHeight(),
                                new_channel_height, this, SLOT(selfUpdatingFinished()));
}

//-------------------------------------------------------------------------
void AdaptBrowserViewWidget::on_secsPerPageSpinbox_valueChanged (double value)
{
    if (updating_values_)
        return;

    self_updating_ = true;
    float new_pixels_per_sample = signal_visualisation_view_->getViewportWidth() /
                                (value * settings_->getChannelManager().getSampleRate());

    GuiHelper::animateProperty (settings_.data(), "pixelsPerSample", settings_->getPixelsPerSample(),
                                new_pixels_per_sample, this, SLOT(selfUpdatingFinished()));
}

//-------------------------------------------------------------------------
void AdaptBrowserViewWidget::updateValues ()
{
    if ((!isVisible()) || self_updating_)
        return;
    updating_values_ = true;
    ui_.x_axis_checkbox_->setChecked (signal_visualisation_view_->getXAxisVisibility ());
    ui_.y_axis_checkbox_->setChecked (signal_visualisation_view_->getYAxisVisibility ());
    ui_.labels_checkbox_->setChecked (signal_visualisation_view_->getLabelsVisibility ());
    ui_.yGridSlider->setValue (settings_->getGridFragmentation(Qt::Vertical));
    ui_.xGridSlider->setValue (settings_->getGridFragmentation(Qt::Horizontal));
    ui_.channelsPerPageSpinbox->setValue (signal_visualisation_view_->getViewportHeight() /
                                          settings_->getChannelHeight());
    ui_.secsPerPageSpinbox->setValue ((signal_visualisation_view_->getViewportWidth() /
                                       settings_->getPixelsPerSample()) /
                                      settings_->getChannelManager().getSampleRate());
    updating_values_ = false;
}

//-------------------------------------------------------------------------
void AdaptBrowserViewWidget::selfUpdatingFinished ()
{
    self_updating_ = false;
}

}

void sigviewer::AdaptBrowserViewWidget::on_yGridCheckbox_stateChanged(int checkState)
{
    if (checkState == Qt::Unchecked)
    {//cancel Y Grid
        ui_.yGridSlider->setDisabled(true);
        settings_->enableYGrid(false);
    }
    else if (checkState == Qt::Checked)
    {
        ui_.yGridSlider->setEnabled(true);
        settings_->enableYGrid(true);
    }
}

void sigviewer::AdaptBrowserViewWidget::on_xGridCheckbox_stateChanged(int checkState)
{
    if (checkState == Qt::Unchecked)
    {//cancel X Grid
        ui_.xGridSlider->setDisabled(true);
        settings_->enableXGrid(false);
    }
    else if (checkState == Qt::Checked)
    {
        ui_.xGridSlider->setEnabled(true);
        settings_->enableXGrid(true);
    }

}


void sigviewer::AdaptBrowserViewWidget::on_horizontalSlider_valueChanged(int value)
{
    if (updating_values_)
        return;

    settings_->setChannelOverlapping (static_cast<float>(value) / 100.0);
}
