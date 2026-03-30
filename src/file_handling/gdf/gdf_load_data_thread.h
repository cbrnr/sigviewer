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


#ifndef GDF_LOAD_DATA_THREAD_H
#define GDF_LOAD_DATA_THREAD_H

#include <QThread>
#include <QMutex>

namespace SigViewer_
{

class GDFLoadDataThread : public QThread
{
    Q_OBJECT
public:
    explicit GDFLoadDataThread (QVector<double*>& channel_buffers_, QObject *parent = 0);

    void setNextIndex (unsigned sample_index);

private:
    virtual void run ();

    bool currently_loading_;
    unsigned next_index_;
    unsigned current_index_;
    bool running_;

    QMutex current_index_mutex_;
    QMutex next_index_mutex_;
    QMutex currently_loading_mutex_;
};

}

#endif // GDF_LOAD_DATA_THREAD_H
