// © SigViewer developers
//
// License: GPL-3.0

#ifndef GDF_LOAD_DATA_THREAD_H
#define GDF_LOAD_DATA_THREAD_H

#include <QMutex>
#include <QThread>

namespace SigViewer_ {

class GDFLoadDataThread : public QThread {
    Q_OBJECT
   public:
    explicit GDFLoadDataThread(QVector<double*>& channel_buffers_, QObject* parent = 0);

    void setNextIndex(unsigned sample_index);

   private:
    virtual void run();

    bool currently_loading_;
    unsigned next_index_;
    unsigned current_index_;
    bool running_;

    QMutex current_index_mutex_;
    QMutex next_index_mutex_;
    QMutex currently_loading_mutex_;
};

}  // namespace SigViewer_

#endif  // GDF_LOAD_DATA_THREAD_H
