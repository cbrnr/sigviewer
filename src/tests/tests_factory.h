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


#ifndef TESTS_FACTORY_H
#define TESTS_FACTORY_H

#include "test.h"
#include "event_manager_tests.h"
#include "data_block_tests.h"
#include "editing_commands_tests.h"
#include "gui_tests.h"
#include "color_manager_tests.h"
#include "file_handling_tests.h"
#include "event_table_widget_tests.h"

#include <QSharedPointer>
#include <QList>

namespace sigviewer
{

namespace TestsFactory_
{

//-----------------------------------------------------------------------------
QList<QSharedPointer<Tests_::Test> > allTests ()
{
    QList<QSharedPointer<Tests_::Test> > tests;
    tests.append (QSharedPointer<Tests_::Test>(new Tests_::DataBlockTests));
    tests.append (QSharedPointer<Tests_::Test>(new Tests_::ColorManagerTests));
    tests.append (QSharedPointer<Tests_::Test>(new Tests_::EventManagerTests));
    tests.append (QSharedPointer<Tests_::Test>(new Tests_::EditingCommandsTests));
    tests.append (QSharedPointer<Tests_::Test>(new Tests_::GuiTests));
    tests.append (QSharedPointer<Tests_::Test>(new Tests_::FileHandlingTests));
    tests.append (QSharedPointer<Tests_::Test>(new Tests_::EventTableWidgetTests));

    return tests;
}

}

}

#endif // TESTS_FACTORY_H
