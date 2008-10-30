/*
 * settings_dialog.cpp
 *
 *  Created on: 12.08.2008
 *      Author: oliver
 */

#include "settings_dialog.h"
//#include "base/basic_header.h"

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
const char* SettingsDialog::SHOW_GRID_BUTTON_TEXT_ = "Show grid";
const char* SettingsDialog::OK_BUTTON_TEXT_ = "OK";
const char* SettingsDialog::CANCEL_BUTTON_TEXT_ = "Cancel";

// ------------------------------------------------------------------------------------------
// constructor
SettingsDialog::SettingsDialog(QWidget* parent)

: QDialog(parent)
{
	setWindowTitle(tr("Preferences"));

	QGroupBox* box = new QGroupBox(tr(DEFAULT_AUTO_ZOOM_BEHAVIOUR_GROUP_BOX_TITLE_), this);

	QVBoxLayout* boxLayout = new QVBoxLayout();

	scale_max_to_max_radio_button_ = new QRadioButton(tr(SCALE_MAX_TO_MAX_BUTTON_TEXT_));
	scale_min_to_max_radio_button_ = new QRadioButton(tr(SCALE_MIN_TO_MAX_BUTTON_TEXT_));

	boxLayout->addWidget(scale_max_to_max_radio_button_);
	boxLayout->addWidget(scale_min_to_max_radio_button_);
	boxLayout->addStretch(1);
	box->setLayout(boxLayout);

	overflow_detection_button_ = new QCheckBox(tr(OVERFLOW_DETECTION_BUTTON_TEXT_));
	show_channel_scales_button_ = new QCheckBox(tr(SHOW_CHANNEL_SCALES_BUTTON_TEXT_));
	show_channel_labels_button_ = new QCheckBox(tr(SHOW_CHANNEL_LABELS_BUTTON_TEXT_));
	show_grid_button_ = new QCheckBox(tr(SHOW_GRID_BUTTON_TEXT_));

    QVBoxLayout* top_layout = new QVBoxLayout(this);

    top_layout->addWidget(box);
    top_layout->addWidget(overflow_detection_button_);
    top_layout->addWidget(show_channel_scales_button_);
    top_layout->addWidget(show_channel_labels_button_);
    top_layout->addWidget(show_grid_button_);

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

//void SettingsDialog::setShowChannels(int channelState)
//{
//	show_channel_labels_ = ((channelState == Qt::Checked) ? (true) : (false));
//}

// ------------------------------------------------------------------------------------------
// load settings
void SettingsDialog::loadSettings()

{
    QSettings settings("SigViewer");

    settings.beginGroup("SettingsDialog");

    overflow_detection_button_->setChecked(settings.value("overflow_detection", false).toBool());
    scale_max_to_max_radio_button_->setChecked(settings.value("scale_max_to_max_radio_button_", true).toBool());
	scale_min_to_max_radio_button_->setChecked(settings.value("scale_min_to_max_radio_button_", false).toBool());
	show_channel_scales_button_->setChecked(settings.value("show_channel_scales_button_", false).toBool());
	show_channel_labels_button_->setChecked(settings.value("show_channel_labels_button_", false).toBool());
	show_grid_button_->setChecked(settings.value("show_grid_button_", false).toBool());

    resize(settings.value("size", QSize(250, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();

}

// ------------------------------------------------------------------------------------------
// save settings
void SettingsDialog::saveSettings()

{
    QSettings settings("SigViewer");

    settings.beginGroup("SettingsDialog");

    settings.setValue("overflow_detection", overflow_detection_button_->isChecked());
    settings.setValue("scale_max_to_max_radio_button_", scale_max_to_max_radio_button_->isChecked());
    settings.setValue("scale_min_to_max_radio_button_", scale_min_to_max_radio_button_->isChecked());
    settings.setValue("show_channel_scales_button_", show_channel_scales_button_->isChecked());
    settings.setValue("show_channel_labels_button_", show_channel_labels_button_->isChecked());
    settings.setValue("show_grid_button_", show_grid_button_->isChecked());

    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
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
ScaleMode SettingsDialog::getScaleModeType()
{
	return (scale_max_to_max_radio_button_->isChecked() ? MAX_TO_MAX : MIN_TO_MAX);
}

} //namespace BioSig_

