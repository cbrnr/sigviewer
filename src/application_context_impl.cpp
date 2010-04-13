#include "application_context_impl.h"
#include "file_handling_impl/event_table_file_reader.h"
#include "event_color_manager.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
ApplicationContextImpl::ApplicationContextImpl ()
    : event_color_manager_ (new EventColorManager ()),
      event_table_file_reader_ (new EventTableFileReader ())
{
    event_table_file_reader_->load (":/eventcodes.txt");
}

//-------------------------------------------------------------------------
ApplicationContextImpl::~ApplicationContextImpl ()
{
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

//-----------------------------------------------------------------------------
QSharedPointer<EventColorManager> ApplicationContextImpl::getEventColorManager () const
{
    return event_color_manager_;
}

//-------------------------------------------------------------------------
void ApplicationContextImpl::loadSettings ()
{
    event_color_manager_->loadSettings();
}



} // namespace BioSig_
