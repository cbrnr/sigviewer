/*  main_window.cpp

    Copyright (C) Thomas Brunner
                  Clemens Brunner
    		      Christoph Eibel   
    		      Alois Schloegl
    		      Oliver Terbu
    This file is part of the "SigViewer" repository
    at http://biosig.sf.net/

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


// main_window.cpp

#include "main_window.h"
#include "main_window_model.h"

#include <QAction>
#include <QMessageBox>
#include <QFileDialog>
#include <QComboBox>
#include <QValidator>
#include <QStringList>
#include <QActionGroup>
#include <QCloseEvent>
#include <QToolBar>
#include <QMenuBar>
#include <QLineEdit>
#include <QTextStream>
#include <QStatusBar>
#include <QLabel>
#include <QSettings>

#include <QUrl>

namespace BioSig_
{

// constructor
MainWindow::MainWindow(MainWindowModel& model)
 : QMainWindow(0),
   model_(model)
{
    setAcceptDrops (true);
    setWindowIcon(QIcon(":images/sigviewer16.png"));
    initStatusBar();
    initIconSets();
    initActions();
    initToolBars();
    initMenus();
    resize(QSize(800, 600).expandedTo(minimumSizeHint()));

}

// destructor
MainWindow::~MainWindow()
{
    // nothing
}

// init status bar
void MainWindow::initStatusBar()
{
    QStatusBar* status_bar = statusBar();
    status_bar->showMessage(tr("Ready"));

    status_bar_signal_length_label_ = new QLabel(tr("Length: %1 s").arg("----.-"));
    status_bar_nr_channels_label_ = new QLabel(tr("Channels: %1").arg("---"));
    status_bar_nr_trials_label_ = new QLabel(tr("Trials: %1").arg("---"));
    status_bar_signal_length_label_->setAlignment(Qt::AlignHCenter);
    status_bar_nr_channels_label_->setAlignment(Qt::AlignHCenter);
    status_bar_nr_trials_label_->setAlignment(Qt::AlignHCenter);
    status_bar_signal_length_label_->setMinimumWidth(status_bar_signal_length_label_->sizeHint().width() + 10);
    status_bar_nr_channels_label_->setMinimumWidth(status_bar_nr_channels_label_->sizeHint().width() + 10);
    status_bar_nr_trials_label_->setMinimumWidth(status_bar_nr_trials_label_->sizeHint().width() + 10);
    status_bar->addPermanentWidget(status_bar_signal_length_label_);
    status_bar->addPermanentWidget(status_bar_nr_channels_label_);
    status_bar->addPermanentWidget(status_bar_nr_trials_label_);
}

// init icons sets
void MainWindow::initIconSets()
{
    file_open_icon_.addFile(":/images/fileopen_22x22.png");
    file_save_icon_.addFile(":/images/save_22x22.png");
    file_save_as_icon_.addFile(":/images/save_as_22x22.png");
    file_close_icon_.addFile(":/images/fileclose_22x22.png");
    file_info_icon_.addFile(":/images/file_info_22x22.png");
    file_exit_icon_.addFile(":/images/exit_22x22.png");
    edit_to_all_channels_icon_.addFile(":/images/to_all_channels_22x22.png");
    edit_copy_to_channels_icon_.addFile(":/images/copy_to_channels_22x22.png");
    edit_delete_icon_.addFile(":/images/delete_22x22.png");
    edit_change_channel_icon_.addFile(":/images/change_channel_22x22.png");
    edit_change_type_icon_.addFile(":/images/change_type_22x22.png");
    mouse_mode_new_icon_.addFile(":/images/new_22x22.png");
    mouse_mode_pointer_icon_.addFile(":/images/pointer_22x22.png");
    mouse_mode_hand_icon_.addFile(":/images/hand_22x22.png");
    mouse_mode_shift_signal_icon_.addFile(":/images/shift_signal_22x22.png");
    mouse_mode_zoom_icon_.addFile(":/images/zoom_22x22.png");
    view_zoom_in_icon_.addFile(":/images/zoom_in_22x22.png");
    view_zoom_out_icon_.addFile(":/images/zoom_out_22x22.png");
    view_auto_scale_icon_.addFile(":/images/auto_scale_22x22.png");
    options_channels_icon_.addFile(":/images/channels_22x22.png");
    options_show_events_icon_.addFile(":/images/events_22x22.png");
    //TODO: (Oliver) add icon
    //OLIVER - CHANGE - BEGINS
    //options_show_settings_icon_.addFile(":/images/settings_22x22.png");
    //OLIVER - CHANGE - ENDS
    help_about_icon_.addFile(":/images/help_22x22.png");
}

// init actions
void MainWindow::initActions()
{
    undo_view_action_ = new QAction(tr("Previous View"), this);
    connect(undo_view_action_, SIGNAL(triggered()),
            &model_, SLOT(undoViewAction()));

    redo_view_action_ = new QAction(tr("Redo View"), this);
    connect(redo_view_action_, SIGNAL(triggered()),
            &model_, SLOT(redoViewAction()));


    calculate_mean_action_ = new QAction(tr("Calculate Mean (alpha)"), this);
    calculate_mean_action_->setStatusTip(tr("Calculates mean of selected event type"));
    calculate_mean_action_->setEnabled(true);
    connect(calculate_mean_action_, SIGNAL(triggered()),
            &model_, SLOT(calculateMeanAction()));

    calculate_frequency_spectrum_action_ = new QAction(tr("Calculate Power Spectrum"), this);
    calculate_frequency_spectrum_action_->setStatusTip(tr("Calculates power spectrum of selected event type"));
    calculate_frequency_spectrum_action_->setEnabled(false);
    connect(calculate_frequency_spectrum_action_, SIGNAL(triggered()),
            &model_, SLOT(calculateFrequencySpectrumAction()));

    calculate_erd_ers_map_action_ = new QAction(tr("Generate ERD/ERS Map"), this);
    calculate_erd_ers_map_action_->setStatusTip(tr("Calculates ERD/ERS map of selected event type"));
    calculate_erd_ers_map_action_->setEnabled(false);
    connect(calculate_erd_ers_map_action_, SIGNAL(triggered()),
            &model_, SLOT(calculateERDERSMap()));


    undo_action_ = new QAction(tr("Undo"), this);
    undo_action_->setShortcut(tr("Ctrl+Z"));
    undo_action_->setShortcutContext(Qt::ApplicationShortcut);
    connect(undo_action_, SIGNAL(triggered()),
            &model_, SLOT(undoAction()));

    redo_action_ = new QAction(tr("Redo"), this);
    redo_action_->setShortcut(tr("Ctrl+Shift+Z"));
    connect(redo_action_, SIGNAL(triggered()),
            &model_, SLOT(redoAction()));


    file_open_action_ = new QAction(file_open_icon_, tr("&Open..."), this);
    file_open_action_->setObjectName("file_open_action_");
    file_open_action_->setShortcut(tr("Ctrl+O"));
    file_open_action_->setStatusTip(tr("Open a signal file"));
    connect(file_open_action_, SIGNAL(triggered()),
            &model_, SLOT(fileOpenAction()));

    file_save_action_ = new QAction(file_save_icon_, tr("&Save..."), this);
    file_save_action_->setObjectName("file_save_action_");
    file_save_action_->setShortcut(tr("Ctrl+S"));
    file_save_action_->setStatusTip(tr("Save signal file"));
    connect(file_save_action_, SIGNAL(triggered()),
            &model_, SLOT(fileSaveAction()));

    file_save_as_action_ = new QAction(file_save_as_icon_, tr("Save As..."),
                                       this);
    file_save_as_action_->setObjectName("file_save_as_action_");
    file_save_as_action_->setStatusTip(tr("Save the signal file under a new name"));
    connect(file_save_as_action_, SIGNAL(triggered()),
            &model_, SLOT(fileSaveAsAction()));

    file_export_events_action_ = new QAction(tr("Export Events..."), this);
    file_export_events_action_->setObjectName("file_export_events_action_");
    file_export_events_action_->setStatusTip(tr("Export events to file"));
    connect(file_export_events_action_, SIGNAL(triggered()),
            &model_, SLOT(fileExportEventsAction()));

    file_import_events_action_ = new QAction(tr("Import Events..."), this);
    file_import_events_action_->setObjectName("file_import_events_action_");
    file_import_events_action_->setStatusTip(tr("Import events from file"));
    connect(file_import_events_action_, SIGNAL(triggered()),
            &model_, SLOT(fileImportEventsAction()));

    file_close_action_ = new QAction(file_close_icon_, tr("&Close"), this);
    file_close_action_->setObjectName("file_close_action_");
    file_close_action_->setStatusTip(tr("Close the opened signal file"));
    connect(file_close_action_, SIGNAL(triggered()),
            &model_, SLOT(fileCloseAction()));

    file_info_action_ = new QAction(file_info_icon_, tr("&Info..."), this);
    file_info_action_->setObjectName("file_info_action_");
    //file_info_action_->setShortcut(tr("Ctrl+I"));
    file_info_action_->setStatusTip(tr("Show the basic information of the signal file"));
    connect(file_info_action_, SIGNAL(triggered()),
            &model_, SLOT(fileInfoAction()));

    file_exit_action_ = new QAction(file_exit_icon_, tr("E&xit"), this);
    file_exit_action_->setObjectName("file_exit_action_");
    file_exit_action_->setStatusTip(tr("Exit the application"));
    connect(file_exit_action_, SIGNAL(triggered()),
            &model_, SLOT(fileExitAction()));

    edit_to_all_channels_action_ = new QAction(edit_to_all_channels_icon_,
                                               tr("To &all Channels..."), this);
    edit_to_all_channels_action_->setObjectName("edit_to_all_channels_action_");
    edit_to_all_channels_action_->setShortcut(tr("Ctrl+A"));
    edit_to_all_channels_action_->setStatusTip(tr("Put the selected event to all channels"));
    connect(edit_to_all_channels_action_, SIGNAL(triggered()),
            &model_, SLOT(editToAllChannelsAction()));

    edit_copy_to_channels_action_ = new QAction(edit_copy_to_channels_icon_,
                                                tr("Co&py To Channels..."),
                                                this);
    edit_copy_to_channels_action_
        ->setObjectName("edit_copy_to_channels_action_");
    edit_copy_to_channels_action_->setShortcut(tr("Ctrl+P"));
    edit_copy_to_channels_action_
        ->setStatusTip(tr("Copy the selected event to other channels (\"Edit Events\"-Mode: Ctrl+LeftMousePress)"));
    connect(edit_copy_to_channels_action_, SIGNAL(triggered()),
            &model_, SLOT(editCopyToChannelsAction()));

    edit_delete_action_ = new QAction(edit_delete_icon_, tr("&Delete"), this);
    edit_delete_action_->setObjectName("edit_delete_action_");
    edit_delete_action_->setShortcut(tr("Del"));
    edit_delete_action_->setStatusTip(tr("Delete the selected event"));
    connect(edit_delete_action_, SIGNAL(triggered()),
            &model_, SLOT(editDeleteAction()));

    edit_change_channel_action_ = new QAction(edit_change_channel_icon_,
                                              tr("Change Cha&nnel..."), this);
    edit_change_channel_action_->setObjectName("edit_change_channel_action_");
    edit_change_channel_action_->setShortcut(tr("Ctrl+N"));
    edit_change_channel_action_
        ->setStatusTip(tr("Change the channel of the selected event (\"Edit Events\"-Mode: Shift+LeftMousePress)"));
    connect(edit_change_channel_action_, SIGNAL(triggered()),
            &model_, SLOT(editChangeChannelAction()));

    edit_change_type_action_ = new QAction(edit_change_type_icon_,
                                           tr("Change &Type..."), this);
    edit_change_type_action_->setObjectName("edit_change_type_action_");
    edit_change_type_action_->setShortcut(tr("Ctrl+T"));
    edit_change_type_action_->setStatusTip(tr("Change the type of the selected event"));
    connect(edit_change_type_action_, SIGNAL(triggered()),
            &model_, SLOT(editChangeTypeAction()));

    edit_event_table_action_ = new QAction(tr("&Event Table..."), this);
    edit_event_table_action_->setObjectName("edit_event_table_action_");
    edit_event_table_action_->setStatusTip(tr("Edit the events in a Table"));
    connect(edit_event_table_action_, SIGNAL(triggered()),
            &model_, SLOT(editEventTableAction()));

    QActionGroup* mouse_mode_action_group = new QActionGroup(this);
    mouse_mode_action_group->setExclusive(true);

    mouse_mode_new_action_ = new QAction(mouse_mode_new_icon_,
                                         tr("&New Events"), mouse_mode_action_group);
    mouse_mode_new_action_->setObjectName("mouse_mode_new_action_");
    mouse_mode_new_action_->setCheckable(true);
    mouse_mode_new_action_->setShortcut(tr("Ctrl+1"));
    mouse_mode_new_action_->setStatusTip(tr("Set new events"));
    connect(mouse_mode_new_action_, SIGNAL(triggered()),
            &model_, SLOT(mouseModeNewAction()));

    mouse_mode_pointer_action_ = new QAction(mouse_mode_pointer_icon_,
                                             tr("&Edit Events"),
                                             mouse_mode_action_group);
    mouse_mode_pointer_action_->setObjectName("mouse_mode_pointer_action_");
    mouse_mode_pointer_action_->setCheckable(true);
    mouse_mode_pointer_action_->setShortcut(tr("Ctrl+2"));
    mouse_mode_pointer_action_->setStatusTip(tr("Edit existing events"));
    connect(mouse_mode_pointer_action_, SIGNAL(triggered()),
            &model_, SLOT(mouseModePointerAction()));

    mouse_mode_hand_action_ = new QAction(mouse_mode_hand_icon_,
                                             tr("&Scroll"),
                                             mouse_mode_action_group);
    mouse_mode_hand_action_->setObjectName("mouse_mode_hand_action_");
    mouse_mode_hand_action_->setCheckable(true);
    mouse_mode_hand_action_->setShortcut(tr("Ctrl+3"));
    mouse_mode_hand_action_->setStatusTip(tr("Scroll signal file (All Modes: MiddleMousePress)"));
    connect(mouse_mode_hand_action_, SIGNAL(triggered()),
            &model_, SLOT(mouseModeHandAction()));

    mouse_mode_shift_signal_action_ = new QAction(mouse_mode_shift_signal_icon_,
                                                  tr("&Shift Signal"),
                                                  mouse_mode_action_group);
    mouse_mode_shift_signal_action_
        ->setObjectName("mouse_mode_shift_signal_action_");
    mouse_mode_shift_signal_action_->setCheckable(true);
    mouse_mode_shift_signal_action_->setShortcut(tr("Ctrl+4"));
    mouse_mode_shift_signal_action_->setStatusTip(tr("Shift one channel in y-direction (All Modes: Shift+MiddleMousePress)"));
    connect(mouse_mode_shift_signal_action_, SIGNAL(triggered()),
            &model_, SLOT(mouseModeShiftSignalAction()));

//    mouse_mode_zoom_action_ = new QAction(mouse_mode_zoom_icon_,  tr("&Zoom"),
//                                          mouse_mode_action_group);
//    mouse_mode_zoom_action_->setObjectName("mouse_mode_zoom_action_");
//    mouse_mode_zoom_action_->setCheckable(true);
//    mouse_mode_zoom_action_->setShortcut(tr("Ctrl+5"));
//    mouse_mode_zoom_action_->setStatusTip(tr("Zoom to a window"));
//    connect(mouse_mode_zoom_action_, SIGNAL(triggered()),
//            &model_, SLOT(mouseModeZoomAction()));

    view_zoom_in_action_ = new QAction(view_zoom_in_icon_,  tr("Zoom &In"),
                                       this);
    view_zoom_in_action_->setObjectName("view_zoom_in_action_");
    view_zoom_in_action_->setStatusTip(tr("Zoom in all channels (All Modes: Shift+MouseWheel or Ctrl+MouseWheel)"));
    connect(view_zoom_in_action_, SIGNAL(triggered()),
            &model_, SLOT(viewZoomInAction()));

    view_zoom_out_action_= new QAction(view_zoom_out_icon_,  tr("Zoom &Out"),
                                       this);
    view_zoom_out_action_->setObjectName("view_zoom_out_action_");
    view_zoom_out_action_->setStatusTip(tr("Zoom out all channels (All Modes: Shift+MouseWheel or Ctrl+MouseWheel)"));
    connect(view_zoom_out_action_, SIGNAL(triggered()),
            &model_, SLOT(viewZoomOutAction()));

    view_auto_scale_action_= new QAction(view_auto_scale_icon_,
                                         tr("&Auto Scale"), this);
    view_auto_scale_action_->setObjectName("view_auto_scale_action_");
    view_auto_scale_action_->setStatusTip(tr("Autoscale all channels"));
    connect(view_auto_scale_action_, SIGNAL(triggered()),
            &model_, SLOT(viewAutoScaleAction()));

    view_go_to_action_= new QAction(tr("&Go To..."), this);
    view_go_to_action_->setShortcut(tr("Ctrl+G"));
    view_go_to_action_->setStatusTip(tr("Go to a specified point of the signal file"));
    connect(view_go_to_action_, SIGNAL(triggered()),
            &model_, SLOT(viewGoToAction()));

    view_show_and_select_next_event_action_= new QAction(tr("&Next Event..."), this);
    view_show_and_select_next_event_action_->setShortcut(tr("Ctrl+Right"));
    view_show_and_select_next_event_action_->setStatusTip(tr("Jumps to the next specified event and selects it"));
    connect(view_show_and_select_next_event_action_, SIGNAL(triggered()),
            &model_, SLOT(viewShowAndSelectNextEventAction()));

    view_show_and_select_previous_event_action_= new QAction(tr("&Previous Event..."), this);
    view_show_and_select_previous_event_action_->setShortcut(tr("Ctrl+Left"));
    view_show_and_select_previous_event_action_->setStatusTip(tr("Jumps to the previous specified event and selects it"));
    connect(view_show_and_select_previous_event_action_, SIGNAL(triggered()),
            &model_, SLOT(viewShowAndSelectPreviousEventAction()));

    view_show_events_of_selected_type_action_= new QAction(tr("Hide Events of other Type"), this);
    view_show_events_of_selected_type_action_->setStatusTip(tr("Only shows events which are of the same type as the selected one"));
    connect(view_show_events_of_selected_type_action_, SIGNAL(triggered()),
            &model_, SLOT(viewShowEventsOfSelectedTypeAction()));

    view_fit_to_event_action_ = new QAction(tr("Fit View to Selected Event"), this);
    view_fit_to_event_action_->setStatusTip(tr("Fits the view to the selected event"));
    connect(view_fit_to_event_action_, SIGNAL(triggered()),
            &model_, SLOT(viewFitToEventAction()));


    options_channels_action_= new QAction(options_channels_icon_,
                                    tr("Channe&ls..."), this);
    options_channels_action_->setObjectName("options_channels_action_");
    options_channels_action_->setShortcut(tr("Ctrl+L"));
    options_channels_action_->setStatusTip(tr("Select the channels that should be shown"));
    connect(options_channels_action_, SIGNAL(triggered()),
            &model_, SLOT(optionsChannelsAction()));

    options_show_events_action_= new QAction(options_show_events_icon_,
                                    tr("&Events..."), this);
    options_show_events_action_->setObjectName("options_show_events_action_");
    options_show_events_action_->setShortcut(tr("Ctrl+E"));
    options_show_events_action_->setStatusTip(tr("Select the events that should be shown"));
    connect(options_show_events_action_, SIGNAL(triggered()),
            &model_, SLOT(optionsShowEventsAction()));

    options_show_settings_action_= new QAction(options_show_settings_icon_,
                                    tr("&Preferences..."), this);
    options_show_settings_action_->setObjectName("options_show_settings_action_");
    options_show_settings_action_->setShortcut(tr("Ctrl+P"));
    options_show_settings_action_->setStatusTip(tr("Show the preferences dialog"));
    connect(options_show_settings_action_, SIGNAL(triggered()),
            &model_, SLOT(optionsShowSettingsAction()));

    help_log_action_= new QAction(tr("&Log..."), this);
    help_log_action_->setObjectName("help_log_action_");
    help_log_action_->setStatusTip(tr("Log"));
    connect(help_log_action_, SIGNAL(triggered()),
            &model_, SLOT(helpLogAction()));

    help_about_action_= new QAction(help_about_icon_,
                                    tr("&About SigViewer..."), this);
    help_about_action_->setObjectName("help_about_action_");
    help_about_action_->setStatusTip(tr("About SigViewer"));
    connect(help_about_action_, SIGNAL(triggered()),
            &model_, SLOT(helpAboutAction()));
}

// init tool bars
void MainWindow::initToolBars()
{
    file_toolbar_ = addToolBar(tr("File"));
    file_toolbar_->setIconSize(QSize(22, 22));
    file_toolbar_->addAction(file_open_action_);
    file_toolbar_->addAction(file_save_action_);
    file_toolbar_->addAction(file_save_as_action_);
    file_toolbar_->addAction(file_info_action_);
    file_toolbar_->addAction(file_close_action_);

    mouse_mode_toolbar_ = addToolBar(tr("Mode"));
    mouse_mode_toolbar_->setIconSize(QSize(22, 22));
    mouse_mode_toolbar_->addAction(mouse_mode_new_action_);
    mouse_mode_toolbar_->addAction(mouse_mode_pointer_action_);
    mouse_mode_toolbar_->addAction(mouse_mode_hand_action_);
    mouse_mode_toolbar_->addAction(mouse_mode_shift_signal_action_);
    //mouse_mode_toolbar_->addAction(mouse_mode_zoom_action_);

    edit_toolbar_ = addToolBar(tr("Edit"));
    edit_toolbar_->setIconSize(QSize(22, 22));
    edit_toolbar_->addAction(edit_to_all_channels_action_);
    edit_toolbar_->addAction(edit_copy_to_channels_action_);
    edit_toolbar_->addAction(edit_delete_action_);
    edit_toolbar_->addAction(edit_change_channel_action_);
    edit_toolbar_->addAction(edit_change_type_action_);

    option_toolbar_ = addToolBar(tr("Options"));
    option_toolbar_->setIconSize(QSize(22, 22));
    option_toolbar_->addAction(options_channels_action_);
    option_toolbar_->addAction(options_show_events_action_);
    //option_toolbar_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //option_toolbar_->addAction(options_show_settings_action_);

    secs_per_page_combobox_ = new QComboBox();
    secs_per_page_combobox_->setToolTip(tr("Seconds per Page"));
    secs_per_page_combobox_->setEditable(true);
    secs_per_page_combobox_->setInsertPolicy(QComboBox::NoInsert);
    secs_per_page_combobox_->setValidator(new QDoubleValidator(1.0, 10000.0, 1,
                                                               this));
    secs_per_page_combobox_->insertItem(1, "1");
    secs_per_page_combobox_->insertItem(2, "3");
    secs_per_page_combobox_->insertItem(3, "5");
    secs_per_page_combobox_->insertItem(4, "10");	// important for BCI
    secs_per_page_combobox_->insertItem(5, "20");	// important for sleep staging
    secs_per_page_combobox_->insertItem(6, "30");	// important for sleep staging
    secs_per_page_combobox_->insertItem(7, "50");
    secs_per_page_combobox_->insertItem(8, "100");
    secs_per_page_combobox_->insertItem(9, "300");
    secs_per_page_combobox_->insertItem(10, "500");
    secs_per_page_combobox_->insertItem(11, "1000");
    secs_per_page_combobox_->insertItem(12, tr("whole"));

    signals_per_page_combobox_ = new QComboBox();
    signals_per_page_combobox_->setToolTip("Signals per Page");
    signals_per_page_combobox_->setStatusTip(tr("Number of shown signals per page"));
    signals_per_page_combobox_->setEditable(true);
    signals_per_page_combobox_->setInsertPolicy(QComboBox::NoInsert);
    signals_per_page_combobox_->setValidator(new QDoubleValidator(1.0, 30.0, 1,
                                                                  this));
    signals_per_page_combobox_->insertItem(1, "1");
    signals_per_page_combobox_->insertItem(2, "2");
    signals_per_page_combobox_->insertItem(3, "3");
    signals_per_page_combobox_->insertItem(4, "5");
    signals_per_page_combobox_->insertItem(5, "7");
    signals_per_page_combobox_->insertItem(6, "10");
    signals_per_page_combobox_->insertItem(7, "15");
    signals_per_page_combobox_->insertItem(8, "20");
    signals_per_page_combobox_->insertItem(9, "25");
    signals_per_page_combobox_->insertItem(10, "30");
    signals_per_page_combobox_->insertItem(11, "all");

    navigation_toolbar_ = addToolBar(tr("Navigation"));
    navigation_toolbar_->setIconSize(QSize(22, 22));
    navigation_toolbar_->addWidget(secs_per_page_combobox_);
    navigation_toolbar_->addWidget(signals_per_page_combobox_);
    navigation_toolbar_->addAction(view_zoom_in_action_);
    navigation_toolbar_->addAction(view_zoom_out_action_);
    navigation_toolbar_->addAction(view_auto_scale_action_);
    //navigation_toolbar_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    connect(secs_per_page_combobox_, SIGNAL(activated(const QString&)),
            &model_, SLOT(secsPerPageChanged(const QString&)));
    connect(signals_per_page_combobox_, SIGNAL(activated(const QString&)),
            &model_, SLOT(signalsPerPageChanged(const QString&)));
    connect(secs_per_page_combobox_->lineEdit(), SIGNAL(returnPressed()),
            this, SLOT(secsPerPageReturnPressed()));
    connect(signals_per_page_combobox_->lineEdit(), SIGNAL(returnPressed()),
            this, SLOT(signalsPerPageReturnPressed()));
}

// secs per page return pressed
void MainWindow::secsPerPageReturnPressed()
{
    model_.secsPerPageChanged(secs_per_page_combobox_->currentText());
}

// signals per page return pressed
void MainWindow::signalsPerPageReturnPressed()
{
    model_.signalsPerPageChanged(signals_per_page_combobox_->currentText());
}

// init menus
void MainWindow::initMenus()
{
    file_menu_ = menuBar()->addMenu(tr("&File"));
    file_menu_->addAction(file_open_action_);
    file_menu_->addAction(file_save_action_);
    file_menu_->addAction(file_save_as_action_);
    file_menu_->addSeparator();
    file_menu_->addAction(file_export_events_action_);
    file_menu_->addAction(file_import_events_action_);
    file_menu_->addSeparator();
    file_menu_->addAction(file_info_action_);
    file_menu_->addSeparator();
    file_recent_files_menu_ = file_menu_->addMenu(tr("&Recent Files"));
    connect(file_recent_files_menu_, SIGNAL(aboutToShow()),
            &model_, SLOT(recentFileMenuAboutToShow()));
    connect(file_recent_files_menu_, SIGNAL(triggered(QAction*)),
            &model_, SLOT(recentFileActivated(QAction*)));
    file_menu_->addSeparator();
    file_menu_->addAction(file_close_action_);
    file_menu_->addSeparator();
    file_menu_->addAction(file_exit_action_);

    edit_menu_ = menuBar()->addMenu(tr("&Edit"));
    edit_menu_->addAction(undo_action_);
    edit_menu_->addAction(redo_action_);
    edit_menu_->addSeparator();
    edit_menu_->addAction(edit_to_all_channels_action_);
    edit_menu_->addAction(edit_copy_to_channels_action_);
    edit_menu_->addAction(edit_delete_action_);
    edit_menu_->addAction(edit_change_channel_action_);
    edit_menu_->addAction(edit_change_type_action_);
    edit_menu_->addSeparator();
    edit_menu_->addAction(edit_event_table_action_);

    mouse_mode_menu_ = menuBar()->addMenu(tr("&Mode"));
    mouse_mode_menu_->addAction(mouse_mode_new_action_);
    mouse_mode_menu_->addAction(mouse_mode_pointer_action_);
    mouse_mode_menu_->addAction(mouse_mode_hand_action_);
    mouse_mode_menu_->addAction(mouse_mode_shift_signal_action_);
//    mouse_mode_menu_->addAction(mouse_mode_zoom_action_);

    view_menu_ = menuBar()->addMenu(tr("&View"));
    view_menu_->addAction(view_zoom_in_action_);
    view_menu_->addAction(view_zoom_out_action_);
    view_menu_->addAction(view_auto_scale_action_);
    view_menu_->addSeparator();
    view_menu_->addAction(view_go_to_action_);
    view_menu_->addAction(view_show_and_select_next_event_action_);
    view_menu_->addAction(view_show_and_select_previous_event_action_);
    view_menu_->addAction(view_show_events_of_selected_type_action_);
    view_menu_->addAction(view_fit_to_event_action_);
    view_menu_->addSeparator();
    view_menu_->addAction(undo_view_action_);
    view_menu_->addAction(redo_view_action_);

    tools_menu_ = menuBar()->addMenu(tr("&Tools"));
    tools_menu_->addAction(calculate_mean_action_);
    tools_menu_->addAction(calculate_frequency_spectrum_action_);
    tools_menu_->addAction(calculate_erd_ers_map_action_);

    options_menu_ = menuBar()->addMenu(tr("&Options"));
    options_menu_->addAction(options_channels_action_);
    options_menu_->addAction(options_show_events_action_);
    options_menu_->addAction(options_show_settings_action_);

    help_menu_ = menuBar()->addMenu(tr("&Help"));
    help_menu_->addAction(help_log_action_);
    help_menu_->addSeparator();
    help_menu_->addAction(help_about_action_);
}

// close event
void MainWindow::closeEvent(QCloseEvent* close_event)
{
    if (model_.getState() == MainWindowModel::STATE_EXIT)
    {
        close_event->accept();
    }
    else
    {
        close_event->ignore();
        model_.fileExitAction();
    }
}

void MainWindow::dropEvent (QDropEvent* event)
{
    if (event->mimeData()->hasText())
    {
        QUrl url (event->mimeData()->text());
        event->acceptProposedAction();
        model_.openFile(url.path());
    } else if (event->mimeData()->hasUrls())
    {
        QUrl url (event->mimeData()->urls().first().toLocalFile());
        event->acceptProposedAction();
        model_.openFile(url.path());
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText() || event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}



// load settings
void MainWindow::loadSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(800, 600)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
}

// save settings
void MainWindow::saveSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

// set file save enabled
void MainWindow::setFileSaveEnabled(bool enabled)
{
    file_save_action_->setEnabled(enabled);
}

// set file save as enabled
void MainWindow::setFileSaveAsEnabled(bool enabled)
{
    file_save_as_action_->setEnabled(enabled);
}

// set file export events enabled
void MainWindow::SetFileExportEventsEnabled(bool enabled)
{
    file_export_events_action_->setEnabled(enabled);
}

// set file import events enabled
void MainWindow::SetFileImportEventsEnabled(bool enabled)
{
    file_import_events_action_->setEnabled(enabled);
}

// set file close enabled
void MainWindow::setFileCloseEnabled(bool enabled)
{
    file_close_action_->setEnabled(enabled);
}

// set file info enabled
void MainWindow::setFileInfoEnabled(bool enabled)
{
    file_info_action_->setEnabled(enabled);
}

// set edit to all channels enabled
void MainWindow::setEditToAllChannelsEnabled(bool enabled)
{
    edit_to_all_channels_action_->setEnabled(enabled);
}

// set edit copy to channels enabled
void MainWindow::setEditCopyToChannelsEnabled(bool enabled)
{
    edit_copy_to_channels_action_->setEnabled(enabled);
}

// set edit delete enabled
void MainWindow::setEditDeleteEnabled(bool enabled)
{
    edit_delete_action_->setEnabled(enabled);
}

// set edit change channel enabled
void MainWindow::setEditChangeChannelEnabled(bool enabled)
{
    edit_change_channel_action_->setEnabled(enabled);
}

// set edit change type enabled
void MainWindow::setEditChangeTypeEnabled(bool enabled)
{
    edit_change_type_action_->setEnabled(enabled);
}

// set edit event table enabled
void MainWindow::setEditEventTableEnabled(bool enabled)
{
    edit_event_table_action_->setEnabled(enabled);
}

// set mouse mode new anabled
void MainWindow::setMouseModeNewEnabled(bool enabled)
{
    mouse_mode_new_action_->setEnabled(enabled);
}

// set mouse mode pointer anabled
void MainWindow::setMouseModePointerEnabled(bool enabled)
{
    mouse_mode_pointer_action_->setEnabled(enabled);
}

// set mouse mode hand enabled
void MainWindow::setMouseModeHandEnabled(bool enabled)
{
    mouse_mode_hand_action_->setEnabled(enabled);
}

// set mouse mode shift signal enabled
void MainWindow::setMouseModeShiftSignalEnabled(bool enabled)
{
    mouse_mode_shift_signal_action_->setEnabled(enabled);
}

// set mouse mode zoom enabled
//void MainWindow::setMouseModeZoomEnabled(bool enabled)
//{
//    mouse_mode_zoom_action_->setEnabled(enabled);
//}

// set options channels enabled
void MainWindow::setOptionsChannelsEnabled(bool enabled)
{
    options_channels_action_->setEnabled(enabled);
}

// set options show events enabled
void MainWindow::setOptionsShowEventsEnabled(bool enabled)
{
    options_show_events_action_->setEnabled(enabled);
}

// set view zoom in enabled
void MainWindow::setViewZoomInEnabled(bool enabled)
{
    view_zoom_in_action_->setEnabled(enabled);
}

// set view zoom out enabled
void MainWindow::setViewZoomOutEnabled(bool enabled)
{
    view_zoom_out_action_->setEnabled(enabled);
}

// set view auto scale enabled
void MainWindow::setViewAutoScaleEnabled(bool enabled)
{
    view_auto_scale_action_->setEnabled(enabled);
}

// set view go to enabled
void MainWindow::setViewGoToEnabled(bool enabled)
{
    view_go_to_action_->setEnabled(enabled);
}

void MainWindow::setViewFitToEvent(bool enabled)
{
    view_fit_to_event_action_->setEnabled(enabled);
}

void MainWindow::setViewHideEventsOfOtherType(bool enabled)
{
    view_show_events_of_selected_type_action_->setEnabled(enabled);
}

// set view go to next previous event enabled
void MainWindow::setViewGoToNextPreviousEventEnabled(bool enabled)
{
    view_show_and_select_next_event_action_->setEnabled(enabled);
    view_show_and_select_previous_event_action_->setEnabled(enabled);
}

// set secs per page enabled
void MainWindow::setSecsPerPageEnabled(bool enabled)
{
    secs_per_page_combobox_->setEnabled(enabled);
}

// set signals per page enabled
void MainWindow::setSignalsPerPageEnabled(bool enabled)
{
    signals_per_page_combobox_->setEnabled(enabled);
}

// show file close dialog
bool MainWindow::showFileCloseDialog(const QString& file_name)
{
    int res;
    res =  QMessageBox::question(this, tr("Really close?"),
                tr("Changes in '%1' are not saved!!").arg(file_name) + "\n" +
                tr("Really close?"), tr("Yes"),  tr("No"));
    return (res == 0);
}

// show file open dialog
QString MainWindow::showOpenDialog(const QString& path,
                                   const QString& extensions)
{
    QString extension_selection = tr("Signal files (%1)").arg(extensions);
    QStringList ext_list = extensions.split (" ");//, QString::SkipEmptyParts, extensions);
    for (QStringList::iterator it = ext_list.begin();
         it != ext_list.end();
         it++)
    {
        extension_selection += ";; " + *it +" (" + *it + ")";
    }
    extension_selection += ";; *.* (*.*)";
    return QFileDialog::getOpenFileName(this, tr("Chose signal file to open"),
                                        path, extension_selection);
}

// show import dialog
QString MainWindow::showImportDialog(const QString& path,
                                     const QString& extensions)
{
    QString extension_selection;
    QStringList ext_list = extensions.split(" ");
    for (QStringList::iterator it = ext_list.begin();
         it != ext_list.end();
         it++)
    {
        extension_selection += *it + '\n';
    }
    extension_selection += "*.*";
    return QFileDialog::getOpenFileName(this, tr("Chose signal file to import"),
                                        path,extension_selection);
}

// show help about dialog
void MainWindow::showHelpAboutDialog()
{
    QMessageBox about_message_box(tr("About SigViewer"),
                                  tr("<b><nobr>SigViewer Version 0.3.0</nobr></b><br>\
                                      <br>\
									  Thomas Brunner, Christoph Eibel, Clemens Brunner, Alois Schl&ouml;gl, Oliver Terbu<br>\
									  <br>\
									  <nobr>Graz University of Technology</nobr>"),
                                  QMessageBox::NoIcon,
                                  QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton,
                                  this);
    about_message_box.setIconPixmap(QPixmap(":images/sigviewer128.png"));
    about_message_box.exec();
}

// show error read dialog
void MainWindow::showErrorReadDialog(const QString& file_name)
{
    QMessageBox::critical(this, tr("Error reading file"),
                          tr("Invalid file: %1").arg(file_name));
}

// show error write dialog
void MainWindow::showErrorWriteDialog(const QString& file_name)
{
        QMessageBox::critical(this, tr("Error writing file"),
                              tr("Writing failed: %1").arg(file_name));
}

// show file export dialog
QString MainWindow::showExportDialog(const QString& path,
                                     const QString& extensions)
{
    //QString extension_selection = tr("Signal files (%1)").arg(extensions);
    QString extension_selection;
    QStringList ext_list = extensions.split(" ");
    for (QStringList::iterator it = ext_list.begin();
         it != ext_list.end();
         it++)
    {
        extension_selection += *it + '\n';
    }
    extension_selection += "*.*";
    QString selected_extension;
    QString file_name = QFileDialog::getSaveFileName(this,
                                            tr("Chose signal file to export"),
                                            path, extension_selection,
                                            &selected_extension);
    if (selected_extension != "*.*")
    {
        selected_extension = selected_extension.mid(1);
        if(!file_name.endsWith(selected_extension))
        {
            file_name += selected_extension;
        }
    }
    return file_name;
}

// show file save as dialog
QString MainWindow::showSaveAsDialog(const QString& path,
                                     const QString& extensions)
{
    QString extension_selection;
    QStringList ext_list = extensions.split(" ");
    for (QStringList::iterator it = ext_list.begin();
         it != ext_list.end();
         it++)
    {
        if (*it != "*.evt")
        {
            extension_selection += *it + '\n';
        }
    }
    extension_selection += "*.*";
    QString selected_extension;
    QString file_name = QFileDialog::getSaveFileName(this,
                                        tr("Chose signal file to save as"),
                                        path, extension_selection,
                                        &selected_extension);
    if (selected_extension != "*.*")
    {
        selected_extension = selected_extension.mid(1);
        if(!file_name.endsWith(selected_extension))
        {
            file_name += selected_extension;
        }
    }
    return file_name;
}

// show overwrite dialog
bool MainWindow::showOverwriteDialog(const QString& file_name)
{
    int res=  QMessageBox::question(this, tr("Really overwrite?"),
                tr("File '%1' alreay exists!!").arg(file_name) + "\n" +
                tr("Really overwrite?"), tr("Yes"),  tr("No"));
    return (res == 0);
}

// show inconsistent events dialog
void MainWindow::showInconsistentEventsDialog()
{
    QMessageBox::warning(this, tr("Inconsistent Events"),
                         tr("Inconsistant event positions or channels!!"),
                         QMessageBox::Ok, QMessageBox::NoButton);
}

// set recent files
void MainWindow::setRecentFiles(const QStringList& recent_file_list)
{
    file_recent_files_menu_->clear();
    for (QStringList::const_iterator it = recent_file_list.begin();
         it != recent_file_list.end();
         it++)
    {
        file_recent_files_menu_->addAction(*it);
    }
}

// set mouse mode
void MainWindow::setMouseMode(SignalBrowserMode mode)
{
    switch (mode)
    {
        case MODE_NEW:
            mouse_mode_new_action_->setChecked(true);
            break;
        case MODE_POINTER:
            mouse_mode_pointer_action_->setChecked(true);
            break;
        case MODE_HAND:
            mouse_mode_hand_action_->setChecked(true);
            break;
        case MODE_SHIFT_SIGNAL:
            mouse_mode_shift_signal_action_->setChecked(true);
            break;
//        case SignalBrowserModel::MODE_ZOOM:
//            mouse_mode_zoom_action_->setChecked(true);
//            break;
    }
}

// set signals per page
void MainWindow::setSignalsPerPage(float64 signals_per_page)
{
    if (signals_per_page < 0)
    {
        // all
        signals_per_page_combobox_->setCurrentIndex(signals_per_page_combobox_->count() - 1);
    }
    else
    {
        // TODO : works not correctly
        signals_per_page_combobox_->setEditText(QString::number(signals_per_page));
    }
}

// set secs per page
void MainWindow::setSecsPerPage(float64 secs_per_page)
{
    QString tmp = QString::number(secs_per_page);
    setSecsPerPage(tmp);
}

void MainWindow::setSecsPerPage(const QString& secs_per_page)
{
    int32 index = secs_per_page_combobox_->findText(secs_per_page);
    if (index != -1)
    {
        secs_per_page_combobox_->setCurrentIndex(index);
    }
    else
    {
        // TODO : works not correctly
        signals_per_page_combobox_->clearFocus();
        secs_per_page_combobox_->setEditText(secs_per_page);
    }
}


// add Action
bool MainWindow::addActionTo(QMenu* menu, const QString& action_name)
{
    QAction* action = findChild<QAction*>(action_name);
    if (!action)
    {
        model_.getLogStream() << "MainWindow::addActionTo Error: "
                              << " unknow Action: '" << action_name
                              << "'\n";
        return false;
    }
    menu->addAction(action);
    return true;
}

// set status bar signal length
void MainWindow::setStatusBarSignalLength(float64 length)
{
    QString tmp = length < 0 ? "----.-" : QString("%1").arg(length, 0, 'f', 1);
    status_bar_signal_length_label_->setText(tr("Length: %1 s").arg(tmp));
    status_bar_signal_length_label_->setMinimumWidth(status_bar_signal_length_label_->sizeHint().width() + 10);
}

// set status bar nr channels
void MainWindow::setStatusBarNrChannels(int32 nr_channels)
{
    QString tmp = nr_channels < 0 ? "---" : QString("%1").arg(nr_channels, 3);
    status_bar_nr_channels_label_->setText(tr("Channels: %1").arg(tmp));
    status_bar_nr_channels_label_->setMinimumWidth(status_bar_nr_channels_label_->sizeHint().width() + 10);
}

// set status bar nr trials
void MainWindow::setStatusBarNrTrials(int32 nr_trials)
{
    QString tmp = nr_trials < 0 ? "---" : QString("%1").arg(nr_trials, 3);
    status_bar_nr_trials_label_->setText(tr("Trials: %1").arg(tmp));
    status_bar_nr_trials_label_->setMinimumWidth(status_bar_nr_trials_label_->sizeHint().width() + 10);
}

} // namespace BioSig_
