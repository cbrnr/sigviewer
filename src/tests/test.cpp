// © SigViewer developers
//
// License: GPL-3.0


#include "test.h"
#include "file_handling/event_manager.h"
#include "file_handling/file_channel_manager.h"
#include "file_handling/file_signal_reader_factory.h"
#include "application_context.h"
#include "gui/main_window_model.h"
#include "gui/gui_action_factory.h"

namespace sigviewer
{

namespace Tests_
{

//-------------------------------------------------------------------------
Test::Test (QString const& name) : name_ (name), passed_tests_(0),
    channel_manager_ (new FileChannelManager (FileSignalReaderFactory::getInstance()->getHandler("blub.sinusdummy")))
{

}


//-------------------------------------------------------------------------
ChannelManager const& Test::getChannelManagerDummyData ()
{
    return *channel_manager_;
}

//-------------------------------------------------------------------------
QSharedPointer<EventManager> Test::createEventManagerWithDummyData ()
{
    FileSignalReader* reader = FileSignalReaderFactory::getInstance()->getHandler("blub.sinusdummy");
    QSharedPointer<EventManager> evt_mgr (new EventManager(*reader));
    delete reader;
    return evt_mgr;
}

//-------------------------------------------------------------------------
QSharedPointer<ApplicationContext> Test::applicationContext ()
{
    return ApplicationContext::getInstance();
}

//-------------------------------------------------------------------------
QAction* Test::action (QString const& action_name)
{
    return GuiActionFactory::getInstance()->getQAction(action_name);
}

//-------------------------------------------------------------------------
QSharedPointer<SignalVisualisationModel> Test::currentVisModel ()
{
    return applicationContext()->getMainWindowModel()->getCurrentSignalVisualisationModel();
}

}

}
