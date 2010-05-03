#ifndef VIEW_OPTIONS_GUI_COMMAND_H
#define VIEW_OPTIONS_GUI_COMMAND_H

#include "../gui/gui_action_command.h"
#include "../gui/gui_action_factory_registrator.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
class AdaptChannelViewGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    AdaptChannelViewGuiCommand ();

    //-------------------------------------------------------------------------
    virtual ~AdaptChannelViewGuiCommand () {}

    //-------------------------------------------------------------------------
    virtual void init ();

public slots:
    //-------------------------------------------------------------------------
    void selectShownChannels ();

    //-------------------------------------------------------------------------
    void selectShownEvents ();

private:
    static QString const CHANNELS_;
    static QString const EVENTS_;
    static QStringList const ACTIONS_;

    static GuiActionFactoryRegistrator registrator_;
};

}

#endif // VIEW_OPTIONS_GUI_COMMAND_H
