#ifndef APPLICATION_CONTEXT_IMPL_H
#define APPLICATION_CONTEXT_IMPL_H

#include "base/application_states.h"
#include "gui/color_manager.h"
#include "gui/main_window_model.h"
#include "gui/application_context.h"
#include "file_context.h"

#include <QObject>
#include <QSharedPointer>

namespace BioSig_
{

//-----------------------------------------------------------------------------
/// ApplicationContext
///
/// exists once in an application
class ApplicationContextImpl : public QObject, public ApplicationContext
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    static QSharedPointer<ApplicationContext> getContext ();

    //-------------------------------------------------------------------------
    static ApplicationContextImpl* getInstance ();

    //-------------------------------------------------------------------------
    static void init ();

    //-------------------------------------------------------------------------
    static void cleanup ();

    //-------------------------------------------------------------------------
    ApplicationContextImpl () {}
    virtual ~ApplicationContextImpl ();

    //-------------------------------------------------------------------------
    virtual QSharedPointer<FileContext> getCurrentFileContext ();

    //-------------------------------------------------------------------------
    virtual void setCurrentTabContext (QSharedPointer<TabContext> tab_context);

    //-------------------------------------------------------------------------
    virtual QSharedPointer<CommandExecuter> getCurrentCommandExecuter ();

    //-------------------------------------------------------------------------
    /// NO MULTI-FILE SUPPORT IMPLEMENTED YET!!!
    /// THIS CALL WILL REPLACE ACTUAL FILE CONTEXT
    virtual void addFileContext (QSharedPointer<FileContext> file_context);

    //-------------------------------------------------------------------------
    virtual void removeCurrentFileContext ();

    //-------------------------------------------------------------------------
    virtual QSharedPointer<MainWindowModel> getMainWindowModel ();

    //-------------------------------------------------------------------------
    virtual QSharedPointer<ColorManager> getEventColorManager ();

signals:
    void stateChanged (ApplicationState state);
    void currentFileStateChanged (FileState state);
    void currentTabSelectionStateChanged (TabSelectionState state);
    void currentTabEditStateChanged (TabEditState state);

private:
    void setState (ApplicationState state);
    static QSharedPointer<ApplicationContextImpl> instance_;
    QSharedPointer<ColorManager> color_manager_;
    QSharedPointer<MainWindowModel> main_window_model_;
    QSharedPointer<FileContext> current_file_context_;
    QSharedPointer<TabContext> current_tab_context_;
    ApplicationState state_;
};

} // namespace BioSig_

#endif // APPLICATION_CONTEXT_IMPL_H
