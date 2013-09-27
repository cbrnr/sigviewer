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
