// © SigViewer developers
//
// License: GPL-3.0


#include "main_window.h"
#include "gui/background_processes.h"
#include "gui/commands/open_file_gui_command.h"

#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QSettings>
#include <QMimeData>
#include <QIcon>
#include <QPalette>
#include <QSizePolicy>
#include <QStyle>
#include <QStyleOption>
#include <QToolButton>
#include <QProxyStyle>

namespace
{

class MenuPaddingStyle : public QProxyStyle
{
public:
    QSize sizeFromContents(ContentsType type, const QStyleOption* option,
                           const QSize& size, const QWidget* widget) const override
    {
        QSize base = QProxyStyle::sizeFromContents(type, option, size, widget);
        if (type == CT_MenuItem)
            base.setWidth(base.width() + 30);
        return base;
    }
};

} // anonymous namespace

namespace sigviewer
{

//----------------------------------------------------------------------------
MainWindow::MainWindow(QSharedPointer<ApplicationContext> application_context)
 : QMainWindow(0)
{
    setWindowTitle(tr("SigViewer"));
    setAcceptDrops(true);
    setWindowIcon(QIcon(":images/sigviewer128.png"));
    initStatusBar();
    initToolBars();
    initMenus(application_context);
    initHamburgerMenu();
    setUnifiedTitleAndToolBarOnMac (true);
#ifdef Q_OS_MACOS
    const QString toolbar_style = R"(
        QToolButton:hover {
            background: rgba(128, 128, 128, 0.2);
            border-radius: 4px;
        }
        QToolButton:pressed {
            background: rgba(128, 128, 128, 0.35);
            border-radius: 4px;
        }
    )";
    file_toolbar_->setStyleSheet(toolbar_style);
#endif
    
    QSettings settings;
    resize(settings.value("MainWindow/size", QSize(1200, 800)).toSize());
    setMinimumSize(900, 800);
    restoreGeometry(settings.value("geometry").toByteArray());  //restore geometry and window state (full screen etc.)
    restoreState(settings.value("windowState").toByteArray());
}

//-----------------------------------------------------------------------------
void MainWindow::initStatusBar()
{
    connect (&BackgroundProcesses::instance(), SIGNAL(newProcess(QString,int)), SLOT(addBackgroundProcessToStatusBar(QString,int)));
    connect (&BackgroundProcesses::instance(), SIGNAL(processChangedState(QString,int)), SLOT(updateBackgroundProcessonStatusBar(QString,int)));
    connect (&BackgroundProcesses::instance(), SIGNAL(processRemoved(QString)), SLOT(removeBackgroundProcessFromStatusBar(QString)));

    QStatusBar* status_bar = statusBar();
    //status_bar->showMessage(tr("Ready"));

    status_bar_signal_length_label_ = new QLabel (this);
    status_bar_nr_channels_label_ = new QLabel (this);
    status_bar_signal_length_label_->setAlignment(Qt::AlignHCenter);
    status_bar_nr_channels_label_->setAlignment(Qt::AlignHCenter);
    status_bar->addPermanentWidget(status_bar_signal_length_label_);
    status_bar->addPermanentWidget(status_bar_nr_channels_label_);
    QSettings settings;
    status_bar->setVisible (settings.value ("MainWindow/statusbar", true).toBool());
}

//-----------------------------------------------------------------------------
void MainWindow::initToolBars()
{
    file_toolbar_ = addToolBar(tr("Toolbar"));
    file_toolbar_->setObjectName("Toolbar");
    file_toolbar_->setMovable(false);
    file_toolbar_->addAction(action(tr("Open...")));
    file_toolbar_->addAction(action(tr("Save")));
    file_toolbar_->addAction(action(tr("Info...")));
    file_toolbar_->addSeparator();
    file_toolbar_->addAction(action(tr("Undo")));
    file_toolbar_->addAction(action(tr("Redo")));
    file_toolbar_->addSeparator();
    file_toolbar_->addAction(action(tr("New Event")));
    file_toolbar_->addAction(action(tr("Edit Event")));
    file_toolbar_->addAction(action(tr("Scroll")));
    file_toolbar_->addAction(action(tr("View Options")));
    file_toolbar_->addSeparator();
    file_toolbar_->addAction(action(tr("Events...")));
    file_toolbar_->addSeparator();
    file_toolbar_->addAction(action(tr("Channels...")));
    file_toolbar_->addAction(action(tr("Scale All...")));
    file_toolbar_->addAction(action(tr("Zoom In Channels")));
    file_toolbar_->addAction(action(tr("Zoom Out Channels")));
    file_toolbar_->addAction(action(tr("Zoom In Time")));
    file_toolbar_->addAction(action(tr("Zoom Out Time")));
}

