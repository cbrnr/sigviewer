#include "application_context.h"
#include "application_context_impl.h"

namespace BioSig_
{

ApplicationContext* ApplicationContext::instance_ = 0;

//-----------------------------------------------------------------------------
ApplicationContext* ApplicationContext::getInstance ()
{
    if (!instance_)
        instance_ = new ApplicationContext ();

    return instance_;
}

//-----------------------------------------------------------------------------
ApplicationContext::ApplicationContext ()
    : impl_ (QSharedPointer<ApplicationContextImpl> (0))
{
    // nothing to do here
}


//-------------------------------------------------------------------------
void ApplicationContext::setImpl (QSharedPointer<ApplicationContextImpl> impl)
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


//-----------------------------------------------------------------------------
void ApplicationContext::setState (ApplicationState state)
{
    state_ = state;
    emit stateChanged (state_);
}




} // namespace BioSig_
