// main_window_model.h

#ifndef MAIN_WINDOW_MODEL_H
#define MAIN_WINDOW_MODEL_H

#include "base/user_types.h"

#include <memory>

#include <QObject>
#include <QStringList>
#include <QString>

class QAction;
class QTextStream;

namespace BioSig_
{

class FileSignalReader;
class MainWindow;
class SignalBrowserModel;
class SignalBrowserView;
class EventTableFileReader;
class EventColorManager;


// main window model
class MainWindowModel : public QObject
{
    Q_OBJECT

public:

    enum State
    {
        STATE_EXIT,
        STATE_FILE_CLOSED,
        STATE_FILE_OPENED,
        STATE_FILE_CHANGED
    };

    enum SelectionState
    {
        SELECTION_STATE_OFF,
        SELECTION_STATE_NONE,
        SELECTION_STATE_ONE_CHANNEL,
        SELECTION_STATE_ALL_CHANNELS
    };

    MainWindowModel();
    ~MainWindowModel();

    QTextStream& getLogStream();
    EventTableFileReader& getEventTableFileReader();
    EventColorManager& getEventColorManager();
    void setMainWindow(MainWindow* main_window);
    MainWindow* getMainWindow();
    void loadSettings();
    void saveSettings();
    State getState();
    void setSelectionState(SelectionState selection_state);
    void setChanged();

public slots:

    // actions
    void undoAction();
    void redoAction();
    void fileOpenAction();
    void fileSaveAction();
    void fileSaveAsAction();
    void fileExportEventsAction();
    void fileImportEventsAction();
    void fileCloseAction();
    void fileInfoAction();
    void fileExitAction();
    void editToAllChannelsAction();
    void editCopyToChannelsAction();
    void editDeleteAction();
    void editChangeChannelAction();
    void editChangeTypeAction();
    void editEventTableAction();
    void mouseModeNewAction();
    void mouseModePointerAction();
    void mouseModeHandAction();
    void mouseModeShiftSignalAction();
    void mouseModeZoomAction();
    void viewZoomInAction();
    void viewZoomOutAction();
    void viewAutoScaleAction();
    void viewGoToAction();
    void viewShowAndSelectNextEventAction();
    void secsPerPageChanged(const QString& secs_per_page);
    void signalsPerPageChanged(const QString& signals_per_page);
    void optionsChannelsAction();
    void optionsShowEventsAction();
    void optionsShowSettingsAction();
    void helpLogAction();
    void helpAboutAction();
    void recentFileActivated(QAction* recent_file_action);
    void pixelPerSecChanged(float64 pixel_per_sec);
    void signalHeightChanged(int32 signal_height);
    void recentFileMenuAboutToShow();

private:

    // not allowed
    MainWindowModel(const MainWindowModel&);
    const MainWindowModel& operator=(const MainWindowModel&);

    void setState(State state);
    void openFile(const QString& file_name);
    bool checkMainWindowPtr(const QString function);
    bool checkNotClosedState(const QString function);
    void channelSelection ();

    MainWindow* main_window_;
    State state_;
    SelectionState selection_state_;
    std::auto_ptr<FileSignalReader> file_signal_reader_;
    std::auto_ptr<SignalBrowserModel> signal_browser_model_;
    std::auto_ptr<SignalBrowserView> signal_browser_;
    std::auto_ptr<EventTableFileReader> event_table_file_reader_;
    std::auto_ptr<EventColorManager> event_color_manager_;
    QStringList recent_file_list_;
    int32 number_recent_files_;
    std::auto_ptr<QTextStream> log_stream_;
    QString log_string_;
    QString secs_per_page_;
    bool overflow_detection_;
};

} // namespace BioSig_

#endif

