#include "application_context.h"
#include "base/exception.h"

#include "main_window_model_impl.h"

#include <QDebug>

namespace BioSig_
{

ApplicationContext* ApplicationContext::instance_ = 0;

//-----------------------------------------------------------------------------
ApplicationContext* ApplicationContext::getInstance ()
{
    if (!instance_)
        instance_ = new ApplicationContext;

    return instance_;
}

//-------------------------------------------------------------------------
void ApplicationContext::init ()
{
    if (!instance_)
        instance_ = new ApplicationContext;

    instance_->color_manager_ = QSharedPointer<ColorManager> (new ColorManager);
    instance_->main_window_model_ = QSharedPointer<MainWindowModel> (new MainWindowModelImpl);
    instance_->setState (APP_STATE_NO_FILE_OPEN);
}

//-------------------------------------------------------------------------
void ApplicationContext::cleanup ()
{
    delete instance_;
    instance_ = 0;
}


//-----------------------------------------------------------------------------
ApplicationContext::~ApplicationContext ()
{
    qDebug () << "deleting ApplicationContext";
}

//-------------------------------------------------------------------------
QSharedPointer<FileContext> ApplicationContext::getCurrentFileContext () const
{
    return current_file_context_;
}

//-------------------------------------------------------------------------
void ApplicationContext::setCurrentTabContext (QSharedPointer<TabContext> tab_context)
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
QSharedPointer<CommandExecuter> ApplicationContext::getCurrentCommandExecuter ()
{
    return current_tab_context_;
}

//-------------------------------------------------------------------------
void ApplicationContext::addFileContext (QSharedPointer<FileContext>file_context)
{
    current_file_context_ = file_context;
    if (!connect (current_file_context_.data(), SIGNAL(stateChanged(FileState)), SIGNAL(currentFileStateChanged(FileState))))
        throw (Exception ("ApplicationContext::addFileContext faild to connect stateChanged(FileState)"));
}

//-------------------------------------------------------------------------
void ApplicationContext::removeCurrentFileContext ()
{
    if (!current_file_context_.isNull())
        current_file_context_->disconnect ();
    current_file_context_ = QSharedPointer<FileContext> (0);
}

//-----------------------------------------------------------------------------
ApplicationState ApplicationContext::getState () const
{
    return state_;
}

//-----------------------------------------------------------------------------
QSharedPointer<MainWindowModel> ApplicationContext::getMainWindowModel () const
{
    return main_window_model_;
}

//-------------------------------------------------------------------------
QSharedPointer<ColorManager> ApplicationContext::getEventColorManager () const
{
    return color_manager_;
}

//-----------------------------------------------------------------------------
void ApplicationContext::setState (ApplicationState state)
{
    state_ = state;
    emit stateChanged (state_);
}


} // namespace BioSig_
