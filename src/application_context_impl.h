#ifndef APPLICATION_CONTEXT_IMPL_H
#define APPLICATION_CONTEXT_IMPL_H

#include <QSharedPointer>

namespace BioSig_
{

class MainWindowModel;
class GUIActionManager;
class EventTableFileReader;

class ApplicationContextImpl
{
public:
    //-------------------------------------------------------------------------
    ApplicationContextImpl ();

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

private:
    QSharedPointer<EventTableFileReader> event_table_file_reader_;
    QSharedPointer<MainWindowModel> main_window_model_;
    QSharedPointer<GUIActionManager> gui_action_manager_;
};

} // namespace BioSig_

#endif // APPLICATION_CONTEXT_IMPL_H
