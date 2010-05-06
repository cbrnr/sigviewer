#include "main_window_model_impl.h"
#include "main_window.h"
#include "tab_context.h"
#include "file_context.h"
#include "application_context.h"

#include "file_handling/file_signal_reader_factory.h"
#include "file_handling_impl/event_table_file_reader.h"
#include "file_handling_impl/event_manager_impl.h"
#include "file_handling_impl/channel_manager_impl.h"
#include "gui_impl/gui_helper_functions.h"
#include "gui_impl/open_file_gui_command.h"

#include "signal_browser/signal_browser_model_4.h"
#include "signal_browser/signal_browser_view.h"
#include "signal_browser/calculcate_frequency_spectrum_command.h"

#include <QSettings>

namespace BioSig_
{

int const MainWindowModelImpl::NUMBER_RECENT_FILES_ = 8;


// constructor
MainWindowModelImpl::MainWindowModelImpl ()
: main_window_(0),
  application_context_ (ApplicationContext::getInstance()),
  tab_widget_ (0)
{

}

// destructor
MainWindowModelImpl::~MainWindowModelImpl()
{
    // nothing
}

// set main window
void MainWindowModelImpl::setMainWindow (MainWindow* main_window)
{
    main_window_ = main_window;
    application_context_->setState(APP_STATE_NO_FILE_OPEN);
}

// void load settings
void MainWindowModelImpl::loadSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("MainWindowModelImpl");

    int size = settings.beginReadArray("recent_file");
    for (int i = 0; i < size; i++)
    {
        settings.setArrayIndex(i);
        recent_file_list_.append(settings.value("name").toString());
    }

    settings.endArray();

    settings.endGroup();
}

