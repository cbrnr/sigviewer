#ifndef GUI_TESTS_H
#define GUI_TESTS_H

#include "test.h"

namespace SigViewer_
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
