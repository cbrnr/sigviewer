// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef EDITING_COMMANDS_TESTS_H
#define EDITING_COMMANDS_TESTS_H

#include "test.h"

namespace sigviewer
{
namespace Tests_
{
class EditingCommandsTests : public Test
{
public:
    EditingCommandsTests () : Test ("Editing Commands Tests") {}

    virtual QString run ();
};

}

}

#endif // EDITING_COMMANDS_TESTS_H
