#include "test.h"
#include "file_handling_impl/event_manager_impl.h"
#include "file_handling_impl/channel_manager_impl.h"
#include "file_handling/file_signal_reader_factory.h"
#include "application_context_impl.h"
#include "gui/gui_action_factory.h"

namespace SigViewer_
{

namespace Tests_
{

//-------------------------------------------------------------------------
Test::Test (QString const& name) : name_ (name), passed_tests_(0),
    channel_manager_ (new ChannelManagerImpl (FileSignalReaderFactory::getInstance()->getHandler("blub.sinusdummy")))
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
    QSharedPointer<EventManager> evt_mgr (new EventManagerImpl(*reader));
    delete reader;
    return evt_mgr;
}

//-------------------------------------------------------------------------
QSharedPointer<ApplicationContext> Test::applicationContext ()
{
    return ApplicationContextImpl::getInstance();
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
