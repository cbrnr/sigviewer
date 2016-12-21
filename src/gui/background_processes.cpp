// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "background_processes.h"

namespace sigviewer
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
