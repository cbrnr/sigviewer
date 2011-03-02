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
#include "gui/gui_action_factory.h"
#include "gui/background_processes.h"
#include "gui_impl/commands/open_file_gui_command.h"

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
#include <QApplication>

#include <QUrl>
#include <QDebug>

namespace SigViewer_
{

//----------------------------------------------------------------------------
MainWindow::MainWindow (QSharedPointer<ApplicationContext> application_context)
 : QMainWindow(0)
{
    setWindowTitle (tr("SigViewer"));
    setAcceptDrops (true);
    setWindowIcon(QIcon(":images/sigviewer16.png"));
    initStatusBar();
    initToolBars();
    initMenus (application_context);
    setUnifiedTitleAndToolBarOnMac (true);
    QSettings settings ("SigViewer");
    resize (settings.value("MainWindow/size", QSize(800, 600)).toSize());
}

//-----------------------------------------------------------------------------
void MainWindow::initStatusBar()
{
    connect (&BackgroundProcesses::instance(), SIGNAL(newProcess(QString,int)), SLOT(addBackgroundProcessToStatusBar(QString,int)));
    connect (&BackgroundProcesses::instance(), SIGNAL(processChangedState(QString,int)), SLOT(updateBackgroundProcessonStatusBar(QString,int)));
    connect (&BackgroundProcesses::instance(), SIGNAL(processRemoved(QString)), SLOT(removeBackgroundProcessFromStatusBar(QString)));

    QStatusBar* status_bar = statusBar();
    status_bar->showMessage(tr("Ready"));

    status_bar_signal_length_label_ = new QLabel (this);
    status_bar_nr_channels_label_ = new QLabel (this);
    status_bar_signal_length_label_->setAlignment(Qt::AlignHCenter);
    status_bar_nr_channels_label_->setAlignment(Qt::AlignHCenter);
    status_bar->addPermanentWidget(status_bar_signal_length_label_);
    status_bar->addPermanentWidget(status_bar_nr_channels_label_);
    QSettings settings ("SigViewer");
    status_bar->setVisible (settings.value ("MainWindow/statusbar", true).toBool());
}

//-----------------------------------------------------------------------------
void MainWindow::initToolBars()
{
    view_toolbar_views_menu_ = new QMenu (tr("Toolbars"), this);

    file_toolbar_ = addToolBar(tr("File"));
    view_toolbar_views_menu_->addAction (file_toolbar_->toggleViewAction());
    file_toolbar_->addAction (action("Open..."));
    file_toolbar_->addAction (action("Save"));
    file_toolbar_->addAction (action("Info..."));
    file_toolbar_->addAction (action("Undo"));
    file_toolbar_->addAction (action("Redo"));
    file_toolbar_->addAction (action("Close"));

    mouse_mode_toolbar_ = addToolBar(tr("Mode"));
    view_toolbar_views_menu_->addAction (mouse_mode_toolbar_->toggleViewAction());
    mouse_mode_toolbar_->addAction (action("New Event"));
    mouse_mode_toolbar_->addAction (action("Edit Event"));
    mouse_mode_toolbar_->addAction (action("Scroll"));
    mouse_mode_toolbar_->addAction (action("View Options"));


    view_toolbar_ = addToolBar(tr("View"));
    view_toolbar_views_menu_->addAction (view_toolbar_->toggleViewAction());
    view_toolbar_->addAction(action("Events..."));
    view_toolbar_->addAction(action("Channels..."));
    view_toolbar_->addAction(action("Scale All..."));
    view_toolbar_->addAction(action("Zoom In Vertical"));
    view_toolbar_->addAction(action("Zoom Out Vertical"));
    view_toolbar_->addAction(action("Zoom In Horizontal"));
    view_toolbar_->addAction(action("Zoom Out Horizontal"));

    view_toolbar_views_menu_->addSeparator ();
    toggle_all_toolbars_ = new QAction (tr("Hide all Toolbars"), this);
    connect (toggle_all_toolbars_, SIGNAL(triggered()), SLOT(toggleAllToolbars()));
    toggle_all_toolbars_->setData (true);
    view_toolbar_views_menu_->addAction (toggle_all_toolbars_);
}

//-------------------------------------------------------------------
void MainWindow::toggleStatusBar (bool visible)
{
    statusBar()->setVisible (visible);
    QSettings settings ("SigViewer");
    settings.setValue("MainWindow/statusbar", statusBar()->isVisible());
}

//-------------------------------------------------------------------
void MainWindow::toggleAllToolbars ()
{
    if (toggle_all_toolbars_->data().toBool())
    {
        toggle_all_toolbars_->setData (false);
        toggle_all_toolbars_->setText(tr("Show all Toolbars"));
        foreach (QAction* toggle_action, view_toolbar_views_menu_->actions())
            if (toggle_action->isCheckable() && toggle_action->isChecked())
                toggle_action->trigger ();
    }
    else
    {
        toggle_all_toolbars_->setData (true);
        toggle_all_toolbars_->setText(tr("Hide all Toolbars"));
        foreach (QAction* toggle_action, view_toolbar_views_menu_->actions())
            if (toggle_action->isCheckable() && !toggle_action->isChecked())
                toggle_action->trigger ();
    }
}


//-----------------------------------------------------------------------------
void MainWindow::addBackgroundProcessToStatusBar (QString name, int max)
{
    QProgressBar* progress_bar = new QProgressBar (this);
    progress_bar->setMinimum (0);
    progress_bar->setMaximum (max);
    progress_bar->setValue (0);

    background_processes_progressbars_[name] = progress_bar;
    background_processes_labels_[name] = new QLabel (name, this);
    statusBar()->insertPermanentWidget (0, progress_bar);
    statusBar()->insertPermanentWidget (0, background_processes_labels_[name]);
}

//-----------------------------------------------------------------------------
void MainWindow::updateBackgroundProcessonStatusBar (QString name, int value)
{
    if (!background_processes_progressbars_.contains (name))
        return;

    background_processes_progressbars_[name]->setValue (value);
}

//-----------------------------------------------------------------------------
void MainWindow::removeBackgroundProcessFromStatusBar (QString name)
{
    if (!background_processes_progressbars_.contains (name))
        return;

    statusBar()->removeWidget (background_processes_progressbars_[name]);
    delete background_processes_progressbars_[name];
    background_processes_progressbars_.remove (name);

    statusBar()->removeWidget (background_processes_labels_[name]);
    delete background_processes_labels_[name];
    background_processes_labels_.remove (name);
}

//-----------------------------------------------------------------------------
void MainWindow::initMenus (QSharedPointer<ApplicationContext> application_context)
{
    file_recent_files_menu_ = new QMenu(tr("Open &Recent"), this);
    connect(file_recent_files_menu_, SIGNAL(aboutToShow()),
            SIGNAL(recentFileMenuAboutToShow()));
    connect(file_recent_files_menu_, SIGNAL(triggered(QAction*)),
            SIGNAL(recentFileActivated(QAction*)));

    file_menu_ = menuBar()->addMenu(tr("&File"));
    file_menu_->addAction(action("Open..."));
    file_menu_->addMenu (file_recent_files_menu_);
    file_menu_->addAction (action("Save"));
    file_menu_->addAction (action("Save as..."));
    file_menu_->addSeparator ();
    file_menu_->addAction (action("Export to PNG..."));
    file_menu_->addAction (action("Export to GDF..."));
    file_menu_->addAction (action("Export Events..."));
    file_menu_->addAction (action("Import Events..."));
    file_menu_->addSeparator ();
    file_menu_->addAction (action("Info..."));
    file_menu_->addSeparator ();
    file_menu_->addAction (action("Close"));
    file_menu_->addSeparator ();
    file_menu_->addAction (action("Exit"));

    edit_menu_ = menuBar()->addMenu(tr("&Edit"));
    edit_menu_->addAction (action("Undo"));
    edit_menu_->addAction (action("Redo"));
    edit_menu_->addSeparator ();
    edit_menu_->addAction (action("To all Channels"));
    edit_menu_->addAction (action("Copy to Channels..."));
    edit_menu_->addAction (action("Delete"));
    edit_menu_->addAction (action("Change Channel..."));
    edit_menu_->addAction (action("Change Type..."));
    edit_menu_->addSeparator ();
    edit_menu_->addAction (action("Insert Over"));

    mouse_mode_menu_ = menuBar()->addMenu(tr("&Mode"));
    mouse_mode_menu_->addAction (action("New Event"));
    mouse_mode_menu_->addAction (action("Edit Event"));
    mouse_mode_menu_->addAction (action("Scroll"));
    mouse_mode_menu_->addAction (action("View Options"));

    QAction* toggle_status_bar = new QAction (tr("Statusbar"), this);
    toggle_status_bar->setCheckable (true);
    toggle_status_bar->setChecked (statusBar()->isVisible());
    connect (toggle_status_bar, SIGNAL(toggled(bool)), this, SLOT(toggleStatusBar(bool)));

    view_menu_ = menuBar()->addMenu(tr("&View"));
    view_menu_->addMenu (view_toolbar_views_menu_);
    view_menu_->addAction(toggle_status_bar);
    QMenu* animations_menu = view_menu_->addMenu(tr("Animations"));
    animations_menu->addAction(action("Animations"));
    animations_menu->addAction(action("Set Animation Duration"));
    view_menu_->addSeparator();
    view_menu_->addAction(action("Events..."));
    view_menu_->addAction(action("Channels..."));
    view_menu_->addAction(action("Scale All..."));
    view_menu_->addSeparator();
    view_menu_->addAction(action("Zoom In Vertical"));
    view_menu_->addAction(action("Zoom Out Vertical"));
    view_menu_->addAction(action("Zoom In Horizontal"));
    view_menu_->addAction(action("Zoom Out Horizontal"));
    view_menu_->addSeparator();
    view_menu_->addAction(action("Go to..."));
    view_menu_->addSeparator();
    view_menu_->addAction(action("Goto and Select Next Event"));
    view_menu_->addAction(action("Goto and Select Previous Event"));
    view_menu_->addSeparator();
    view_menu_->addAction(action("Fit View to Selected Event"));
    view_menu_->addAction(action("Hide Events of other Type"));
    view_menu_->addAction(action("Show all Events"));

    tools_menu_ = menuBar()->addMenu(tr("&Tools"));
    tools_menu_->addActions(GuiActionFactory::getInstance()->getQActions("Signal Processing"));

    help_menu_ = menuBar()->addMenu(tr("&Help"));
    if (application_context->modeActivated (APPLICATION_TEST_MODE))
    {
        help_menu_->addAction (action("Run Tests..."));
        help_menu_->addSeparator();
    }
    help_menu_->addAction (action("About"));
}

//-----------------------------------------------------------------------------
void MainWindow::closeEvent (QCloseEvent* event)
{
    GuiActionFactory::getInstance()->getQAction("Exit")->trigger();
    event->ignore ();
}

//-----------------------------------------------------------------------------
void MainWindow::dropEvent (QDropEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        QString localPath(event->mimeData()->urls().first().toLocalFile());
        event->acceptProposedAction();
        OpenFileGuiCommand::openFile (localPath);
    } else if (event->mimeData()->hasText())
    {
        QString localPath(event->mimeData()->text());
        event->acceptProposedAction();
        OpenFileGuiCommand::openFile (localPath);
    }
}

//-----------------------------------------------------------------------------
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText() || event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

//-----------------------------------------------------------------------------
void MainWindow::resizeEvent (QResizeEvent* event)
{
    QSettings settings ("SigViewer");
    settings.setValue("MainWindow/size", event->size());
}

//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
void MainWindow::setStatusBarSignalLength(float64 length)
{
    if (length > 0)
        status_bar_signal_length_label_->setText (tr("Length: ") + QString::number(length, 'f', 1) + "s");
    status_bar_signal_length_label_->setVisible (length > 0);
}

//-----------------------------------------------------------------------------
void MainWindow::setStatusBarNrChannels(int32 nr_channels)
{
    if (nr_channels > 0)
        status_bar_nr_channels_label_->setText (tr("Channels: ") + QString::number(nr_channels));
    status_bar_nr_channels_label_->setVisible (nr_channels > 0);
}

//-----------------------------------------------------------------------------
QAction* MainWindow::action (QString const& action_id)
{
    return GuiActionFactory::getInstance()->getQAction (action_id);
}

} // namespace SigViewer_
