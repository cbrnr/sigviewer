// © SigViewer developers
//
// License: GPL-3.0

#ifndef TAB_CONTEXT_H
#define TAB_CONTEXT_H

#include <QObject>
#include <QUndoCommand>
#include <QUndoStack>

#include "base/tab_states.h"
#include "command_executer.h"

namespace sigviewer {

class TabContext : public QObject, public CommandExecuter {
    Q_OBJECT
   public:
    TabContext();

    ~TabContext();

    void gotActive();

    virtual void executeCommand(QUndoCommand* command);

    virtual void undo();

    virtual void redo();

   signals:

    void selectionStateChanged(TabSelectionState state);

    void editStateChanged(TabEditState state);

   public slots:

    void setSelectionState(TabSelectionState state);

    void setEditState(TabEditState state);

   private:
    // disabled
    TabContext(TabContext const&);
    TabContext& operator=(TabContext const&);

    void updateUndoRedoEditState();

    TabSelectionState selection_state_;
    TabEditState edit_state_;

    QUndoStack edit_undo_stack_;
};

}  // namespace sigviewer

#endif  // TAB_CONTEXT_H
