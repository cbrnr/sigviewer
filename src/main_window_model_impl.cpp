#include "main_window_model_impl.h"
#include "main_window.h"
#include "tab_context.h"
#include "file_context.h"
#include "application_context.h"

#include "gui_impl/open_file_gui_command.h"

#include "signal_browser/signal_browser_model_4.h"
#include "signal_browser/signal_browser_view.h"

#include <QSettings>

namespace BioSig_
{

int const MainWindowModelImpl::NUMBER_RECENT_FILES_ = 8;

//-----------------------------------------------------------------------------
MainWindowModelImpl::MainWindowModelImpl ()
: main_window_(0),
  tab_widget_ (0)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::setMainWindow (MainWindow* main_window)
{
    main_window_ = main_window;
    ApplicationContext::getInstance()->setState(APP_STATE_NO_FILE_OPEN);
}

//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
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
        // waldesel:
        // --begin
        //   to be replaced as soon as multi file support is implemented
        if (!(ApplicationContext::getInstance()->getCurrentFileContext().isNull()))
            GuiActionFactory::getInstance()->getQAction("Close")->trigger();
        // --end
        return;
    }
    QWidget* widget = tab_widget_->widget (tab_index);
    browser_models_.erase (tab_index);
    tab_widget_->removeTab (tab_index);
    delete widget;
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::recentFileMenuAboutToShow()
{
    main_window_->setRecentFiles(recent_file_list_);
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::recentFileActivated(QAction* recent_file_action)
{
    OpenFileGuiCommand::openFile (recent_file_action->text());
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::storeAndInitTabContext (QSharedPointer<TabContext> context, int tab_index)
{
    tab_contexts_[tab_index] = context;

    ApplicationContext::getInstance()->setCurrentTabContext (context);

    context->setSelectionState (TAB_STATE_NO_EVENT_SELECTED);
    context->setEditState (TAB_STATE_NO_REDO_NO_UNDO);
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
    // waldesel:
    // --begin
    //   to be replaced as soon as multi file support is implemented
    if (!(ApplicationContext::getInstance()->getCurrentFileContext().isNull()))
        GuiActionFactory::getInstance()->getQAction("Close")->trigger();
    // --end

    if (!tab_widget_)
    {
        tab_widget_ = new QTabWidget (main_window_);
        connect (tab_widget_, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
        connect (tab_widget_, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
        tab_widget_->setTabsClosable (true);
    }

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
    resetCurrentFileName (file_ctx->getFileName());
    connect (file_ctx.data(), SIGNAL(fileNameChanged(QString const&)), SLOT(resetCurrentFileName(QString const&)));

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
    //   to be refactored as soon as multi file support is implemented
    main_window_->setCentralWidget (0);
    tab_widget_ = 0;
    tab_contexts_.clear ();
    // --end

    main_window_->setStatusBarSignalLength(-1);
    main_window_->setStatusBarNrChannels(-1);
    resetCurrentFileName ("");
}

//-----------------------------------------------------------------------------
QSharedPointer<SignalVisualisationModel> MainWindowModelImpl::getCurrentSignalVisualisationModel ()
{
    if (!tab_widget_)
        return QSharedPointer<SignalVisualisationModel>(0);

    return browser_models_[tab_widget_->currentIndex()];
}

//-------------------------------------------------------------------------
void MainWindowModelImpl::resetCurrentFileName (QString const& file_name)
{
    if (file_name.size() == 0)
        main_window_->setWindowTitle (tr("SigViewer"));
    else
        main_window_->setWindowTitle (file_name + tr(" - SigViewer"));
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
