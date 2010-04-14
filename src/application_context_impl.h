#ifndef APPLICATION_CONTEXT_IMPL_H
#define APPLICATION_CONTEXT_IMPL_H

#include "event_color_manager.h"
#include "gui_action_manager.h"
#include "main_window_model.h"

#include <QSharedPointer>

namespace BioSig_
{

class EventTableFileReader;

class ApplicationContextImpl
{
public:
    //-------------------------------------------------------------------------
    ApplicationContextImpl ();

    //-------------------------------------------------------------------------
    ~ApplicationContextImpl ();

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
