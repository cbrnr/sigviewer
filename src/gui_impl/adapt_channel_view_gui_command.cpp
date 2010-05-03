#include "adapt_channel_view_gui_command.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
QString const AdaptChannelViewGuiCommand::CHANNELS_ = "Channels...";
QString const AdaptChannelViewGuiCommand::EVENTS_ = "Events...";
QStringList const AdaptChannelViewGuiCommand::ACTIONS_;

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator registrator_ ("View Options",
                                          QSharedPointer<AdaptChannelViewGuiCommand> (new AdaptChannelViewGuiCommand));

//-----------------------------------------------------------------------------
AdaptChannelViewGuiCommand::AdaptChannelViewGuiCommand ()
    : GuiActionCommand (ACTIONS_)
{
    // nothing to do here
}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::init ()
{

}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::selectShownChannels ()
{

}

//-------------------------------------------------------------------------
void AdaptChannelViewGuiCommand::selectShownEvents ()
{

}

}
