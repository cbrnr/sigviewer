#include "application_context_impl.h"
#include "file_handling_impl/event_table_file_reader.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
ApplicationContextImpl::ApplicationContextImpl ()
    : event_table_file_reader_ (new EventTableFileReader ())
{
    event_table_file_reader_->load (":/eventcodes.txt");
}

//-----------------------------------------------------------------------------
QSharedPointer<GUIActionManager> ApplicationContextImpl::getGUIActionManager ()
{
    return gui_action_manager_;
}

//-----------------------------------------------------------------------------
void ApplicationContextImpl::setGUIActionManager (QSharedPointer<GUIActionManager> gui_action_manager)
{
    gui_action_manager_ = gui_action_manager;
}

//-----------------------------------------------------------------------------
QSharedPointer<MainWindowModel> ApplicationContextImpl::getMainWindowModel () const
{
    return main_window_model_;
}

//-----------------------------------------------------------------------------
void ApplicationContextImpl::setMainWindowModel (QSharedPointer<MainWindowModel> main_window_model)
{
    main_window_model_ = main_window_model;
}

//-----------------------------------------------------------------------------
QSharedPointer<EventTableFileReader> ApplicationContextImpl::getEventTableFileReader () const
{
    return event_table_file_reader_;
}


} // namespace BioSig_
