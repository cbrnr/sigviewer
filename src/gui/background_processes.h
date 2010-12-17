#ifndef BACK_GROUND_PROCESSES_H
#define BACK_GROUND_PROCESSES_H

#include <QString>
#include <QObject>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
class BackgroundProcesses : public QObject
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    static BackgroundProcesses& instance ();

    //-------------------------------------------------------------------------
    void addProcess (QString const& name, int max);

    //-------------------------------------------------------------------------
    void setProcessState (QString const& name, int state);

    //-------------------------------------------------------------------------
    void removeProcess (QString const& name);

signals:
    void newProcess (QString name, int max);
    void processChangedState (QString name, int state);
    void processRemoved (QString name);
};

}

#endif // BACK_GROUND_PROCESSES_H
