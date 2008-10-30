/*
 * settings_dialog.h
 *
 *  Created on: 12.08.2008
 *      Author: oliver
 */

#ifndef SETTINGS_DIALOG_H_
#define SETTINGS_DIALOG_H_

#include "base/user_types.h"

#include <QDialog>

class QPushButton;
class QRadioButton;
class QCheckBox;

namespace BioSig_
{

//TODO: enum auslagern, weil sonst muss überall settings_dialog.h inkludiert werden... eventuell in base/user_types.h ?
enum ScaleMode
{
	MAX_TO_MAX,
	MIN_TO_MAX
};

// channel selection dialog
class SettingsDialog : public QDialog
{

	Q_OBJECT

public:

	SettingsDialog(QWidget* parent = 0);
    void loadSettings();
    void saveSettings();

    bool isShowChannelScales();
    bool isShowChannelLables();
    bool isShowGrid();
    bool isOverflowDetection();
    ScaleMode getScaleModeType();

//private slots:
//
//	void setShowChannels(int state);

private:

	//translations
	static const char* DEFAULT_AUTO_ZOOM_BEHAVIOUR_GROUP_BOX_TITLE_;
	static const char* SCALE_MAX_TO_MAX_BUTTON_TEXT_;
	static const char* SCALE_MIN_TO_MAX_BUTTON_TEXT_;
	static const char* OVERFLOW_DETECTION_BUTTON_TEXT_;
	static const char* SHOW_CHANNEL_SCALES_BUTTON_TEXT_;
	static const char* SHOW_CHANNEL_LABELS_BUTTON_TEXT_;
	static const char* SHOW_GRID_BUTTON_TEXT_;
	static const char* OK_BUTTON_TEXT_;
	static const char* CANCEL_BUTTON_TEXT_;

	//buttons
    QPushButton* ok_button_;
    QPushButton* cancel_button_;
    QRadioButton* scale_max_to_max_radio_button_;
    QRadioButton* scale_min_to_max_radio_button_;
    QCheckBox* overflow_detection_button_;
    QCheckBox* show_channel_scales_button_;
    QCheckBox* show_channel_labels_button_;
    QCheckBox* show_grid_button_;

//    bool show_channel_labels_;
};

} //namespace BioSig_



#endif /* SETTINGS_DIALOG_H_ */
