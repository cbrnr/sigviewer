#ifndef UNDO_REDO_GUI_COMMAND_H
#define UNDO_REDO_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"

namespace SigViewer_
{

//-----------------------------------------------------------------------------
class UndoRedoGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    UndoRedoGuiCommand ();

    //-------------------------------------------------------------------------
    virtual ~UndoRedoGuiCommand () {}

    //-------------------------------------------------------------------------
    virtual void init ();

public slots:
    //-------------------------------------------------------------------------
    void undo ();

    //-------------------------------------------------------------------------
    void redo ();

protected:
    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness ();

private:
    //-------------------------------------------------------------------------
    static QString const UNDO_;
    static QString const REDO_;
    static QStringList const ACTIONS_;

    static GuiActionFactoryRegistrator registrator_;
};

}

#endif // UNDO_REDO_GUI_COMMAND_H
