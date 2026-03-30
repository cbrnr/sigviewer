// © SigViewer developers
//
// License: GPL-3.0


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
    EditingCommandsTests () : Test (tr("Editing Commands Tests")) {}

    virtual QString run ();
};

}

}

#endif // EDITING_COMMANDS_TESTS_H
