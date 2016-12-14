#include "color_manager_tests.h"

#include "gui/color_manager.h"

namespace SigViewer_
{
namespace Tests_
{


//-----------------------------------------------------------------------------
ColorManagerTests::ColorManagerTests ()
    : Test (tr("ColorManager Tests"))
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
QString ColorManagerTests::run ()
{
    QSharedPointer<ColorManager const> color_manager = applicationContext ()->getEventColorManager();
    VERIFY (color_manager.isNull() == false, "initialized");

    VERIFY (color_manager->getDefaultEventColor() == QColor (200, 0, 0, 30), "default color");
    VERIFY (color_manager->getDefaultEventColor(0x0300) == QColor (0, 0, 200, 30), "Start of Trial color");

    return "";
}

}
}
