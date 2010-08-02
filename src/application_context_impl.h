#ifndef APPLICATION_CONTEXT_IMPL_H
#define APPLICATION_CONTEXT_IMPL_H

#include "base/application_states.h"
#include "gui/color_manager.h"
#include "gui/main_window_model.h"
#include "gui/application_context.h"
#include "file_context.h"

#include <QObject>
#include <QSharedPointer>

namespace SigViewer_
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
    static QSharedPointer<ApplicationContextImpl> getInstance (bool cleanup = false);

    //-------------------------------------------------------------------------
    static void init (std::set<ApplicationMode> activated_modes);

    //-------------------------------------------------------------------------
    static void cleanup ();

    //-------------------------------------------------------------------------
    ApplicationContextImpl () {}
    virtual ~ApplicationContextImpl ();

    //-------------------------------------------------------------------------
    virtual bool modeActivated (ApplicationMode mode) const;

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
    std::set<ApplicationMode> activated_modes_;
    QSharedPointer<ColorManager> color_manager_;
    QSharedPointer<MainWindowModel> main_window_model_;
    QSharedPointer<FileContext> current_file_context_;
    QSharedPointer<TabContext> current_tab_context_;
    ApplicationState state_;
};

} // namespace SigViewer_

#endif // APPLICATION_CONTEXT_IMPL_H
