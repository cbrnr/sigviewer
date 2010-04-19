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

    //-----------------------------------------------------------------------------
    QSharedPointer<GUIActionManager> getGUIActionManager ();

    //-----------------------------------------------------------------------------
    void setGUIActionManager (QSharedPointer<GUIActionManager> gui_action_manager);

    //-------------------------------------------------------------------------
    QSharedPointer<MainWindowModel> getMainWindowModel () const;

    //-------------------------------------------------------------------------
    void setMainWindowModel (QSharedPointer<MainWindowModel> main_window_model);

    //-------------------------------------------------------------------------
    QSharedPointer<EventTableFileReader> getEventTableFileReader () const;

    //-------------------------------------------------------------------------
    QSharedPointer<EventColorManager> getEventColorManager () const;

    //-------------------------------------------------------------------------
    void loadSettings ();
private:
    QSharedPointer<EventColorManager> event_color_manager_;
    QSharedPointer<EventTableFileReader> event_table_file_reader_;
    QSharedPointer<MainWindowModel> main_window_model_;
    QSharedPointer<GUIActionManager> gui_action_manager_;
};

} // namespace BioSig_

#endif // APPLICATION_CONTEXT_IMPL_H
