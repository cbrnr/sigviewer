#include "application_context.h"

namespace BioSig_
{

QSharedPointer<ApplicationContext> ApplicationContext::instance_;

//-----------------------------------------------------------------------------
QSharedPointer<ApplicationContext> ApplicationContext::getInstance ()
{
    if (instance_.isNull())
        instance_ = QSharedPointer<ApplicationContext> (new ApplicationContext ());

    return instance_;
}

//-----------------------------------------------------------------------------
ApplicationContext::ApplicationContext ()
    : impl_ (QSharedPointer<ApplicationContextImplInterface> (0))
{
    // nothing to do here
}


//-------------------------------------------------------------------------
void ApplicationContext::setImpl (QSharedPointer<ApplicationContextImplInterface> impl)
{
    impl_ = impl;
}

//-------------------------------------------------------------------------
QSharedPointer<FileContext> ApplicationContext::getCurrentFileContext () const
{
    return current_file_context_;
}

//-------------------------------------------------------------------------
void ApplicationContext::addFileContext (QSharedPointer<FileContext>file_context)
{
    current_file_context_ = file_context;
    impl_->getGUIActionManager()->connect(current_file_context_.data(), SIGNAL(stateChanged(FileState)), SLOT(setFileState(FileState)));
}

//-------------------------------------------------------------------------
void ApplicationContext::removeCurrentFileContext ()
{
    if (!current_file_context_.isNull())
        current_file_context_->disconnect ();
    current_file_context_ = QSharedPointer<FileContext> (0);
}


//-----------------------------------------------------------------------------
QSharedPointer<GUIActionManager> ApplicationContext::getGUIActionManager ()
{
    return impl_->getGUIActionManager ();
}

//-----------------------------------------------------------------------------
ApplicationState ApplicationContext::getState () const
{
    return state_;
}

//-----------------------------------------------------------------------------
QSharedPointer<MainWindowModel> ApplicationContext::getMainWindowModel () const
{
    return impl_->getMainWindowModel ();
}

//-------------------------------------------------------------------------
QSharedPointer<EventTableFileReader> ApplicationContext::getEventTableFileReader () const
{
    return impl_->getEventTableFileReader ();
}

//-------------------------------------------------------------------------
QSharedPointer<EventColorManager> ApplicationContext::getEventColorManager () const
{
    return impl_->getEventColorManager ();
}



//-----------------------------------------------------------------------------
void ApplicationContext::setState (ApplicationState state)
{
    state_ = state;
    emit stateChanged (state_);
}




} // namespace BioSig_
