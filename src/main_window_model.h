// main_window_model.h

#ifndef MAIN_WINDOW_MODEL_H
#define MAIN_WINDOW_MODEL_H

#include "base/user_types.h"

#include <memory>

#include <QObject>
#include <QStringList>
#include <QString>
#include <QUndoStack>

class QAction;
class QTextStream;

namespace BioSig_
{

class FileSignalReader;
class MainWindow;
#ifndef QT4_PORTED
class SignalBrowserModel;
class SignalBrowser;
#else
namespace PortingToQT4_
{
    class SignalBrowserView;
    class SignalBrowserModel;
}
#endif
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

    //-------------------------------------------------------------------------
    /// puts the given UndoCommand on the UndoStack (the stack cares for
    /// destruction)
    ///
    /// @param command the command which should be executed
    void executeUndoCommand (QUndoCommand* command);

public slots:

    // actions
    void undoAction();
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
#ifndef QT4_PORTED
    std::auto_ptr<SignalBrowserModel> signal_browser_model_;
    std::auto_ptr<SignalBrowser> signal_browser_;
#else
    std::auto_ptr<PortingToQT4_::SignalBrowserModel> signal_browser_model_;
    std::auto_ptr<PortingToQT4_::SignalBrowserView> signal_browser_;
#endif

    std::auto_ptr<EventTableFileReader> event_table_file_reader_;
    std::auto_ptr<EventColorManager> event_color_manager_;
    QStringList recent_file_list_;
    int32 number_recent_files_;
    std::auto_ptr<QTextStream> log_stream_;
    QString log_string_;
    QString secs_per_page_;
    bool overflow_detection_;
    QUndoStack undo_stack_;
};

} // namespace BioSig_

#endif

