// © SigViewer developers
//
// License: GPL-3.0


#ifndef MAIN_WINDOW_MODEL_H
#define MAIN_WINDOW_MODEL_H

#include "signal_visualisation_model.h"
#include "event_view.h"
#include "file_context.h"
#include "file_handling/channel_manager.h"
#include "file_handling/event_manager.h"
#include "tab_context.h"

#include <QObject>
#include <QStringList>
#include <QTabWidget>
#include <QMap>


class QAction;

namespace sigviewer
{

class ApplicationContext;
class MainWindow;
class SignalBrowserView;

class MainWindowModel : public QObject
{
    Q_OBJECT
public:
    MainWindowModel (QSharedPointer<ApplicationContext> application_context);

    ~MainWindowModel ();

    QSharedPointer<SignalVisualisationModel> createSignalVisualisation (QString const& title,
                                                                        ChannelManager const& channel_manager);

    QSharedPointer<SignalVisualisationModel> createSignalVisualisationOfFile (QSharedPointer<FileContext> file_ctx);

    void closeCurrentFileTabs ();

    QSharedPointer<SignalVisualisationModel> getCurrentSignalVisualisationModel ();

    QSharedPointer<EventView> getCurrentEventView ();

public slots:
    void tabChanged (int tab_index);

    void closeTab (int tab_index);

    /// Propagate overview visibility to the currently open signal browser view.
    void setOverviewVisible (bool visible);

private slots:
    void recentFileActivated(QAction* recent_file_action);

    void recentFileMenuAboutToShow();

    void resetCurrentFileName (QString const& file_name);

private:
    void loadSettings();

    void saveSettings();

    int createSignalVisualisationImpl (ChannelManager const& channel_manager,
                                       QSharedPointer<EventManager> event_manager);

    void storeAndInitTabContext (QSharedPointer<TabContext> context, int tab_index);

    static int const NUMBER_RECENT_FILES_;

    QSharedPointer<ApplicationContext> application_context_;
    MainWindow* main_window_;
    QTabWidget* tab_widget_;
    SignalBrowserView* current_signal_browser_view_;
    QStringList recent_file_list_;
    std::map<int, QSharedPointer<SignalVisualisationModel> > browser_models_;
    QMap<int, QSharedPointer<EventView> > event_views_;
    std::map<int, QSharedPointer<TabContext> > tab_contexts_;

    // not allowed
    MainWindowModel(const MainWindowModel&);
    const MainWindowModel& operator=(const MainWindowModel&);
};

}

#endif // MAIN_WINDOW_MODEL_H

