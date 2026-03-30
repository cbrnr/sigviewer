// © SigViewer developers
//
// License: GPL-3.0


#include "gdf_load_data_thread.h"

#include <QMutexLocker>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
GDFLoadDataThread::GDFLoadDataThread(QObject *parent) :
    QThread(parent)
{
}

//-----------------------------------------------------------------------------
void GDFLoadDataThread::setNextIndex (unsigned sample_index)
{

}

//-----------------------------------------------------------------------------
void GDFLoadDataThread::run ()
{

}


}
