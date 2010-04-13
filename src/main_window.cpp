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
#include "gui_action_manager.h"
#include "application_context.h"
#include "gui/gui_action_factory.h"

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
MainWindow::MainWindow ()
 : QMainWindow(0),
   model_ (ApplicationContext::getInstance()->getMainWindowModel()),
   action_manager_ (ApplicationContext::getInstance()->getGUIActionManager())
{
    setWindowTitle (tr("SigViewer"));
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
    status_bar_signal_length_label_->setAlignment(Qt::AlignHCenter);
    status_bar_nr_channels_label_->setAlignment(Qt::AlignHCenter);
    status_bar_signal_length_label_->setMinimumWidth(status_bar_signal_length_label_->sizeHint().width() + 10);
    status_bar_nr_channels_label_->setMinimumWidth(status_bar_nr_channels_label_->sizeHint().width() + 10);
    status_bar->addPermanentWidget(status_bar_signal_length_label_);
    status_bar->addPermanentWidget(status_bar_nr_channels_label_);
}

// init icons sets
void MainWindow::initIconSets()
{
    help_about_icon_.addFile(":/images/help_22x22.png");
}

// init actions
void MainWindow::initActions()
{
    calculate_frequency_spectrum_action_ = new QAction(tr("Calculate Power Spectrum"), this);
    calculate_frequency_spectrum_action_->setStatusTip(tr("Calculates power spectrum of selected event type"));
    calculate_frequency_spectrum_action_->setEnabled(false);
    connect(calculate_frequency_spectrum_action_, SIGNAL(triggered()),
            model_.data(), SLOT(calculateFrequencySpectrumAction()));

    calculate_erd_ers_map_action_ = new QAction(tr("Generate ERD/ERS Map"), this);
    calculate_erd_ers_map_action_->setStatusTip(tr("Calculates ERD/ERS map of selected event type"));
    calculate_erd_ers_map_action_->setEnabled(false);
    connect(calculate_erd_ers_map_action_, SIGNAL(triggered()),
            model_.data(), SLOT(calculateERDERSMap()));

    help_log_action_= new QAction(tr("&Log..."), this);
    help_log_action_->setObjectName("help_log_action_");
    help_log_action_->setStatusTip(tr("Log"));
    connect(help_log_action_, SIGNAL(triggered()),
            model_.data(), SLOT(helpLogAction()));

    help_about_action_= new QAction(help_about_icon_,
                                    tr("&About SigViewer..."), this);
    help_about_action_->setObjectName("help_about_action_");
    help_about_action_->setStatusTip(tr("About SigViewer"));
    connect(help_about_action_, SIGNAL(triggered()),
            model_.data(), SLOT(helpAboutAction()));
}

// init tool bars
void MainWindow::initToolBars()
{
    view_toolbar_views_menu_ = new QMenu (tr("Toolbars"), this);

    file_toolbar_ = addToolBar(tr("File"));
    view_toolbar_views_menu_->addAction (file_toolbar_->toggleViewAction());
    file_toolbar_->addActions (action_manager_->getActionsOfGroup(GUIActionManager::FILE_TOOLBAR_ACTIONS));


    mouse_mode_toolbar_ = addToolBar(tr("Mode"));
    view_toolbar_views_menu_->addAction (mouse_mode_toolbar_->toggleViewAction());
    mouse_mode_toolbar_->addActions (action_manager_->getActionsOfGroup(GUIActionManager::MODE_ACTIONS));

    edit_toolbar_ = addToolBar(tr("Edit"));
    view_toolbar_views_menu_->addAction (edit_toolbar_->toggleViewAction());
    edit_toolbar_->addActions (action_manager_->getActionsOfGroup(GUIActionManager::EDIT_TOOLBAR_ACTIONS));

    view_toolbar_ = addToolBar(tr("View"));
    view_toolbar_views_menu_->addAction (view_toolbar_->toggleViewAction());
    view_toolbar_->addActions(action_manager_->getActionsOfGroup(GUIActionManager::VIEW_TOOLBAR_ACTIONS));

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
    view_toolbar_views_menu_->addAction (navigation_toolbar_->toggleViewAction());
    navigation_toolbar_->setIconSize(QSize(22, 22));
    navigation_toolbar_->addWidget(secs_per_page_combobox_);
    navigation_toolbar_->addWidget(signals_per_page_combobox_);

    connect(secs_per_page_combobox_, SIGNAL(activated(const QString&)),
            model_.data(), SLOT(secsPerPageChanged(const QString&)));
    connect(signals_per_page_combobox_, SIGNAL(activated(const QString&)),
            model_.data(), SLOT(signalsPerPageChanged(const QString&)));
    connect(secs_per_page_combobox_->lineEdit(), SIGNAL(returnPressed()),
            this, SLOT(secsPerPageReturnPressed()));
    connect(signals_per_page_combobox_->lineEdit(), SIGNAL(returnPressed()),
            this, SLOT(signalsPerPageReturnPressed()));
}

