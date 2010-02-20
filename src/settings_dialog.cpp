/*
 * settings_dialog.cpp
 *
 *  Created on: 12.08.2008
 *      Author: oliver
 */

#include "settings_dialog.h"
#include "base/user_types.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSettings>
#include <QRadioButton>
#include <QCheckBox>
#include <QGroupBox>


namespace BioSig_

{

const char* SettingsDialog::DEFAULT_AUTO_ZOOM_BEHAVIOUR_GROUP_BOX_TITLE_ =
            "Default auto zoom behavior:";
const char* SettingsDialog::SCALE_MAX_TO_MAX_BUTTON_TEXT_ =
            "Scale from -|MAX| to +|MAX|";
const char* SettingsDialog::SCALE_MIN_TO_MAX_BUTTON_TEXT_ =
            "Scale from MIN to MAX";
const char* SettingsDialog::OVERFLOW_DETECTION_BUTTON_TEXT_ =
            "Enable overflow detection";
const char* SettingsDialog::SHOW_CHANNEL_SCALES_BUTTON_TEXT_ =
            "Show channel scales";
const char* SettingsDialog::SHOW_CHANNEL_LABELS_BUTTON_TEXT_ =
            "Show channel labels";
const char* SettingsDialog::SHOW_EVENT_INFO_TEXT_ =
            "Show event info widget";
const char* SettingsDialog::SHOW_GRID_BUTTON_TEXT_ = "Show grid";
const char* SettingsDialog::OK_BUTTON_TEXT_ = "OK";
const char* SettingsDialog::CANCEL_BUTTON_TEXT_ = "Cancel";

// ------------------------------------------------------------------------------------------
// constructor
SettingsDialog::SettingsDialog (QSharedPointer<SignalBrowserModel const> signal_browser_model,
                                QWidget* parent)
: QDialog(parent),
  signal_browser_model_ (signal_browser_model)
{
    setWindowTitle(tr("Preferences"));

    QGroupBox* box = new QGroupBox(tr(DEFAULT_AUTO_ZOOM_BEHAVIOUR_GROUP_BOX_TITLE_), this);

    QVBoxLayout* boxLayout = new QVBoxLayout();

    scale_max_to_max_radio_button_ = new QRadioButton(tr(SCALE_MAX_TO_MAX_BUTTON_TEXT_));
    scale_min_to_max_radio_button_ = new QRadioButton(tr(SCALE_MIN_TO_MAX_BUTTON_TEXT_));

    bool max_to_max_zoom_enabled = signal_browser_model_->getAutoZoomBehaviour() == MAX_TO_MAX;
    scale_max_to_max_radio_button_->setChecked (max_to_max_zoom_enabled);
    scale_min_to_max_radio_button_->setChecked (!max_to_max_zoom_enabled);

    boxLayout->addWidget(scale_max_to_max_radio_button_);
    boxLayout->addWidget(scale_min_to_max_radio_button_);
    boxLayout->addStretch(1);
    box->setLayout(boxLayout);

    overflow_detection_button_ = new QCheckBox(tr(OVERFLOW_DETECTION_BUTTON_TEXT_));
    show_channel_scales_button_ = new QCheckBox(tr(SHOW_CHANNEL_SCALES_BUTTON_TEXT_));
    show_channel_scales_button_->setChecked (signal_browser_model_->getShowXScales());
    show_channel_labels_button_ = new QCheckBox(tr(SHOW_CHANNEL_LABELS_BUTTON_TEXT_));
    show_channel_labels_button_->setChecked (signal_browser_model_->getShowChannelLabels());

    show_grid_button_ = new QCheckBox(tr(SHOW_GRID_BUTTON_TEXT_));
    show_grid_button_->setChecked (signal_browser_model_->getGridVisible());

    show_event_info_button_ = new QCheckBox (tr(SHOW_EVENT_INFO_TEXT_), this);
    show_event_info_button_->setChecked(signal_browser_model_->getShowEventInfo());

    QVBoxLayout* top_layout = new QVBoxLayout(this);

    top_layout->addWidget(box);
    top_layout->addWidget(overflow_detection_button_);
    top_layout->addWidget(show_channel_scales_button_);
    top_layout->addWidget(show_channel_labels_button_);
    top_layout->addWidget(show_grid_button_);
    top_layout->addWidget(show_event_info_button_);

    top_layout->setMargin(10);
    top_layout->setSpacing(10);
    top_layout->addStretch(1);

    QHBoxLayout* button_layout = new QHBoxLayout(this);

    button_layout->setMargin(0);
    button_layout->addStretch(1);

    ok_button_ = new QPushButton(tr(OK_BUTTON_TEXT_), this);

    button_layout->addWidget(ok_button_);

    cancel_button_ = new QPushButton(tr(CANCEL_BUTTON_TEXT_), this);

    button_layout->addWidget(cancel_button_);
    button_layout->addStretch(1);

    top_layout->addLayout(button_layout);

    top_layout->activate();

    connect(ok_button_, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancel_button_, SIGNAL(clicked()), this, SLOT(reject()));
//    connect(show_channel_labels_button_, SIGNAL(stateChanged(int)), this, SLOT(setShowChannels(int)));
}

// ------------------------------------------------------------------------------------------
bool SettingsDialog::isShowChannelScales()
{
    return show_channel_scales_button_->isChecked();
}

// ------------------------------------------------------------------------------------------
bool SettingsDialog::isShowChannelLables()
{
    return show_channel_labels_button_->isChecked();
}

// ------------------------------------------------------------------------------------------
bool SettingsDialog::isShowGrid()
{
    return show_grid_button_->isChecked();
}

// ------------------------------------------------------------------------------------------
bool SettingsDialog::isOverflowDetection()
{
    return overflow_detection_button_->isChecked();
}

// ------------------------------------------------------------------------------------------
bool SettingsDialog::isShowEventInfo()
{
    return show_event_info_button_->isChecked();
}


// ------------------------------------------------------------------------------------------
ScaleMode SettingsDialog::getScaleModeType()
{
    return (scale_max_to_max_radio_button_->isChecked() ? MAX_TO_MAX : MIN_TO_MAX);
}

} //namespace BioSig_

