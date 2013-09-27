// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


#ifndef MAIN_WINDOW_MODEL_IMPL_H
#define MAIN_WINDOW_MODEL_IMPL_H

#include "base/sigviewer_user_types.h"
#include "file_context.h"
#include "tab_context.h"
#include "file_handling/event_manager.h"
#include "file_handling/channel_manager.h"
#include "gui/signal_visualisation_model.h"
#include "gui/main_window_model.h"
#include "gui/application_context.h"

#include <QObject>
#include <QStringList>
#include <QString>
#include <QSharedPointer>
#include <QTabWidget>
#include <QMap>

#include <map>

class QAction;

namespace SigViewer_
{

class MainWindow;

//-------------------------------------------------------------------------
class MainWindowModelImpl : public QObject, public MainWindowModel
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    MainWindowModelImpl (QSharedPointer<ApplicationContext> application_context);

    //-------------------------------------------------------------------------
    virtual  ~MainWindowModelImpl ();

    //-------------------------------------------------------------------------
    virtual QSharedPointer<SignalVisualisationModel> createSignalVisualisation (QString const& title,
                                                                                ChannelManager const& channel_manager);

    //-------------------------------------------------------------------------
    virtual QSharedPointer<SignalVisualisationModel> createSignalVisualisationOfFile (QSharedPointer<FileContext> file_ctx);

    //-------------------------------------------------------------------------
    virtual void closeCurrentFileTabs ();

    //-------------------------------------------------------------------------
    virtual QSharedPointer<SignalVisualisationModel> getCurrentSignalVisualisationModel ();

    //-------------------------------------------------------------------------
    virtual QSharedPointer<EventView> getCurrentEventView ();

public slots:
    //-------------------------------------------------------------------------
    void tabChanged (int tab_index);

    //-------------------------------------------------------------------------
    void closeTab (int tab_index);

private slots:
    //-------------------------------------------------------------------------
    void recentFileActivated(QAction* recent_file_action);

    //-------------------------------------------------------------------------
    void recentFileMenuAboutToShow();

    //-------------------------------------------------------------------------
    void resetCurrentFileName (QString const& file_name);

private:    
    //-------------------------------------------------------------------------
    void loadSettings();

    //-------------------------------------------------------------------------
    void saveSettings();

    //-------------------------------------------------------------------------
    int createSignalVisualisationImpl (ChannelManager const& channel_manager,
                                       QSharedPointer<EventManager> event_manager);

    //-------------------------------------------------------------------------
    void storeAndInitTabContext (QSharedPointer<TabContext> context, int tab_index);

    static int const NUMBER_RECENT_FILES_;

    QSharedPointer<ApplicationContext> application_context_;
    MainWindow* main_window_;
    QTabWidget* tab_widget_;
    QStringList recent_file_list_;
    std::map<int, QSharedPointer<SignalVisualisationModel> > browser_models_;
    QMap<int, QSharedPointer<EventView> > event_views_;
    std::map<int, QSharedPointer<TabContext> > tab_contexts_;

    //-------------------------------------------------------------------------
    // not allowed
    MainWindowModelImpl(const MainWindowModelImpl&);
    const MainWindowModelImpl& operator=(const MainWindowModelImpl&);

};

} // namespace SigViewer_

#endif

