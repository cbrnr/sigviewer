#ifndef APPLICATION_CONTEXT_IMPL_INTERFACE_H
#define APPLICATION_CONTEXT_IMPL_INTERFACE_H

#include "gui/color_manager.h"
#include "file_handling_impl/event_table_file_reader.h"
#include "gui/main_window_model.h"

#include <QSharedPointer>

namespace BioSig_
{

class ApplicationContextImplInterface
{
public:
    virtual ~ApplicationContextImplInterface () {}

    //-------------------------------------------------------------------------
    virtual QSharedPointer<MainWindowModel> getMainWindowModel () const = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<EventTableFileReader> getEventTableFileReader () const = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<ColorManager> getEventColorManager () const = 0;

};

}

#endif // APPLICATION_CONTEXT_IMPL_INTERFACE_H
