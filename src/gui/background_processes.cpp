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