//-------------------------------------------------------------------
void MainWindow::toggleStatusBar (bool visible)
{
    statusBar()->setVisible (visible);
    QSettings settings;
    settings.setValue("MainWindow/statusbar", statusBar()->isVisible());
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
    file_menu_->addAction(action(tr("Open...")));
    file_menu_->addMenu (file_recent_files_menu_);
    file_menu_->addAction (action(tr("Save")));
    file_menu_->addAction (action(tr("Save as...")));
    file_menu_->addAction (action(tr("Info...")));
    file_menu_->addAction (action(tr("Close")));
    file_menu_->addSeparator ();
    file_menu_->addAction (action(tr("Import Events...")));
    file_menu_->addAction (action(tr("Export Events to CSV...")));
    file_menu_->addAction (action(tr("Export Events to EVT...")));
    // file_menu_->addAction (action(tr("Export to GDF...")));
    file_menu_->addSeparator ();
    file_menu_->addAction (action(tr("Exit")));

    edit_menu_ = menuBar()->addMenu(tr("&Edit"));
    edit_menu_->addAction (action(tr("Undo")));
    edit_menu_->addAction (action(tr("Redo")));
    edit_menu_->addSeparator ();
    edit_menu_->addAction (action(tr("To all Channels")));
    edit_menu_->addAction (action(tr("Copy to Channels...")));
    edit_menu_->addAction (action(tr("Delete")));
    edit_menu_->addAction (action(tr("Change Channel...")));
    edit_menu_->addAction (action(tr("Change Type...")));
    edit_menu_->addSeparator ();
    edit_menu_->addAction (action(tr("Insert Over")));

    mouse_mode_menu_ = menuBar()->addMenu(tr("&Mode"));
    mouse_mode_menu_->addAction (action(tr("New Event")));
    mouse_mode_menu_->addAction (action(tr("Edit Event")));
    mouse_mode_menu_->addAction (action(tr("Scroll")));
    mouse_mode_menu_->addAction (action(tr("View Options")));


    QSettings settings;

    QAction* toggle_status_bar = new QAction (tr("Statusbar"), this);
    toggle_status_bar->setCheckable (true);
    toggle_status_bar->setChecked (settings.value ("MainWindow/statusbar", true).toBool());
    connect (toggle_status_bar, SIGNAL(toggled(bool)), this, SLOT(toggleStatusBar(bool)));

    view_menu_ = menuBar()->addMenu(tr("&View"));
    view_menu_->addAction(file_toolbar_->toggleViewAction());
    view_menu_->addAction(toggle_status_bar);
#ifndef Q_OS_MACOS
    toggle_menubar_ = new QAction(tr("Menubar"), this);
    toggle_menubar_->setCheckable(true);
    toggle_menubar_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_M));
    connect(toggle_menubar_, &QAction::triggered, this, &MainWindow::toggleMenuBar);
    view_menu_->addAction(toggle_menubar_);
