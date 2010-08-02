#include "application_context_impl.h"
#include "base/exception.h"

#include "gui_impl/main_window_model_impl.h"

#include <QDebug>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
QSharedPointer<ApplicationContextImpl> ApplicationContextImpl::getInstance (bool cleanup)
{
    static QSharedPointer<ApplicationContextImpl> instance (new ApplicationContextImpl);
    if (cleanup)
        instance.clear();
    if (instance.isNull ())
    {
        instance = QSharedPointer<ApplicationContextImpl> (new ApplicationContextImpl);
        qDebug () << "ApplicationContextImpl::getInstance() created new instance = " << instance.data();
    }
    return instance;
}

//-------------------------------------------------------------------------
void ApplicationContextImpl::init (std::set<ApplicationMode> activated_modes)
{
    qDebug () << "ApplicationContextImpl::init() instance = " << getInstance();

    getInstance()->activated_modes_ = activated_modes;
    getInstance()->setState (APP_STATE_NO_FILE_OPEN);
    if (activated_modes.count (APPLICATION_NON_GUI_MODE) == 0)
    {
        getInstance()->color_manager_ = QSharedPointer<ColorManager> (new ColorManager);
        getInstance()->main_window_model_ = QSharedPointer<MainWindowModel> (new MainWindowModelImpl (getInstance()));
    }
}

//-------------------------------------------------------------------------
void ApplicationContextImpl::cleanup ()
{
    qDebug () << "ApplicationContextImpl::cleanup()";
    getInstance (true);
}


//-----------------------------------------------------------------------------
ApplicationContextImpl::~ApplicationContextImpl ()
{
    qDebug () << "deleting ApplicationContextImpl";
}

//-------------------------------------------------------------------------
bool ApplicationContextImpl::modeActivated (ApplicationMode mode) const
{
    return activated_modes_.count (mode) > 0;
}

//-------------------------------------------------------------------------
QSharedPointer<FileContext> ApplicationContextImpl::getCurrentFileContext ()
{
    return current_file_context_;
}

//-------------------------------------------------------------------------
void ApplicationContextImpl::setCurrentTabContext (QSharedPointer<TabContext> tab_context)
{
    if (!current_tab_context_.isNull())
        current_tab_context_->disconnect (this);
    current_tab_context_ = tab_context;
    if (!connect (current_tab_context_.data(), SIGNAL(selectionStateChanged(TabSelectionState)), SIGNAL(currentTabSelectionStateChanged(TabSelectionState))))
        throw (Exception ("ApplicationContext::setCurrentTabContext failed to connect to selectionStateChanged"));
    if (!connect (current_tab_context_.data(), SIGNAL(editStateChanged(TabEditState)), SIGNAL(currentTabEditStateChanged(TabEditState))))
        throw (Exception ("ApplicationContext::setCurrentTabContext failed to connect to editStateChanged"));
}

//-------------------------------------------------------------------------
QSharedPointer<CommandExecuter> ApplicationContextImpl::getCurrentCommandExecuter ()
{
    return current_tab_context_;
}

//-------------------------------------------------------------------------
void ApplicationContextImpl::addFileContext (QSharedPointer<FileContext>file_context)
{
    current_file_context_ = file_context;
    if (!connect (current_file_context_.data(), SIGNAL(stateChanged(FileState)), SIGNAL(currentFileStateChanged(FileState))))
        throw (Exception ("ApplicationContext::addFileContext faild to connect stateChanged(FileState)"));
    setState (APP_STATE_FILE_OPEN);
}

//-------------------------------------------------------------------------
void ApplicationContextImpl::removeCurrentFileContext ()
{
    if (!current_file_context_.isNull())
        current_file_context_->disconnect ();
    current_file_context_ = QSharedPointer<FileContext> (0);
    setState (APP_STATE_NO_FILE_OPEN);
}

//-----------------------------------------------------------------------------
QSharedPointer<MainWindowModel> ApplicationContextImpl::getMainWindowModel ()
{
    return main_window_model_;
}

//-------------------------------------------------------------------------
QSharedPointer<ColorManager> ApplicationContextImpl::getEventColorManager ()
{
    return color_manager_;
}

//-----------------------------------------------------------------------------
void ApplicationContextImpl::setState (ApplicationState state)
{
    qDebug () << "ApplicationContextImpl::setState " << state << "; this = " << this;
    state_ = state;
    emit stateChanged (state_);
}


} // namespace SigViewer_
