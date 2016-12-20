// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


#include "test.h"
#include "file_handling_impl/event_manager_impl.h"
#include "file_handling_impl/channel_manager_impl.h"
#include "file_handling/file_signal_reader_factory.h"
#include "application_context_impl.h"
#include "gui/gui_action_factory.h"

namespace sigviewer
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