// secs per page return pressed
void MainWindow::secsPerPageReturnPressed()
{
    model_->secsPerPageChanged(secs_per_page_combobox_->currentText());
}

// signals per page return pressed
void MainWindow::signalsPerPageReturnPressed()
{
    model_->signalsPerPageChanged(signals_per_page_combobox_->currentText());
}

//-------------------------------------------------------------------
void MainWindow::toggleStatusBar (bool visible)
{
    statusBar()->setVisible (visible);
}


//-----------------------------------------------------------------------------
void MainWindow::initMenus()
{
    QList<QAction*> file_menu_actions =
            action_manager_->getActionsOfGroup(GUIActionManager::FILE_MENU_ACTIONS);
    file_recent_files_menu_ = new QMenu(tr("Open &Recent"), this);
    connect(file_recent_files_menu_, SIGNAL(aboutToShow()),
            model_.data(), SLOT(recentFileMenuAboutToShow()));
    connect(file_recent_files_menu_, SIGNAL(triggered(QAction*)),
            model_.data(), SLOT(recentFileActivated(QAction*)));
    file_menu_actions.insert (1, file_recent_files_menu_->menuAction());
    file_menu_ = new QMenu(tr("&File"), this);
    //file_menu_->addAction (GuiActionFactory::getInstance()->getQAction("Open File"));
    file_menu_->addActions(file_menu_actions);
    menuBar()->addMenu(file_menu_);

    edit_menu_ = menuBar()->addMenu(tr("&Edit"));
    edit_menu_->addActions (action_manager_->getActionsOfGroup (GUIActionManager::EDIT_MENU_ACTIONS));

    mouse_mode_menu_ = menuBar()->addMenu(tr("&Mode"));
    mouse_mode_menu_->addActions (action_manager_->getActionsOfGroup(GUIActionManager::MODE_ACTIONS));

    view_menu_ = menuBar()->addMenu(tr("&View"));
    view_menu_->addMenu(view_toolbar_views_menu_);
    QAction* toggle_status_bar = new QAction (tr("Statusbar"), this);
    toggle_status_bar->setCheckable (true);
    toggle_status_bar->setChecked (true);
    connect (toggle_status_bar, SIGNAL(toggled(bool)), this, SLOT(toggleStatusBar(bool)));
    view_menu_->addAction(toggle_status_bar);
    view_menu_->addSeparator();
    view_menu_->addActions (action_manager_->getActionsOfGroup(GUIActionManager::VIEW_MENU_ACTIONS));

    tools_menu_ = menuBar()->addMenu(tr("&Tools"));
    tools_menu_->addActions(action_manager_->getActionsOfGroup(GUIActionManager::TOOLS_MENU_ACTIONS));

    options_menu_ = menuBar()->addMenu(tr("&Options"));
    options_menu_->addActions (action_manager_->getActionsOfGroup(GUIActionManager::OPTIONS_MENU_ACTIONS));

    help_menu_ = menuBar()->addMenu(tr("&Help"));
    help_menu_->addAction(help_log_action_);
    help_menu_->addSeparator();
    help_menu_->addAction(help_about_action_);
}

// close event
void MainWindow::closeEvent(QCloseEvent*)
{
    model_->fileExitAction();
}

void MainWindow::dropEvent (QDropEvent* event)
{
    if (event->mimeData()->hasText())
    {
        QUrl url (event->mimeData()->text());
        event->acceptProposedAction();
        model_->openFile(url.path());
    } else if (event->mimeData()->hasUrls())
    {
        QUrl url (event->mimeData()->urls().first().toLocalFile());
        event->acceptProposedAction();
        model_->openFile(url.path());
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

} // namespace BioSig_
