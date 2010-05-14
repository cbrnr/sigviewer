#ifndef APPLICATION_CONTEXT_IMPL_H
#define APPLICATION_CONTEXT_IMPL_H

#include "application_context_impl_interface.h"

namespace BioSig_
{

class ApplicationContextImpl : public ApplicationContextImplInterface
{
public:
    //-------------------------------------------------------------------------
    ApplicationContextImpl ();

    //-------------------------------------------------------------------------
    virtual ~ApplicationContextImpl ();

    //-------------------------------------------------------------------------
    QSharedPointer<MainWindowModel> getMainWindowModel () const;

    //-------------------------------------------------------------------------
    void setMainWindowModel (QSharedPointer<MainWindowModel> main_window_model);

    //-------------------------------------------------------------------------
    QSharedPointer<EventTableFileReader> getEventTableFileReader () const;

    //-------------------------------------------------------------------------
    QSharedPointer<ColorManager> getEventColorManager () const;

    //-------------------------------------------------------------------------
    void loadSettings ();
private:
    QSharedPointer<ColorManager> event_color_manager_;
    QSharedPointer<EventTableFileReader> event_table_file_reader_;
    QSharedPointer<MainWindowModel> main_window_model_;
};

} // namespace BioSig_

#endif // APPLICATION_CONTEXT_IMPL_H
