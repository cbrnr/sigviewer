#ifndef CHANNEL_SELECTION_GUI_ACTION_COMMAND_H
#define CHANNEL_SELECTION_GUI_ACTION_COMMAND_H

#include "../gui/gui_action_command.h"
#include "../gui/gui_action_factory_registrator.h"

namespace BioSig_
{

class ChannelSelectionGUIActionCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    ChannelSelectionGUIActionCommand ();

    //-------------------------------------------------------------------------
    virtual ~ChannelSelectionGUIActionCommand ();

    //-------------------------------------------------------------------------
    virtual void init ();

public slots:
    //-------------------------------------------------------------------------
    virtual void trigger ();

private:
    static GuiActionFactoryRegistrator registrator_;
};

}

#endif // CHANNEL_SELECTION_GUI_ACTION_COMMAND_H