#endif
    view_menu_->addSeparator();
    view_menu_->addAction(action(tr("Events...")));
    view_menu_->addAction(action(tr("Channels...")));
    view_menu_->addAction(action(tr("Scale All...")));
    view_menu_->addSeparator();
    view_menu_->addAction(action(tr("Zoom In Channels")));
    view_menu_->addAction(action(tr("Zoom Out Channels")));
    view_menu_->addAction(action(tr("Zoom In Time")));
    view_menu_->addAction(action(tr("Zoom Out Time")));
    view_menu_->addSeparator();
    view_menu_->addAction(action(tr("Go to...")));
    view_menu_->addSeparator();
    view_menu_->addAction(action(tr("Goto and Select Next Event")));
    view_menu_->addAction(action(tr("Goto and Select Previous Event")));
    view_menu_->addSeparator();
    view_menu_->addAction(action(tr("Fit View to Selected Event")));
    view_menu_->addAction(action(tr("Hide Events of other Type")));
    view_menu_->addAction(action(tr("Show all Events")));

    tools_menu_ = menuBar()->addMenu(tr("&Tools"));
    tools_menu_->addActions(GuiActionFactory::getInstance()->getQActions("Signal Processing"));
    tools_menu_->addSeparator();
    tools_menu_->addActions(GuiActionFactory::getInstance()->getQActions("Detrend"));

    help_menu_ = menuBar()->addMenu(tr("&Help"));
    help_menu_->addAction (action(tr("About")));
}

//-----------------------------------------------------------------------------
void MainWindow::initHamburgerMenu()
{
#ifndef Q_OS_MACOS
    QSettings settings;
    bool show_menubar = settings.value("MainWindow/menubar", true).toBool();

    // Expanding spacer to push the hamburger button to the right
    QWidget* hamburger_spacer = new QWidget(this);
    hamburger_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    hamburger_spacer_action_ = file_toolbar_->addWidget(hamburger_spacer);

    // Hamburger button with popup menu
    hamburger_button_ = new QToolButton(this);
    hamburger_button_->setIcon(QIcon::fromTheme("menu"));
    hamburger_button_->setToolTip(tr("Menu"));
    QMenu* hamburger_popup = new QMenu(this);
    for (QAction* menu_action : menuBar()->actions())
    {
        if (QMenu* submenu = menu_action->menu())
            hamburger_popup->addMenu(submenu);
    }
    hamburger_popup->setStyle(new MenuPaddingStyle);
    hamburger_button_->setMenu(hamburger_popup);
    hamburger_button_->setPopupMode(QToolButton::InstantPopup);
    hamburger_action_ = file_toolbar_->addWidget(hamburger_button_);

    bool hamburger_enabled = !show_menubar;
    hamburger_spacer_action_->setVisible(hamburger_enabled);
    hamburger_action_->setVisible(hamburger_enabled);
    menuBar()->setVisible(show_menubar);
    toggle_menubar_->setChecked(show_menubar);
#endif
}

//-------------------------------------------------------------------
void MainWindow::toggleMenuBar()
{
#ifndef Q_OS_MACOS
    bool menubar_visible = menuBar()->isVisible();
    menuBar()->setVisible(!menubar_visible);
    bool hamburger_enabled = menubar_visible;
    hamburger_spacer_action_->setVisible(hamburger_enabled);
    hamburger_action_->setVisible(hamburger_enabled);
    toggle_menubar_->setChecked(!menubar_visible);
    QSettings settings;
    settings.setValue("MainWindow/menubar", !hamburger_enabled);
#endif
}

//-----------------------------------------------------------------------------
void MainWindow::closeEvent (QCloseEvent* event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    GuiActionFactory::getInstance()->getQAction(tr("Exit"))->trigger();
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
    QSettings settings;
    settings.setValue("MainWindow/size", event->size());
}

//-----------------------------------------------------------------------------
void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::PaletteChange) {
        const bool dark = palette().color(QPalette::Window).lightness() < 128;
        QIcon::setThemeName(dark ? "dark" : "light");
    }
    QMainWindow::changeEvent(event);
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
        status_bar_signal_length_label_->setText (tr("Length: %1s").arg(QString::number(length, 'f', 1)));
    status_bar_signal_length_label_->setVisible (length > 0);
}

//-----------------------------------------------------------------------------
void MainWindow::setStatusBarNrChannels(int32 nr_channels)
{
    if (nr_channels > 0)
        status_bar_nr_channels_label_->setText (tr("Channels: %1").arg(nr_channels));
    status_bar_nr_channels_label_->setVisible (nr_channels > 0);
}

//-----------------------------------------------------------------------------
QAction* MainWindow::action (QString const& action_id)
{
    return GuiActionFactory::getInstance()->getQAction (action_id);
}

}
