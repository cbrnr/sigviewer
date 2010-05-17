#include "application_context_impl.h"
#include "file_handling_impl/event_table_file_reader.h"
#include "gui/color_manager.h"

#include <QDebug>

namespace BioSig_
{

//-----------------------------------------------------------------------------
ApplicationContextImpl::ApplicationContextImpl ()
    : event_color_manager_ (new ColorManager ()),
      event_table_file_reader_ (new EventTableFileReader ())
{
    event_table_file_reader_->load (":/eventcodes.txt");
}

//-------------------------------------------------------------------------
ApplicationContextImpl::~ApplicationContextImpl ()
{
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
QSharedPointer<ColorManager> ApplicationContextImpl::getEventColorManager () const
{
    qDebug () << "ApplicationContextImpl::getEventColorManager event_color_manager_.data = " << event_color_manager_.data();
    return event_color_manager_;
}

//-------------------------------------------------------------------------
void ApplicationContextImpl::loadSettings ()
{
    event_color_manager_->loadSettings();
}



} // namespace BioSig_
