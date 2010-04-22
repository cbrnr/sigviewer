#ifndef ZOOM_GUI_COMMAND_H
#define ZOOM_GUI_COMMAND_H

#include "../gui/gui_action_command.h"
#include "../gui/gui_action_factory_registrator.h"

namespace BioSig_
{

class ZoomGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    ZoomGuiCommand ();

    virtual void init ();

public slots:
    //-------------------------------------------------------------------------
    virtual void trigger (QString const& action_name);

    //-------------------------------------------------------------------------
    virtual void applicationStateChanged (ApplicationState state);


private:
    static float const ZOOM_FACTOR_ = 2;

    static QString const ZOOM_IN_VERTICAL_TEXT_;
    static QString const ZOOM_OUT_VERTICAL_TEXT_;
    static QString const ZOOM_IN_HORIZONTAL_TEXT_;
    static QString const ZOOM_OUT_HORIZONTAL_TEXT_;
    static QStringList const TEXTS_;

    static GuiActionFactoryRegistrator registrator_;
};

}

#endif // ZOOM_GUI_COMMAND_H
