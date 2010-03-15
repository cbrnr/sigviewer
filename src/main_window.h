// main_window.h

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "base/user_types.h"

#include <QMainWindow>
#include <QIcon>

class QCloseEvent;
class QAction;
class QComboBox;
class QMenu;
class QLabel;

namespace BioSig_
{

class ApplicationContext;
class MainWindowModel;
class GUIActionManager;

// main window
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow (ApplicationContext& application_context);
    ~MainWindow ();

    void loadSettings();
    void saveSettings();

    // enable,disable actions
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
    void setStatusBarSignalLength(float64 length);
    void setStatusBarNrChannels(int32 nr_channels);

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
    void toggleStatusBar (bool visible);

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
    GUIActionManager& action_manager_;

    QMenu* file_menu_;
    QMenu* file_recent_files_menu_;
    QMenu* edit_menu_;
    QMenu* mouse_mode_menu_;
    QMenu* view_menu_;
    QMenu* view_toolbar_views_menu_;
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
    QAction* calculate_erd_ers_map_action_;
    QAction* undo_view_action_;
    QAction* redo_view_action_;
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
};

} // namespace BioSig_

#endif

