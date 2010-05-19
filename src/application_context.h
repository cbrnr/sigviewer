#ifndef APPLICATION_CONTEXT_H
#define APPLICATION_CONTEXT_H

#include "base/application_states.h"
#include "gui/color_manager.h"
#include "gui/main_window_model.h"
#include "file_context.h"
#include "tab_context.h"

#include <QObject>
#include <QSharedPointer>

namespace BioSig_
{

//-----------------------------------------------------------------------------
/// ApplicationContext
///
/// exists once in an application
class ApplicationContext : public QObject
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    static ApplicationContext* getInstance ();

    //-------------------------------------------------------------------------
    static void init ();

    //-------------------------------------------------------------------------
    static void cleanup ();

    //-------------------------------------------------------------------------
    ApplicationContext () {}
    ~ApplicationContext ();

    //-------------------------------------------------------------------------
    QSharedPointer<FileContext> getCurrentFileContext () const;

    //-------------------------------------------------------------------------
    void setCurrentTabContext (QSharedPointer<TabContext> tab_context);

    //-------------------------------------------------------------------------
    QSharedPointer<CommandExecuter> getCurrentCommandExecuter ();

    //-------------------------------------------------------------------------
    /// NO MULTI-FILE SUPPORT IMPLEMENTED YET!!!
    /// THIS CALL WILL REPLACE ACTUAL FILE CONTEXT
    void addFileContext (QSharedPointer<FileContext> file_context);

    //-------------------------------------------------------------------------
    void removeCurrentFileContext ();

    //-------------------------------------------------------------------------
    ApplicationState getState () const;

    //-------------------------------------------------------------------------
    QSharedPointer<MainWindowModel> getMainWindowModel () const;

    //-------------------------------------------------------------------------
    QSharedPointer<ColorManager> getEventColorManager () const;

public slots:
    void setState (ApplicationState state);

signals:
    void stateChanged (ApplicationState state);
    void currentFileStateChanged (FileState state);
    void currentTabSelectionStateChanged (TabSelectionState state);
    void currentTabEditStateChanged (TabEditState state);

private:
    static ApplicationContext* instance_;
    QSharedPointer<ColorManager> color_manager_;
    QSharedPointer<MainWindowModel> main_window_model_;
    QSharedPointer<FileContext> current_file_context_;
    QSharedPointer<TabContext> current_tab_context_;
    ApplicationState state_;
};

} // namespace BioSig_

#endif // APPLICATION_CONTEXT_H