// void save settings
void MainWindowModelImpl::saveSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("MainWindowModelImpl");
    settings.beginWriteArray("recent_file");

    for (int i = 0; i < recent_file_list_.size(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("name", recent_file_list_.at(i));
    }

    settings.endArray();
    settings.endGroup();
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::tabChanged (int tab_index)
{
    if (tab_contexts_.find(tab_index) != tab_contexts_.end ())
    {
        ApplicationContext::getInstance()->setCurrentTabContext (tab_contexts_[tab_index]);
        tab_contexts_[tab_index]->gotActive ();
    }
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::closeTab (int tab_index)
{
    if (tab_index == 0)
    {
        fileCloseAction();
        return;
    }
    QWidget* widget = tab_widget_->widget (tab_index);
    browser_models_.erase (tab_index);
    tab_widget_->removeTab (tab_index);
    delete widget;
}

// recent file menu about to show
void MainWindowModelImpl::recentFileMenuAboutToShow()
{
    main_window_->setRecentFiles(recent_file_list_);
}

// recent file activated
void MainWindowModelImpl::recentFileActivated(QAction* recent_file_action)
{
    OpenFileGuiCommand::openFile (recent_file_action->text());
}


// file close action
void MainWindowModelImpl::fileCloseAction()
{
    if (current_file_context_.isNull())
        return;

    if (current_file_context_->getState() == FILE_STATE_CHANGED &&
        !main_window_
        ->showFileCloseDialog(current_file_context_->getFileName ()))
        return; // user cancel

    current_file_context_.clear ();

    // close
    main_window_->setCentralWidget(0);
    tab_widget_ = 0;
    signal_browser_tab_ = 0;
    tab_contexts_.clear ();

    // reset status bar
    main_window_->setStatusBarSignalLength(-1);
    main_window_->setStatusBarNrChannels(-1);

    ApplicationContext::getInstance()->addFileContext (QSharedPointer<FileContext>(0));
    application_context_->setState(APP_STATE_NO_FILE_OPEN);
    main_window_->setWindowTitle (tr("SigViewer"));
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::storeAndInitTabContext (QSharedPointer<TabContext> context, int tab_index)
{
    tab_contexts_[tab_index] = context;


    ApplicationContext::getInstance()->setCurrentTabContext (context);

    context->setSelectionState (TAB_STATE_NO_EVENT_SELECTED);
    context->setEditState (TAB_STATE_NO_REDO_NO_UNDO);
}

// set changed
void MainWindowModelImpl::setChanged()
{
    ApplicationContext::getInstance()->getCurrentFileContext()->setState (FILE_STATE_CHANGED);
}

//-------------------------------------------------------------------------
QSharedPointer<SignalVisualisationModel> MainWindowModelImpl::createSignalVisualisation (QSharedPointer<ChannelManager> channel_manager)
{
    int tab_index = createSignalVisualisationImpl (channel_manager, QSharedPointer<EventManager>(0));
    tab_widget_->setTabText(tab_index, tr("Mean"));
    tab_widget_->setCurrentIndex(tab_index);
    return browser_models_[tab_index];
}

//-----------------------------------------------------------------------------
QSharedPointer<SignalVisualisationModel> MainWindowModelImpl::createSignalVisualisationOfFile (QSharedPointer<FileContext> file_ctx)
{
    if (!tab_widget_)
    {
        tab_widget_ = new QTabWidget (main_window_);
        connect (tab_widget_, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
        connect (tab_widget_, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
        tab_widget_->setTabsClosable (true);
    }


    // waldesel:
    // --begin
    //   to be replaced as soon as multi file support is implemented
    if (!current_file_context_.isNull())
        fileCloseAction();
    // --end


    // waldesel:
    // --begin
    //   this is only to support old code here.. remove this line as soon
    //   command pattern for gui commands is finalised
    current_file_context_ = file_ctx;
    // --end

    recent_file_list_.removeAll (file_ctx->getFilePathAndName());
    if (recent_file_list_.size() == NUMBER_RECENT_FILES_)
        recent_file_list_.pop_back();
    recent_file_list_.push_front (file_ctx->getFilePathAndName());

    // waldesel:
    // --begin
    //   to be replaced as soon as new zooming is implemented
    main_window_->setStatusBarSignalLength (file_ctx->getChannelManager()->getDurationInSec());
    // --end

    main_window_->setStatusBarNrChannels (file_ctx->getChannelManager()->getNumberChannels());
    main_window_->setWindowTitle (file_ctx->getFileName() + tr(" - SigViewer"));


    int tab_index = createSignalVisualisationImpl (file_ctx->getChannelManager(), file_ctx->getEventManager());
    main_window_->setCentralWidget(tab_widget_);
    tab_widget_->show();
    tab_widget_->setTabText(tab_index, tr("Signal Data"));
    return browser_models_[tab_index];
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::closeCurrentFileTabs ()
{
    // waldesel:
    // --begin
    //   this is only to support old code here.. refactor these lines as soon
    //   command pattern for gui commands is finalised
    current_file_context_.clear ();
    // --end

    // waldesel:
    // --begin
    //   to be refactored as soon as multi file support is implemented
    main_window_->setCentralWidget (0);
    tab_widget_ = 0;
    signal_browser_tab_ = 0;
    tab_contexts_.clear ();
    // --end

    main_window_->setStatusBarSignalLength(-1);
    main_window_->setStatusBarNrChannels(-1);
    main_window_->setWindowTitle (tr("SigViewer"));
}

//-----------------------------------------------------------------------------
QSharedPointer<SignalVisualisationModel> MainWindowModelImpl::getCurrentSignalVisualisationModel ()
{
    if (!tab_widget_)
        return QSharedPointer<SignalVisualisationModel>(0);

    return browser_models_[tab_widget_->currentIndex()];
}

//-------------------------------------------------------------------------
int MainWindowModelImpl::createSignalVisualisationImpl (QSharedPointer<ChannelManager> channel_manager,
                                                                        QSharedPointer<EventManager> event_manager)
{
    QSharedPointer<TabContext> tab_context (new TabContext);

    QSharedPointer<SignalBrowserModel> model (new SignalBrowserModel (event_manager,
                                                                      channel_manager,
                                                                      tab_context));

    SignalBrowserView* view = new SignalBrowserView (model, event_manager, tab_context, main_window_->rect(), tab_widget_);

    int tab_index = tab_widget_->addTab (view, tr("Signal"));

    model->setSignalBrowserView (view);
    browser_models_[tab_index] = model;

    if (!event_manager.isNull())
    {
        model->connect (event_manager.data(), SIGNAL(eventCreated(QSharedPointer<SignalEvent const>)),
                                   SLOT(addEventItem(QSharedPointer<SignalEvent const>)));
        model->connect (event_manager.data(), SIGNAL(eventRemoved(EventID)),
                                   SLOT(removeEventItem(EventID)));
        model->connect (event_manager.data(), SIGNAL(eventChanged(EventID)),
                                   SLOT(updateEvent(EventID)));
    }
    storeAndInitTabContext (tab_context, tab_index);
    return tab_index;
}


} // namespace BioSig_
