// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef GUI_TESTS_H
#define GUI_TESTS_H

#include "test.h"

namespace sigviewer
{
namespace Tests_
{

class GuiTests : public Test
{
public:
    GuiTests() : Test (tr("GUI Tests")) {}
    virtual void init ();
    virtual QString run ();
    virtual void cleanup ();
private:
    QString testZooming ();
    QString testEventEditing ();
    QString testEnablednessNoOpenFile ();

    bool animations_triggered_;
};

}

}

#endif // GUI_TESTS_H
