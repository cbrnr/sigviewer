#ifndef SAVE_GUI_COMMAND_H
#define SAVE_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"

namespace SigViewer_
{

//-----------------------------------------------------------------------------
/// SaveGuiCommand
///
/// command for saving a file and converting to other file format
class SaveGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    SaveGuiCommand ();

    //-------------------------------------------------------------------------
    virtual void init ();

public slots:
    //-------------------------------------------------------------------------
    void saveAs ();

    //-------------------------------------------------------------------------
    void save ();

    //-------------------------------------------------------------------------
    void exportToPNG ();

    //-------------------------------------------------------------------------
    void exportToGDF ();

    //-------------------------------------------------------------------------
    void exportEvents ();

protected:
    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness ();


private:
    //-------------------------------------------------------------------------


    static QString const SAVE_AS_;
    static QString const SAVE_;
    static QString const EXPORT_TO_PNG_;
    static QString const EXPORT_TO_GDF_;
    static QString const EXPORT_EVENTS_;
    static QStringList const ACTIONS_;

    static GuiActionFactoryRegistrator registrator_;
};

}

#endif // SAVE_GUI_COMMAND_H
