// main_window.h

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "base/user_types.h"
#include "signal_browser/signal_browser_model_4.h"

#include <QMainWindow>
#include <QIcon>

class QCloseEvent;
class QAction;
class QComboBox;
class QMenu;
class QLabel;

namespace BioSig_
{

class MainWindowModel;

// main window
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(MainWindowModel& model);
    ~MainWindow();

    void loadSettings();
    void saveSettings();

    // enable,disable actions
    void setFileSaveEnabled(bool enabled);
    void setFileSaveAsEnabled(bool enabled);
    void SetFileExportEventsEnabled(bool enabled);
    void SetFileImportEventsEnabled(bool enabled);
    void setFileCloseEnabled(bool enabled);
    void setFileInfoEnabled(bool enabled);
    void setEditToAllChannelsEnabled(bool enabled);
    void setEditCopyToChannelsEnabled(bool enabled);
    void setEditDeleteEnabled(bool enabled);
    void setEditChangeChannelEnabled(bool enabled);
    void setEditChangeTypeEnabled(bool enabled);
    void setEditEventTableEnabled(bool enabled);
    void setMouseModeNewEnabled(bool enabled);
    void setMouseModeHandEnabled(bool enabled);
    void setMouseModeShiftSignalEnabled(bool enabled);
    void setMouseModePointerEnabled(bool enabled);
//    void setMouseModeZoomEnabled(bool enabled);
    void setOptionsChannelsEnabled(bool enabled);
    void setOptionsShowEventsEnabled(bool enabled);
    void setViewZoomInEnabled(bool enabled);
    void setViewZoomOutEnabled(bool enabled);
    void setViewAutoScaleEnabled(bool enabled);
    void setViewGoToEnabled(bool  enabled);
    void setViewFitToEvent(bool enabled);
    void setViewHideEventsOfOtherType(bool enabled);
    void setViewGoToNextPreviousEventEnabled(bool enabled);
    void setSecsPerPageEnabled(bool enabled);
    void setSignalsPerPageEnabled(bool enabled);
    void setSignalsPerPage(float64 signals_per_page);
    void setSecsPerPage(float64 secs_per_page);
    void setSecsPerPage(const QString& secs_per_page);
    void setMouseMode(SignalBrowserModel::Mode mode);
    void setStatusBarSignalLength(float64 length);
    void setStatusBarNrChannels(int32 nr_channels);
    void setStatusBarNrTrials(int32 nr_trials);

    // do actions
    bool showFileCloseDialog(const QString& file_name);
    QString showOpenDialog(const QString& path, const QString& extensions);
    void showHelpAboutDialog();
    void showErrorReadDialog(const QString& file_name);
    void showErrorWriteDialog(const QString& file_name);
    QString showExportDialog(const QString& path, const QString& extensions);
    bool showOverwriteDialog(const QString& file_name);
    QString showSaveAsDialog(const QString& path, const QString& extensions);
    QString showImportDialog(const QString& path, const QString& extensions);
    void showInconsistentEventsDialog();
    void setRecentFiles(const QStringList& recent_file_list);
    bool addActionTo(QMenu* menu, const QString& action_name);

protected:

    virtual void closeEvent(QCloseEvent* close_event);
    virtual void dropEvent (QDropEvent* event);
    virtual void dragEnterEvent(QDragEnterEvent *event);

private slots:

    void secsPerPageReturnPressed();
    void signalsPerPageReturnPressed();

private:

    // not allowed
    MainWindow();
    MainWindow(const MainWindow&);

    const MainWindow& operator=(const MainWindow&);

    void initIconSets();
    void initActions();
    void initToolBars();
    void initMenus();
    void initStatusBar();

    MainWindowModel& model_;

    QMenu* file_menu_;
    QMenu* file_recent_files_menu_;
    QMenu* edit_menu_;
    QMenu* mouse_mode_menu_;
    QMenu* view_menu_;
    QMenu* tools_menu_;
    QMenu* options_menu_;
    QMenu* help_menu_;

    QToolBar* file_toolbar_;
    QToolBar* mouse_mode_toolbar_;
    QToolBar* edit_toolbar_;
    QToolBar* option_toolbar_;
    QToolBar* navigation_toolbar_;

    QAction* calculate_mean_action_;
    QAction* calculate_frequency_spectrum_action_;
    QAction* undo_view_action_;
    QAction* redo_view_action_;
    QAction* undo_action_;
    QAction* redo_action_;
    QAction* file_open_action_;
    QAction* file_save_action_;
    QAction* file_save_as_action_;
    QAction* file_export_events_action_;
    QAction* file_import_events_action_;
    QAction* file_close_action_;
    QAction* file_info_action_;
    QAction* file_exit_action_;
    QAction* edit_to_all_channels_action_;
    QAction* edit_copy_to_channels_action_;
    QAction* edit_delete_action_;
    QAction* edit_change_channel_action_;
    QAction* edit_change_type_action_;
    QAction* edit_event_table_action_;
    QAction* mouse_mode_new_action_;
    QAction* mouse_mode_pointer_action_;
    QAction* mouse_mode_hand_action_;
    QAction* mouse_mode_shift_signal_action_;
    // QAction* mouse_mode_zoom_action_;
    QAction* view_zoom_in_action_;
    QAction* view_zoom_out_action_;
    QAction* view_auto_scale_action_;
    QAction* view_go_to_action_;
    QAction* view_show_and_select_next_event_action_;
    QAction* view_show_and_select_previous_event_action_;
    QAction* view_show_events_of_selected_type_action_;
    QAction* view_fit_to_event_action_;
    QAction* options_channels_action_;
    QAction* options_show_events_action_;
    QAction* help_log_action_;
    QAction* help_about_action_;
    QAction* options_show_settings_action_;

    QIcon file_open_icon_;
    QIcon file_save_icon_;
    QIcon file_save_as_icon_;
    QIcon file_close_icon_;
    QIcon file_info_icon_;
    QIcon file_exit_icon_;
    QIcon edit_to_all_channels_icon_;
    QIcon edit_copy_to_channels_icon_;
    QIcon edit_delete_icon_;
    QIcon edit_change_channel_icon_;
    QIcon edit_change_type_icon_;
    QIcon mouse_mode_new_icon_;
    QIcon mouse_mode_pointer_icon_;
    QIcon mouse_mode_hand_icon_;
    QIcon mouse_mode_shift_signal_icon_;
    QIcon mouse_mode_zoom_icon_;
    QIcon view_zoom_in_icon_;
    QIcon view_zoom_out_icon_;
    QIcon view_auto_scale_icon_;
    QIcon options_channels_icon_;
    QIcon options_show_events_icon_;
    QIcon help_about_icon_;
    QIcon options_show_settings_icon_;

    QComboBox* secs_per_page_combobox_;
    QComboBox* signals_per_page_combobox_;

    QLabel* status_bar_signal_length_label_;
    QLabel* status_bar_nr_channels_label_;
    QLabel* status_bar_nr_trials_label_;
};

} // namespace BioSig_

#endif

