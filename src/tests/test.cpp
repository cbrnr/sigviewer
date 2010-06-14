#include "test.h"
#include "file_handling_impl/event_manager_impl.h"
#include "file_handling/file_signal_reader_factory.h"
#include "application_context_impl.h"
#include "gui/gui_action_factory.h"

namespace BioSig_
{

namespace Tests_
{

QSharedPointer<EventManager> Test::createEventManagerWithDummyData ()
{
    QSharedPointer<FileSignalReader> reader = FileSignalReaderFactory::getInstance()->getHandler("blub.sinusdummy");
    return QSharedPointer<EventManager> (new EventManagerImpl(reader));
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
