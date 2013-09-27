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


#include "background_processes.h"

namespace SigViewer_
{

//-------------------------------------------------------------------------
BackgroundProcesses& BackgroundProcesses::instance ()
{
    static BackgroundProcesses instance;
    return instance;
}

//-------------------------------------------------------------------------
void BackgroundProcesses::addProcess (QString const& name, int max)
{
    emit newProcess (name, max);
}

//-------------------------------------------------------------------------
void BackgroundProcesses::setProcessState (QString const& name, int state)
{
    emit processChangedState (name, state);
}

//-------------------------------------------------------------------------
void BackgroundProcesses::removeProcess (QString const& name)
{
    emit processRemoved (name);
}

}
